#include "todus.hpp"
#ifdef Q_OS_UNIX
#include "todus_unix.pb.h"
#endif
#ifdef Q_OS_WIN
#include "todus_windows.pb.h"
#endif
#include "main.hpp"
#include <ctime>
#include <QTimer>
#include <QByteArray>
#include <QSettings>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QRandomGenerator>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QNetworkReply>
#include <QInputDialog>


toDus::toDus(QObject *padre)
	: QObject(padre), _progresoInicioSesion(ProgresoInicioSesion::Ninguno), _contadorComandos(1), _desconexionSolicitada(false), _fichaAccesoRenovada(false) {
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	_administradorAccesoRed = new QNetworkAccessManager(this);
	_socaloSSL = new QSslSocket(this);
	_idSesion = generarIDSesion(5);
	_socaloSSL->setPeerVerifyMode(QSslSocket::VerifyNone);
	_temporizadorMantenerSesionActiva.setInterval(60000);

	connect(_socaloSSL, &QSslSocket::stateChanged, this, &toDus::eventoCambiarEstado);
	connect(_socaloSSL, &QSslSocket::encrypted, this, &toDus::eventoConexionLista);
	connect(_socaloSSL, &QSslSocket::readyRead, this, &toDus::eventoDatosRecibidos);
	connect(&_temporizadorMantenerSesionActiva, &QTimer::timeout, this, &toDus::xmppMantenerSesionActiva);
}
toDus::~toDus() {
	google::protobuf::ShutdownProtobufLibrary();
}

/**
 * @brief Obtiene el estado de la sesión toDus
 * @return Devuelve el estado de la sesión toDus
 */
toDus::Estado toDus::obtenerEstado() {
	return _estado;
}

/**
 * @brief Desconecta de toDus
 */
void toDus::desconectar() {
	_desconexionSolicitada = true;
	_socaloSSL->disconnectFromHost();
}

/**
 * @brief Reconectar
 */
void toDus::reconectar() {
	if (_socaloSSL->state() != QAbstractSocket::UnconnectedState) {
		_desconexionSolicitada = false;
		_socaloSSL->disconnectFromHost();
	} else {
		iniciarSesion();
	}
}

/**
 * @brief Inicia la sesión en toDus partiendo del número de teléfono o ficha de acceso configurada
 */
void toDus::iniciarSesion() {
	QSettings configuracion;
	QString telefono = configuracion.value("todus/telefono").toString();
	QString fichaAcceso = configuracion.value("todus/fichaAcceso").toString();
	qint32 fichaAccesoTiempoExpiracion = configuracion.value("todus/fichaAccesoTiempoExpiracion").toInt();
	std::time_t tiempoActual = std::time(nullptr);

	_administradorAccesoRed->setProxy(obtenerConfiguracionProxy());
	_socaloSSL->setProxy(obtenerConfiguracionProxy());

	_idInicioSesion = generarIDSesion(150);
	_fichaAccesoRenovada = false;
	_desconexionSolicitada = false;

	if (fichaAcceso.size() > 0) {
		if (tiempoActual > fichaAccesoTiempoExpiracion) {
			solicitarFichaAcceso();
		} else {
			iniciarSesionConFichaAcceso();
		}
	} else if (telefono.size() > 0) {
		telefono.remove(0, 1);
		solicitarCodigoSMS(telefono);
	}
}

/**
 * @brief Solicita a toDus el enlace firmado para obtener un archivo
 * @param enlace Enlace no firmado
 */
void toDus::solicitarEnlaceFirmado(const QString &enlace, std::function<void(const QString &)> retroalimentador) {
	if (_progresoInicioSesion == ProgresoInicioSesion::SesionIniciada) {
		xmppSolicitarEnlaceDescarga(enlace, std::move(retroalimentador));
	}
}

void toDus::solicitarEnlaceFirmadoSubida() {
	if (_progresoInicioSesion == ProgresoInicioSesion::SesionIniciada) {
		xmppSolicitarEnlaceSubida();
	}
}

void toDus::eventoFinalizadaCodigoSMS() {
	if (_respuestaCodigoSMS->error() == QNetworkReply::NetworkError::NoError) {
		QSettings configuracion;
		bool ok = false;
		QString codigo = QInputDialog::getText(nullptr, "Código de verificación", "Código de verificación:", QLineEdit::Normal, "", &ok);

		if (ok == true) {
			configuracion.setValue("todus/codigoVerificacion", codigo);
			solicitarFichaSolicitud(codigo);
		}
	}

//	_respuestaCodigoSMS->deleteLater();
}

