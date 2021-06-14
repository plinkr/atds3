#include "todus.hpp"
#include "todus.pb.h"
#include "http1.hpp"
#include "main.hpp"
#include <ctime>
#include <QByteArray>
#include <QSettings>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QRandomGenerator>
#include <QSslConfiguration>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QInputDialog>
#include <QMutexLocker>
#include <QFile>
//#include <iostream>


toDus::toDus(QObject *padre)
	: QObject(padre), _progresoInicioSesion(ProgresoInicioSesion::Ninguno), _contadorComandos(0), _desconexionSolicitada(false), _fichaAccesoRenovada(false), _contadorPING(1) {
}
toDus::~toDus() {
	google::protobuf::ShutdownProtobufLibrary();
}

void toDus::iniciar() {
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	_idSesion = generarIDSesion(5);
	_socaloSSL = new QSslSocket(this);
	_socaloSSL->setPeerVerifyMode(QSslSocket::VerifyNone);
	_temporizadorMantenerSesionActiva.setInterval(5000);

	connect(_socaloSSL, &QSslSocket::stateChanged, this, &toDus::eventoCambiarEstado);
	connect(_socaloSSL, &QSslSocket::encrypted, this, &toDus::eventoConexionLista);
	connect(_socaloSSL, &QSslSocket::readyRead, this, &toDus::eventoDatosRecibidos);
	connect(&_temporizadorMantenerSesionActiva, &QTimer::timeout, this, &toDus::xmppMantenerSesionActiva);
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
	QString telefono = _configuracion.value("todus/telefono").toString();
	QString fichaSolicitud = _configuracion.value("todus/fichaSolicitud").toString();
	QString fichaAcceso = _configuracion.value("todus/fichaAcceso").toString();
	std::time_t tiempoActual = std::time(nullptr);
	qint32 fichaAccesoTiempoExpiracion = _configuracion.value("todus/fichaAccesoTiempoExpiracion", (long long)tiempoActual).toInt();

	_dominioJID = "im.todus.cu";

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
	} else if (fichaSolicitud.size() > 0) {
		solicitarFichaAcceso();
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

void toDus::solicitarEnlaceFirmadoSubida(const QString &tamano) {
	if (_progresoInicioSesion == ProgresoInicioSesion::SesionIniciada) {
		xmppSolicitarEnlaceSubida(tamano);
	}
}

void toDus::eventoFinalizadaCodigoSMS() {
	if (_httpCodigoSMS->codigoHTTP() == 200 || _httpCodigoSMS->codigoHTTP() == 204) {
		bool ok = false;
		QString codigo = QInputDialog::getText(nullptr, "Código de verificación", "Código de verificación:", QLineEdit::Normal, "", &ok);

		if (ok == true) {
			solicitarFichaSolicitud(codigo);
		}
	}
}

void toDus::eventoFinalizadaFichaSolicitud() {
	if (_httpFichaSolicitud->codigoHTTP() == 200) {
		toDusPB::RecepcionFichaSolicitud pbRecepcionFichaSolicitud;

		if (pbRecepcionFichaSolicitud.ParseFromArray(_buferDescargaFichaSolicitud.constData(), _buferDescargaFichaSolicitud.size()) == true) {
			_configuracion.setValue("todus/fichaSolicitud", pbRecepcionFichaSolicitud.fichasolicitud().c_str());
			if (pbRecepcionFichaSolicitud.usuario().has_nombre() == true) {
				_configuracion.setValue("todus/nombre", pbRecepcionFichaSolicitud.usuario().nombre().c_str());
			}
			if (pbRecepcionFichaSolicitud.usuario().has_enlacefoto() == true) {
				_configuracion.setValue("todus/enlaceAvatar", pbRecepcionFichaSolicitud.usuario().enlacefoto().c_str());
			}
			if (pbRecepcionFichaSolicitud.usuario().has_biografia() == true) {
				_configuracion.setValue("todus/biografia", pbRecepcionFichaSolicitud.usuario().biografia().c_str());
			}
/*
		} else {
			int posicion = 0;
			std::uint16_t longitud = 0;

			if ((int)_buferDescargaFichaSolicitud[posicion] == 0x0a) {
				longitud = (std::uint16_t)_buferDescargaFichaSolicitud[++posicion];
				_configuracion.setValue("todus/fichaSolicitud", _buferDescargaFichaSolicitud.mid(++posicion, longitud).constData());
				posicion += longitud;
				if ((int)_buferDescargaFichaSolicitud[posicion] == 0x12) {
					posicion = _buferDescargaFichaSolicitud.indexOf((char)0x0a, posicion);
					if (posicion != -1) {
						longitud = (std::uint16_t)_buferDescargaFichaSolicitud[++posicion];
						posicion += longitud + 1;
						if ((int)_buferDescargaFichaSolicitud[posicion] == 0x12) {
							longitud = (std::uint16_t)_buferDescargaFichaSolicitud[++posicion];
							_configuracion.setValue("todus/nombre", _buferDescargaFichaSolicitud.mid(++posicion, longitud).constData());
							posicion += longitud;
							if ((int)_buferDescargaFichaSolicitud[posicion] == 0x1a) {
								longitud = (std::uint16_t)_buferDescargaFichaSolicitud[++posicion];
								_configuracion.setValue("todus/enlaceAvatar", _buferDescargaFichaSolicitud.mid(++posicion, longitud).constData());
							}
						}
					}
				}
			}
*/
			solicitarFichaAcceso();
		}
	}
}

void toDus::eventoFinalizadaFichaAcceso() {
	if (_httpFichaAcceso->codigoHTTP() == 200) {
		toDusPB::RecepcionFichaAcceso pbRecepcionFichaAcceso;

		if (pbRecepcionFichaAcceso.ParseFromArray(_buferDescargaFichaAcceso.constData(), _buferDescargaFichaAcceso.size()) == true) {
			_configuracion.setValue("todus/fichaAcceso", pbRecepcionFichaAcceso.fichaacceso().c_str());
			_configuracion.setValue("todus/fichaAccesoTiempoExpiracion", pbRecepcionFichaAcceso.tiempoexpiracion());

			iniciarSesionConFichaAcceso();
		}
	} else if (_httpFichaAcceso->codigoHTTP() >= 400 && _httpFichaAcceso->codigoHTTP() <= 499) {
		_configuracion.remove("todus/codigoVerificacion");
		_configuracion.remove("todus/fichaSolicitud");

		desconectar();

		_estado = Estado::Desconectado;

		emit estadoCambiado(_estado);
	}
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
	QString bufer;
	QRegularExpression re;
	QRegularExpressionMatch rem;

	while (_socaloSSL->bytesAvailable() > 0) {
		bufer = _socaloSSL->readAll();
//		std::cout << bufer.toStdString() << std::endl;

		switch (_progresoInicioSesion) {
			case ProgresoInicioSesion::Ninguno:
				break;
			case ProgresoInicioSesion::Saludo: // Respuesta al saludo
				re = QRegularExpression("<stream:features>.+<e>PLAIN</e>.+</stream:features>", QRegularExpression::CaseInsensitiveOption);
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
					//_socaloSSL->write("<stream:stream xmlns='jc' o='im.todus.cu' xmlns:stream='x1' v='1.0'>", QRegularExpression::CaseInsensitiveOption);
				} else {
					_progresoInicioSesion = ProgresoInicioSesion::Ninguno;
					desconectar();

					solicitarFichaAcceso();
				}
				break;
			case ProgresoInicioSesion::Autenficicacion: // Respuesta satisfactoria al intento de establecer sesión
				re = QRegularExpression(".*<stream:stream i='.+' v='1.0' xml:lang='en' xmlns:stream='x1' f='im.todus.cu' xmlns='jc'>.*", QRegularExpression::CaseInsensitiveOption);
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
					_progresoInicioSesion = ProgresoInicioSesion::SesionIniciada;
					_temporizadorMantenerSesionActiva.start();
					xmppEstablecerSesion();
					_desconexionSolicitada = false;
					_estado = Estado::Listo;
					emit estadoCambiado(_estado);
				}
				break;
			case ProgresoInicioSesion::SesionIniciada: // Respuesta a algún comando
				// Sesion establecida
				re = QRegularExpression("<iq t='result' i='.+'><b1 xmlns='x4'><jid>(.+)</jid><sid>.+</sid></b1></iq>", QRegularExpression::CaseInsensitiveOption);
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
					_jID = rem.captured(1);
					_socaloSSL->write("<en xmlns='x7' u='true' max='300'/>");
				}
				// Corrección del contador del PONG
				re = QRegularExpression("<a h='(.+)' xmlns='x7'/>", QRegularExpression::CaseInsensitiveOption);
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
					_contadorPING = rem.captured(1).toUInt() + 1;
				}
				// PING
				re = QRegularExpression("<r xmlns='x7'/>", QRegularExpression::CaseInsensitiveOption);
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
					// PONG
					xmppPONG();
				}
				// Comando: GURL
				re = QRegularExpression("<iq.+i='(.+)'.*><query xmlns='todus:gurl' du='(.+)' status='200'/></iq>", QRegularExpression::CaseInsensitiveOption);
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
					QString idSesion = rem.captured(1);
					if (_listadoRetroalimentadores.find(idSesion) != _listadoRetroalimentadores.end()) {
						_listadoRetroalimentadores[idSesion](rem.captured(2));
						_listadoRetroalimentadores.remove(idSesion);
					}
				}
				// Comando: PURL
				re = QRegularExpression("<iq.+i='(.+)'.*><query xmlns='todus:purl' put='(.+)' get='(.+)' status='200'/></iq>", QRegularExpression::CaseInsensitiveOption);
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
					QString idSesion = rem.captured(1);
					QString enlaceFirmadoSubida = rem.captured(2);
					QString enlaceNoFirmadoDescarga = rem.captured(3);
					//_listadoEnlacesFirmados.remove(idSesion);
					//emit enlaceFirmadoObtenido(enlaceNoFirmado, re.cap(2));
					std::cout << "curl --progress-bar --header 'Authorization: Bearer " << _configuracion.value("todus/fichaAcceso").toString().toStdString() << "' --request PUT --tcp-nodelay --user-agent 'ToDus 0.38.35 HTTP-Upload' --upload-file \"${archivo}\" '" << enlaceFirmadoSubida.replace("&amp;", "&").toStdString() << "'" << std::endl;
					std::cout << enlaceNoFirmadoDescarga.toStdString() << std::endl;
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