void toDus::eventoRecepcionDatosFichaSolicitud() {
	QSettings configuracion;
	QByteArray fichaSolicitudCruda = configuracion.value("todus/fichaSolicitudCruda").toByteArray();

	fichaSolicitudCruda += _respuestaFichaSolicitud->readAll();
	configuracion.setValue("todus/fichaSolicitudCruda", fichaSolicitudCruda);
}

void toDus::eventoFinalizadaFichaSolicitud() {
	if (_respuestaFichaSolicitud->error() == QNetworkReply::NetworkError::NoError) {
		QSettings configuracion;
		QByteArray fichaSolicitudCruda = configuracion.value("todus/fichaSolicitudCruda").toByteArray();
		toDusPB::RecepcionFichaSolicitud pbRecepcionFichaSolicitud;

		if (pbRecepcionFichaSolicitud.ParseFromArray(fichaSolicitudCruda.constData(), fichaSolicitudCruda.size()) == true) {
			configuracion.setValue("todus/fichaSolicitud", pbRecepcionFichaSolicitud.fichasolicitud().c_str());
			if (pbRecepcionFichaSolicitud.usuario().has_nombre() == true) {
				configuracion.setValue("todus/nombre", pbRecepcionFichaSolicitud.usuario().nombre().c_str());
			}
			if (pbRecepcionFichaSolicitud.usuario().has_enlacefoto() == true) {
				configuracion.setValue("todus/enlaceAvatar", pbRecepcionFichaSolicitud.usuario().enlacefoto().c_str());
			}

			configuracion.remove("todus/fichaSolicitudCruda");
			solicitarFichaAcceso();
		}
	}

//	_respuestaFichaSolicitud->deleteLater();
}

void toDus::eventoRecepcionDatosFichaAcceso() {
	QSettings configuracion;
	QByteArray fichaAccesoCruda = configuracion.value("todus/fichaAccesoCruda").toByteArray();

	fichaAccesoCruda += _respuestaFichaAcceso->readAll();
	configuracion.setValue("todus/fichaAccesoCruda", fichaAccesoCruda);
}

void toDus::eventoFinalizadaFichaAcceso() {
	if (_respuestaFichaAcceso->error() == QNetworkReply::NetworkError::NoError) {
		QSettings configuracion;
		QByteArray fichaAccesoCruda = configuracion.value("todus/fichaAccesoCruda").toByteArray();
		toDusPB::RecepcionFichaAcceso pbRecepcionFichaAcceso;

		if (pbRecepcionFichaAcceso.ParseFromArray(fichaAccesoCruda.constData(), fichaAccesoCruda.size()) == true) {
			configuracion.setValue("todus/fichaAcceso", pbRecepcionFichaAcceso.fichaacceso().c_str());
			configuracion.setValue("todus/fichaAccesoTiempoExpiracion", pbRecepcionFichaAcceso.tiempoexpiracion());

			configuracion.remove("todus/fichaAccesoCruda");
			iniciarSesionConFichaAcceso();
		}
	}

	//_respuestaFichaSolicitud->deleteLater();
}

void toDus::eventoError(QNetworkReply::NetworkError codigo) {
	std::cout << "toDus::eventoError() - " << codigo << std::endl;
	Q_UNUSED(codigo);
}

void toDus::eventoCambiarEstado(QAbstractSocket::SocketState estado) {
	switch (estado) {
		case QAbstractSocket::UnconnectedState:
			_estado = Estado::Desconectado;
			if (_desconexionSolicitada == false) {
				QTimer::singleShot(2000, this, &toDus::iniciarSesion);
			}
			break;
		case QAbstractSocket::HostLookupState:
			_estado = Estado::ResolviendoNombreDNS;
			break;
		case QAbstractSocket::ConnectingState:
			_estado = Estado::Conectando;
			break;
		case QAbstractSocket::ConnectedState:
			_estado = Estado::Conectado;
			break;
		case QAbstractSocket::ClosingState:
			_temporizadorMantenerSesionActiva.stop();
			_estado = Estado::Desconectando;
			break;
		default:
			break;
	}

	emit estadoCambiado(_estado);
}

void toDus::eventoConexionLista() {
	_estado = Estado::IniciandoSesion;
	emit estadoCambiado(_estado);
	_progresoInicioSesion = ProgresoInicioSesion::Saludo;
	xmppSaludar();
}

void toDus::eventoDatosRecibidos() {
	QByteArray bufer;
	QRegularExpression re;
	QRegularExpressionMatch rem;

	while (_socaloSSL->bytesAvailable() > 0) {
		bufer = _socaloSSL->readAll();

		switch (_progresoInicioSesion) {
			case ProgresoInicioSesion::Ninguno:
				break;
			case ProgresoInicioSesion::Saludo: // Respuesta al saludo
				re = QRegularExpression("<stream:features>.+<e>PLAIN</e>.+</stream:features>");
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
					_progresoInicioSesion = ProgresoInicioSesion::MecanismosAutentificacion;
					xmppIniciarSesion();
				}
				break;
			case ProgresoInicioSesion::MecanismosAutentificacion: // Respuesta al intento de autentificación
				re = QRegularExpression("<ok xmlns='x2'/>");
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
					_progresoInicioSesion = ProgresoInicioSesion::Autenficicacion;
					xmppSaludar();
					xmppEstablecerSesion();
				} else {
					re = QRegularExpression("<failure xmlns='x2'><not-authorized/>.+</failure>");
					rem = re.match(bufer);
					if (rem.hasMatch() == true) {
						_progresoInicioSesion = ProgresoInicioSesion::Ninguno;
						desconectar();

						if (_fichaAccesoRenovada == false) {
							QSettings configuracion;

							_fichaAccesoRenovada = true;
							solicitarFichaSolicitud(configuracion.value("todus/codigoVerificacion").toString());
						}
					}
				}
				break;
			case ProgresoInicioSesion::Autenficicacion: // Respuesta satisfactoria al intento de establecer sesión
				re = QRegularExpression("<iq t='result' i='" + _idSesion + "-" + QString::fromStdString(std::to_string(_contadorComandos - 1)) + "'>.+<jid>(.+)</jid>.+</iq>");
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
					_progresoInicioSesion = ProgresoInicioSesion::SesionIniciada;
					_jID = rem.captured(1);
					_temporizadorMantenerSesionActiva.start();
					_desconexionSolicitada = false;
					_estado = Estado::Listo;
					emit estadoCambiado(_estado);
				}
				break;
			case ProgresoInicioSesion::SesionIniciada: // Respuesta a algún comando
				// La respuesta está dirigida a esta sesión?
				re = QRegularExpression("<iq.+o='" + _jID + "'.+t='result'.+>.+</iq>");
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
					// Comando: GURL
					re = QRegularExpression("<iq.+i='(.+)'.*><query xmlns='todus:gurl' du='(.+)' status='200'/></iq>");
					rem = re.match(bufer);
					if (rem.hasMatch() == true) {
						QString idSesion = rem.captured(1);
						if (_listadoRetroalimentadores.find(idSesion) != _listadoRetroalimentadores.end()) {
							_listadoRetroalimentadores[idSesion](rem.captured(2));
							_listadoRetroalimentadores.remove(idSesion);
						}
					} else {
						// Comando: PURL
						re = QRegularExpression("<iq.+i='(.+)'.*><query xmlns='todus:purl' put='(.+)' get='(.+)' status='200'/></iq>");
						rem = re.match(bufer);
						if (rem.hasMatch() == true) {
							QString idSesion = rem.captured(1);
							QString enlaceFirmadoSubida = rem.captured(2);
							QString enlaceNoFirmadoDescarga = rem.captured(3);
							//_listadoEnlacesFirmados.remove(idSesion);
							//emit enlaceFirmadoObtenido(enlaceNoFirmado, re.cap(2));
							std::cout << enlaceFirmadoSubida.replace("&amp;", "&").toStdString() << std::endl;
							std::cout << enlaceNoFirmadoDescarga.toStdString() << std::endl;
						}
					}
				}
				break;
		}
	}
}