unsigned int toDus::obtenerProximoIDComando() {
	QMutexLocker b(&_mutexContadorComandos);

	_contadorComandos++;

	return _contadorComandos;
}

void toDus::solicitarCodigoSMS(const QString &telefono) {
	QString ipServidorAutentificacion = _configuracion.value("avanzadas/ipServidorAutentificacion", "").toString();
	QString nombreDNSServidorAutentificacion = _configuracion.value("avanzadas/nombreDNSServidorAutentificacion", "auth.todus.cu").toString();
	std::uint16_t puertoServidorAutentificacion = _configuracion.value("avanzadas/puertoServidorAutentificacion", 443).toUInt();
	std::string datos;
	toDusPB::SolicitudSMS pbSolicitudSMS;

	_httpCodigoSMS = new HTTP1(this);

	_httpCodigoSMS->establecerMetodo(HTTP1::Metodo::PUBLICAR);
	if (ipServidorAutentificacion.size() > 0) {
		_httpCodigoSMS->establecerAnfitrion(ipServidorAutentificacion);
	} else {
		_httpCodigoSMS->establecerAnfitrion(nombreDNSServidorAutentificacion);
	}
	_httpCodigoSMS->establecerPuerto(puertoServidorAutentificacion);
	_httpCodigoSMS->establecerRuta("/v2/auth/users.reserve");

	pbSolicitudSMS.set_telefono(telefono.toStdString());
	pbSolicitudSMS.set_id(_idInicioSesion.toStdString());
	pbSolicitudSMS.SerializeToString(&datos);

	_buferCargaCodigoSMS = QByteArray::fromStdString(datos);
	_httpCodigoSMS->establecerOrigenCarga(&_buferCargaCodigoSMS);
	_buferDescargaCodigoSMS.clear();
	_httpCodigoSMS->establecerDestinoDescarga(&_buferDescargaCodigoSMS);

	_httpCodigoSMS->agregarCabeceraSolicitud("Content-Length", QString::number(datos.size()));
	_httpCodigoSMS->agregarCabeceraSolicitud("Content-Type", "application/x-protobuf");
	_httpCodigoSMS->agregarCabeceraSolicitud("Host", nombreDNSServidorAutentificacion);
	_httpCodigoSMS->agregarCabeceraSolicitud("User-Agent", _configuracion.value("avanzadas/agenteUsuario", _agenteUsuarioTodus).toString() + " Auth");

	connect(_httpCodigoSMS, &HTTP1::finalizado, this, &toDus::eventoFinalizadaCodigoSMS);

	_httpCodigoSMS->ejecutar();
}