QString toDus::generarIDSesion(unsigned int totalCaracteres) {
	QString idSesion;
	int codigoAleatorio = 0;
	unsigned int caractersObtenidos = 0;

	while (caractersObtenidos < totalCaracteres) {
		codigoAleatorio = QRandomGenerator::global()->bounded(48, 122);
		if (codigoAleatorio >= 48 && codigoAleatorio <= 57) {
			idSesion.push_back(QChar(codigoAleatorio));
			caractersObtenidos++;
			continue;
		}
		if (codigoAleatorio >= 65 && codigoAleatorio <= 90) {
			idSesion.push_back(QChar(codigoAleatorio));
			caractersObtenidos++;
			continue;
		}
		if (codigoAleatorio >= 97 && codigoAleatorio <= 122) {
			idSesion.push_back(QChar(codigoAleatorio));
			caractersObtenidos++;
			continue;
		}
	}

	return idSesion;
}

void toDus::solicitarCodigoSMS(const QString &telefono) {
	QSettings configuracion;
	QString ipServidorAutentificacion = configuracion.value("avanzadas/ipServidorAutentificacion", "").toString();
	QString nombreDNSServidorAutentificacion = configuracion.value("avanzadas/nombreDNSServidorAutentificacion", "auth.todus.cu").toString();
	int puertoServidorAutentificacion = configuracion.value("avanzadas/puertoServidorAutentificacion", 443).toInt();
	QUrl url = QUrl("https://" + nombreDNSServidorAutentificacion + "/v2/auth/users.reserve");
	QNetworkRequest solicitud;
	QSslConfiguration configuracionSSL = QSslConfiguration::defaultConfiguration();
	std::string datos;
	toDusPB::SolicitudSMS pbSolicitudSMS;

	pbSolicitudSMS.set_telefono(telefono.toStdString());
	pbSolicitudSMS.set_id(_idInicioSesion.toStdString());
	pbSolicitudSMS.SerializeToString(&datos);

	configuracionSSL.setPeerVerifyMode(QSslSocket::VerifyNone);
	solicitud.setSslConfiguration(configuracionSSL);
	solicitud.setAttribute(QNetworkRequest::AutoDeleteReplyOnFinishAttribute, true);
	solicitud.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
	solicitud.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);

	if (ipServidorAutentificacion.size() > 0) {
		url.setHost(ipServidorAutentificacion);
	}
	url.setPort(puertoServidorAutentificacion);

	solicitud.setUrl(url);
	solicitud.setHeader(QNetworkRequest::UserAgentHeader, configuracion.value("avanzadas/agenteUsuario", _agenteUsuarioTodus).toString() + " Auth");
	solicitud.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-protobuf");
	solicitud.setHeader(QNetworkRequest::ContentLengthHeader, (int)datos.size());
	solicitud.setRawHeader("Host", nombreDNSServidorAutentificacion.toLocal8Bit());

	_respuestaCodigoSMS = _administradorAccesoRed->post(solicitud, datos.c_str());
	connect(_respuestaCodigoSMS, &QNetworkReply::errorOccurred, this, &toDus::eventoError);
	connect(_respuestaCodigoSMS, &QNetworkReply::finished, this, &toDus::eventoFinalizadaCodigoSMS);
}