void toDus::solicitarFichaSolicitud(const QString &codigo) {
	QString telefono = _configuracion.value("todus/telefono").toString();
	QString ipServidorAutentificacion = _configuracion.value("avanzadas/ipServidorAutentificacion", "").toString();
	QString nombreDNSServidorAutentificacion = _configuracion.value("avanzadas/nombreDNSServidorAutentificacion", "auth.todus.cu").toString();
	std::int16_t puertoServidorAutentificacion = _configuracion.value("avanzadas/puertoServidorAutentificacion", 443).toInt();
	std::string datos;
	toDusPB::SolicitudFichaSolicitud pbSolicitudFichaSolicitud;

	_httpFichaSolicitud = new HTTP1(this);

	_httpFichaSolicitud->establecerMetodo(HTTP1::Metodo::PUBLICAR);
	if (ipServidorAutentificacion.size() > 0) {
		_httpFichaSolicitud->establecerAnfitrion(ipServidorAutentificacion);
	} else {
		_httpFichaSolicitud->establecerAnfitrion(nombreDNSServidorAutentificacion);
	}
	_httpFichaSolicitud->establecerPuerto(puertoServidorAutentificacion);
	_httpFichaSolicitud->establecerRuta("/v2/auth/users.register");

	_configuracion.remove("todus/fichaSolicitud");
	_configuracion.remove("todus/nombre");
	_configuracion.remove("todus/enlaceAvatar");

	telefono.remove(0, 1);
	pbSolicitudFichaSolicitud.set_telefono(telefono.toStdString());
	pbSolicitudFichaSolicitud.set_id(_idInicioSesion.toStdString());
	pbSolicitudFichaSolicitud.set_codigo(codigo.toStdString());
	pbSolicitudFichaSolicitud.SerializeToString(&datos);

	_buferCargaFichaSolicitud = QByteArray::fromStdString(datos);
	_httpFichaSolicitud->establecerOrigenCarga(&_buferCargaFichaSolicitud);
	_buferDescargaFichaSolicitud.clear();
	_httpFichaSolicitud->establecerDestinoDescarga(&_buferDescargaFichaSolicitud);

	_httpFichaSolicitud->agregarCabeceraSolicitud("Content-Length", QString::number(datos.size()));
	_httpFichaSolicitud->agregarCabeceraSolicitud("Content-Type", "application/x-protobuf");
	_httpFichaSolicitud->agregarCabeceraSolicitud("Host", nombreDNSServidorAutentificacion);
	_httpFichaSolicitud->agregarCabeceraSolicitud("User-Agent", _configuracion.value("avanzadas/agenteUsuario", _agenteUsuarioTodus).toString() + " Auth");

	connect(_httpFichaSolicitud, &HTTP1::finalizado, this, &toDus::eventoFinalizadaFichaSolicitud);

	_httpFichaSolicitud->ejecutar();
}

void toDus::solicitarFichaAcceso() {
	QString telefono = _configuracion.value("todus/telefono").toString();
	QString fichaSolicitud = _configuracion.value("todus/fichaSolicitud").toString();
	QString numeroVersion = _configuracion.value("todus/numeroVersion", _numeroVersionTodus).toString();
	QString ipServidorAutentificacion = _configuracion.value("avanzadas/ipServidorAutentificacion", "").toString();
	QString nombreDNSServidorAutentificacion = _configuracion.value("avanzadas/nombreDNSServidorAutentificacion", "auth.todus.cu").toString();
	std::uint16_t puertoServidorAutentificacion = _configuracion.value("avanzadas/puertoServidorAutentificacion", 443).toInt();
	std::string datos;
	toDusPB::SolicitudFichaAcceso pbSolicitudFichaAcceso;

	_httpFichaAcceso = new HTTP1(this);

	_httpFichaAcceso->establecerMetodo(HTTP1::Metodo::PUBLICAR);
	if (ipServidorAutentificacion.size() > 0) {
		_httpFichaAcceso->establecerAnfitrion(ipServidorAutentificacion);
	} else {
		_httpFichaAcceso->establecerAnfitrion(nombreDNSServidorAutentificacion);
	}
	_httpFichaAcceso->establecerPuerto(puertoServidorAutentificacion);
	_httpFichaAcceso->establecerRuta("/v2/auth/token");

	_configuracion.remove("todus/fichaAcceso");
	_configuracion.remove("todus/fichaAccesoTiempoExpiracion");

	telefono.remove(0, 1);
	pbSolicitudFichaAcceso.set_telefono(telefono.toStdString());
	pbSolicitudFichaAcceso.set_fichasolicitud(fichaSolicitud.toStdString());
	pbSolicitudFichaAcceso.set_numeroversion(numeroVersion.toStdString());
	pbSolicitudFichaAcceso.SerializeToString(&datos);

	_buferCargaFichaAcceso = QByteArray::fromStdString(datos);
	_httpFichaAcceso->establecerOrigenCarga(&_buferCargaFichaAcceso);
	_buferDescargaFichaAcceso.clear();
	_httpFichaAcceso->establecerDestinoDescarga(&_buferDescargaFichaAcceso);

	_httpFichaAcceso->agregarCabeceraSolicitud("Content-Length", QString::number(datos.size()));
	_httpFichaAcceso->agregarCabeceraSolicitud("Content-Type", "application/x-protobuf");
	_httpFichaAcceso->agregarCabeceraSolicitud("Host", nombreDNSServidorAutentificacion);
	_httpFichaAcceso->agregarCabeceraSolicitud("User-Agent", _configuracion.value("avanzadas/agenteUsuario", _agenteUsuarioTodus).toString() + " Auth");

	connect(_httpFichaAcceso, &HTTP1::finalizado, this, &toDus::eventoFinalizadaFichaAcceso);

	_httpFichaAcceso->ejecutar();
}