void toDus::solicitarFichaSolicitud(const QString &codigo) {
	QSettings configuracion;
	QString telefono = configuracion.value("todus/telefono").toString();
	QString ipServidorAutentificacion = configuracion.value("avanzadas/ipServidorAutentificacion", "").toString();
	QString nombreDNSServidorAutentificacion = configuracion.value("avanzadas/nombreDNSServidorAutentificacion", "auth.todus.cu").toString();
	int puertoServidorAutentificacion = configuracion.value("avanzadas/puertoServidorAutentificacion", 443).toInt();
	QUrl url = QUrl("https://" + nombreDNSServidorAutentificacion + "/v2/auth/users.register");
	QNetworkRequest solicitud;
	QSslConfiguration configuracionSSL = QSslConfiguration::defaultConfiguration();
	std::string datos;
	toDusPB::SolicitudFichaSolicitud pbSolicitudFichaSolicitud;

	telefono.remove(0, 1);
	pbSolicitudFichaSolicitud.set_telefono(telefono.toStdString());
	pbSolicitudFichaSolicitud.set_id(_idInicioSesion.toStdString());
	pbSolicitudFichaSolicitud.set_codigo(codigo.toStdString());
	pbSolicitudFichaSolicitud.SerializeToString(&datos);

	configuracionSSL.setPeerVerifyMode(QSslSocket::VerifyNone);
	solicitud.setSslConfiguration(configuracionSSL);
	solicitud.setAttribute(QNetworkRequest::AutoDeleteReplyOnFinishAttribute, true);
	solicitud.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
	solicitud.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);

	if (ipServidorAutentificacion.size() > 0) {
		url.setHost(ipServidorAutentificacion);
	}
	url.setPort(puertoServidorAutentificacion);

	solicitud.setUrl(url);
	solicitud.setHeader(QNetworkRequest::UserAgentHeader, configuracion.value("avanzadas/agenteUsuario", _agenteUsuarioTodus).toString() + " Auth");
	solicitud.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-protobuf");
	solicitud.setHeader(QNetworkRequest::ContentLengthHeader, (int)datos.size());
	solicitud.setRawHeader("Host", nombreDNSServidorAutentificacion.toLocal8Bit());

	configuracion.remove("todus/fichaSolicitud");
	configuracion.remove("todus/fichaSolicitudCruda");
	configuracion.remove("todus/nombre");
	configuracion.remove("todus/enlaceAvatar");

	_respuestaFichaSolicitud = _administradorAccesoRed->post(solicitud, datos.c_str());
	connect(_respuestaFichaSolicitud, &QIODevice::readyRead, this, &toDus::eventoRecepcionDatosFichaSolicitud);
	connect(_respuestaFichaSolicitud, &QNetworkReply::errorOccurred, this, &toDus::eventoError);
	connect(_respuestaFichaSolicitud, &QNetworkReply::finished, this, &toDus::eventoFinalizadaFichaSolicitud);
}

void toDus::solicitarFichaAcceso() {
	QSettings configuracion;
	QString telefono = configuracion.value("todus/telefono").toString();
	QString fichaSolicitud = configuracion.value("todus/fichaSolicitud").toString();
	QString numeroVersion = configuracion.value("todus/numeroVersion", "21806").toString();
	QString ipServidorAutentificacion = configuracion.value("avanzadas/ipServidorAutentificacion", "").toString();
	QString nombreDNSServidorAutentificacion = configuracion.value("avanzadas/nombreDNSServidorAutentificacion", "auth.todus.cu").toString();
	int puertoServidorAutentificacion = configuracion.value("avanzadas/puertoServidorAutentificacion", 443).toInt();
	QUrl url = QUrl("https://" + nombreDNSServidorAutentificacion + "/v2/auth/token");
	QNetworkRequest solicitud;
	QSslConfiguration configuracionSSL = QSslConfiguration::defaultConfiguration();
	std::string datos;
	toDusPB::SolicitudFichaAcceso pbSolicitudFichaAcceso;

	telefono.remove(0, 1);
	pbSolicitudFichaAcceso.set_telefono(telefono.toStdString());
	pbSolicitudFichaAcceso.set_fichasolicitud(fichaSolicitud.toStdString());
	pbSolicitudFichaAcceso.set_numeroversion(numeroVersion.toStdString());
	pbSolicitudFichaAcceso.SerializeToString(&datos);

	configuracionSSL.setPeerVerifyMode(QSslSocket::VerifyNone);
	solicitud.setSslConfiguration(configuracionSSL);
	solicitud.setAttribute(QNetworkRequest::AutoDeleteReplyOnFinishAttribute, true);
	solicitud.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
	solicitud.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);

	if (ipServidorAutentificacion.size() > 0) {
		url.setHost(ipServidorAutentificacion);
	}
	url.setPort(puertoServidorAutentificacion);

	solicitud.setUrl(url);
	solicitud.setHeader(QNetworkRequest::UserAgentHeader, configuracion.value("avanzadas/agenteUsuario", _agenteUsuarioTodus).toString() + " Auth");
	solicitud.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-protobuf");
	solicitud.setHeader(QNetworkRequest::ContentLengthHeader, (int)datos.size());
	solicitud.setRawHeader("Host", nombreDNSServidorAutentificacion.toLocal8Bit());

	configuracion.remove("todus/fichaAcceso");
	configuracion.remove("todus/fichaAccesoTiempoExpiracion");
	configuracion.remove("todus/fichaAccesoCruda");

	_respuestaFichaAcceso = _administradorAccesoRed->post(solicitud, datos.c_str());
	connect(_respuestaFichaAcceso, &QIODevice::readyRead, this, &toDus::eventoRecepcionDatosFichaAcceso);
	connect(_respuestaFichaAcceso, &QNetworkReply::errorOccurred, this, &toDus::eventoError);
	connect(_respuestaFichaAcceso, &QNetworkReply::finished, this, &toDus::eventoFinalizadaFichaAcceso);
}