void toDus::iniciarSesionConFichaAcceso() {
	QString ipServidorSesion = _configuracion.value("avanzadas/ipServidorSesion", "").toString();
	QString nombreDNSServidorSesion = _configuracion.value("avanzadas/nombreDNSServidorSesion", "im.todus.cu").toString();
	int puertoServidorSesion = _configuracion.value("avanzadas/puertoServidorSesion", 1756).toInt();

	_dominioJID = nombreDNSServidorSesion;
	if (ipServidorSesion.size() > 0) {
		nombreDNSServidorSesion = ipServidorSesion;
	}

	_socaloSSL->connectToHostEncrypted(nombreDNSServidorSesion, puertoServidorSesion);
}

void toDus::xmppSaludar() {
	QString nombreDNSServidorSesion = _configuracion.value("avanzadas/nombreDNSServidorSesion", "im.todus.cu").toString();
	QByteArray buferAEnviar = "<stream:stream xmlns='jc' o='" + nombreDNSServidorSesion.toLocal8Bit() + "' xmlns:stream='x1' v='1.0'>\n";

	_socaloSSL->write(buferAEnviar);
}

void toDus::xmppIniciarSesion() {
	QString fichaAcceso = _configuracion.value("todus/fichaAcceso").toString();
	QString b64Telefono = QByteArray::fromBase64(fichaAcceso.split(".")[1].toLocal8Bit());
	QJsonDocument json = QJsonDocument::fromJson(b64Telefono.toLocal8Bit());
	QString telefono = json.object().value("username").toString();
	QString credenciales = QChar('\0') + telefono + QChar('\0') + fichaAcceso;
	QByteArray buferAEnviar = "<ah xmlns='ah:ns' e='PLAIN'>" + credenciales.toLocal8Bit().toBase64() + "</ah>\n";

	_socaloSSL->write(buferAEnviar);
}

void toDus::xmppEstablecerSesion() {
	QByteArray buferAEnviar = "<iq i='" + _idSesion.toLocal8Bit() + "-" + QByteArray::number(obtenerProximoIDComando()) + "' t='set'><b1 xmlns='x4'></b1></iq>\n";

	_socaloSSL->write(buferAEnviar);
}

void toDus::xmppMantenerSesionActiva() {
//	QByteArray buferAEnviar = "<iq i='" + _idSesion.toLocal8Bit() + "-" + QByteArray::number(obtenerProximoIDComando()) + "' t='get'><query xmlns='todus:roster:hash' hash='CB9C29EED34978062620B4734C36CAA3AC4EE2A36A72C0C7A6D899AE47828490'></query></iq>\n";
//	QByteArray buferAEnviar = "<iq i='" + _idSesion.toLocal8Bit() + "-" + QByteArray::number(obtenerProximoIDComando()) + "' t='get'><query xmlns='todus:block:get'></query></iq>\n";
	QByteArray buferAEnviar = "<p i='" + _idSesion.toLocal8Bit() + "-" + QByteArray::number(obtenerProximoIDComando()) + "'></p>\n";

	_socaloSSL->write(buferAEnviar);
}

void toDus::xmppPONG() {
//	QString buferAEnviar = "<iq from='" + _jID + "' to='" + _dominioJID + "' id='" + _idSesion + "-" + QString::number(obtenerProximoIDComando()) + "' type='get'><ping xmlns='urn:xmpp:ping'/></iq>\n";
	QString buferAEnviar = "<a xmlns='x7' h='" + QString::number(_contadorPING++) + "'/>";

	_socaloSSL->write(buferAEnviar.toLocal8Bit());
}

void toDus::xmppSolicitarEnlaceDescarga(const QString &enlace, std::function<void (const QString &)> retroalimentador) {
	QMutexLocker b(&_mutexSolicitarEnlace);
	QString idSesion = _idSesion + "-" + QString::number(obtenerProximoIDComando());
	QString buferAEnviar = "<iq i='" + idSesion + "' t='get' from='" + _jID + "' to='" + _dominioJID + "' ><query xmlns='todus:gurl' url='" + enlace + "'></query></iq>\n";

	_listadoRetroalimentadores[idSesion] = std::move(retroalimentador);

	_socaloSSL->write(buferAEnviar.toLocal8Bit());
}

void toDus::xmppSolicitarEnlaceSubida(const QString &tamano) {
	QMutexLocker b(&_mutexSolicitarEnlace);
	QString idSesion = _idSesion + "-" + QString::number(obtenerProximoIDComando());
	QString buferAEnviar = "<iq i='" + idSesion + "' t='get' from='" + _jID + "' to='" + _dominioJID + "' ><query xmlns='todus:purl' persistent='false' room='' type='0' size='" + tamano + "'></query></iq>\n";

	_socaloSSL->write(buferAEnviar.toLocal8Bit());
}