void toDus::iniciarSesionConFichaAcceso() {
	QSettings configuracion;
	QString ipServidorSesion = configuracion.value("avanzadas/ipServidorSesion", "").toString();
	QString nombreDNSServidorSesion = configuracion.value("avanzadas/nombreDNSServidorSesion", "im.todus.cu").toString();
	int puertoServidorSesion = configuracion.value("avanzadas/puertoServidorSesion", 1756).toInt();

	_dominioJID = nombreDNSServidorSesion;
	if (ipServidorSesion.size() > 0) {
		nombreDNSServidorSesion = ipServidorSesion;
	}

	_socaloSSL->connectToHostEncrypted(nombreDNSServidorSesion, puertoServidorSesion);
}

void toDus::xmppSaludar() {
	QSettings configuracion;
	QString nombreDNSServidorSesion = configuracion.value("avanzadas/nombreDNSServidorSesion", "im.todus.cu").toString();
	QByteArray buferAEnviar = "<stream:stream xmlns='jc' o='" + nombreDNSServidorSesion.toLocal8Bit() + "' xmlns:stream='x1' v='1.0'>\n";

	_socaloSSL->write(buferAEnviar);
}

void toDus::xmppIniciarSesion() {
	QSettings configuracion;
	QString nombreDNSServidorSesion = configuracion.value("avanzadas/nombreDNSServidorSesion", "im.todus.cu").toString();
	QString fichaAcceso = configuracion.value("todus/fichaAcceso").toString();
	QString b64Telefono = QByteArray::fromBase64(fichaAcceso.split(".")[1].toLocal8Bit());
	QJsonDocument json = QJsonDocument::fromJson(b64Telefono.toLocal8Bit());
	QString telefono = json.object().value("username").toString();
	QString credenciales = QChar('\0') + telefono + QChar('\0') + fichaAcceso;
	QByteArray buferAEnviar = "<ah xmlns='ah:ns' e='PLAIN'>" + credenciales.toLocal8Bit().toBase64() + "</ah>\n";

	_socaloSSL->write(buferAEnviar);
}

void toDus::xmppEstablecerSesion() {
	QByteArray buferAEnviar = "<iq i='" + _idSesion.toLocal8Bit() + "-" + QByteArray::fromStdString(std::to_string(_contadorComandos++)) + "' t='set'><b1 xmlns='x4'></b1></iq>\n";

	_socaloSSL->write(buferAEnviar);
}

void toDus::xmppMantenerSesionActiva() {
	QString buferAEnviar = "<iq from='" + _jID + "' to='" + _dominioJID + "' id='" + _idSesion + "-" + QString::fromStdString(std::to_string(_contadorComandos++)) + "' type='get'><ping xmlns='urn:xmpp:ping'/></iq>\n";

	_socaloSSL->write(buferAEnviar.toLocal8Bit());
}

void toDus::xmppSolicitarEnlaceDescarga(const QString &enlace, std::function<void (const QString &)> retroalimentador) {
	QString idSesion = _idSesion + "-" + QString::fromStdString(std::to_string(_contadorComandos++));
	QString buferAEnviar = "<iq i='" + idSesion + "' t='get' from='" + _jID + "' to='" + _dominioJID + "' ><query xmlns='todus:gurl' url='" + enlace + "'></query></iq>\n";

	_listadoRetroalimentadores[idSesion] = std::move(retroalimentador);

	_socaloSSL->write(buferAEnviar.toLocal8Bit());
}

void toDus::xmppSolicitarEnlaceSubida() {
	QString idSesion = _idSesion + "-" + QString::fromStdString(std::to_string(_contadorComandos++));
	QString buferAEnviar = "<iq i='" + idSesion + "' t='get' from='" + _jID + "' to='" + _dominioJID + "' ><query xmlns='todus:purl' persistent='false' room='' type='0' size='52428800'></query></iq>\n";

	_socaloSSL->write(buferAEnviar.toLocal8Bit());
}
