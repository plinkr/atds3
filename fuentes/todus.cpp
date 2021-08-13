#include "todus.hpp"
#include "todus.pb.h"
#include "http.hpp"
#include "modelopaquetes.hpp"
#include "modelotareas.hpp"
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
#include <QFile>
#include <QCryptographicHash>


toDus::toDus(QObject *padre)
	: QObject(padre), _solicitudesFichaAcceso(0), _estado(Estados::Desconectado), _progresoInicioSesion(ProgresoInicioSesion::Ninguno), _contadorComandos(0), _desconexionSolicitada(false), _fichaAccesoRenovada(false), _contadorPING(1), _estadoSocaloWeb(Estados::Desconectado), _socaloWebIndiceFichaLocal(0) {
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	_solicitudesEnlacesFirmadosAnterior = 0;

	_socaloSSL = new QSslSocket(this);
	_socaloSSL->setSocketOption(QAbstractSocket::LowDelayOption, 1);
	_socaloSSL->setSocketOption(QAbstractSocket::TypeOfServiceOption, 64);
	_socaloSSL->ignoreSslErrors();
	connect(_socaloSSL, &QSslSocket::stateChanged, this, &toDus::eventoCambiarEstado);
	connect(_socaloSSL, &QSslSocket::encrypted, this, &toDus::eventoConexionLista);
	connect(_socaloSSL, &QSslSocket::readyRead, this, &toDus::eventoDatosRecibidos);

	_temporizadorListadoSolicitudesEnlacesFirmados.setInterval(2000);
	connect(&_temporizadorListadoSolicitudesEnlacesFirmados, &QTimer::timeout, this, &toDus::procesarColaSolicitudesEnlacesFirmados);

	_temporizadorSocaloWebPING.setInterval(15000);
	connect(&_temporizadorSocaloWebPING, &QTimer::timeout, this, &toDus::eventoPPFSocaloWebPING);

	_temporizadorSocaloWebVerificarConexion.setInterval(20000);
	connect(&_temporizadorSocaloWebVerificarConexion, &QTimer::timeout, this, &toDus::eventoPPFSocaloWebVerificarConexion);

	_socaloWeb.ignoreSslErrors();
	connect(&_socaloWeb, &QWebSocket::connected, this, &toDus::eventoPPFConectado);
	connect(&_socaloWeb, &QWebSocket::pong, this, &toDus::eventoPPFPong);
	connect(&_socaloWeb, &QWebSocket::textMessageReceived, this, &toDus::eventoPPFMensajeRecibido);
	connect(&_socaloWeb, &QWebSocket::disconnected, this, &toDus::eventoPPFDesconectado);

	_temporizadorListadoSolicitudesEnlacesFirmados.start();
}
toDus::~toDus() {
	_temporizadorSocaloWebVerificarConexion.stop();
	_temporizadorSocaloWebPING.stop();
	_temporizadorListadoSolicitudesEnlacesFirmados.stop();
	google::protobuf::ShutdownProtobufLibrary();
}

void toDus::procesarColaSolicitudesEnlacesFirmados() {
	if (_listadoSolicitudesEnlacesFirmados.size() != _solicitudesEnlacesFirmadosAnterior) {
		emitirRegistro(TiposRegistro::Informacion, "toDus") << "Solicitudes de enlaces firmados por procesar: " << _listadoSolicitudesEnlacesFirmados.size() << std::endl;

		_solicitudesEnlacesFirmadosAnterior = _listadoSolicitudesEnlacesFirmados.size();

		if (_solicitudesEnlacesFirmadosAnterior > 0) {
			if (_estado == Estados::Desconectado) {
				iniciarSesion();
			}
		} else {
			desconectar();
			_estado = Estados::Desconectado;
			return;
		}
	}

	if (_listadoSolicitudesEnlacesFirmados.size() > 0) {
		if (_estado == Estados::Listo) {
			for (qint64 id : _listadoSolicitudesEnlacesFirmados.keys()) {
				if (_listadoSolicitudesEnlacesFirmados[id].reintentos > 1) {
					if (_configuraciones.valor("todus/programaPiscinaFichas", false).toBool() == true) {
						socaloWebSolicitarFicha();
					}
				}

				switch (_listadoSolicitudesEnlacesFirmados[id].tipo) {
					case ModeloPaquetes::Tipos::Publicacion:
						_listadoSolicitudesEnlacesFirmados[id].reintentos++;
						xmppSolicitarEnlaceSubida(id, _listadoSolicitudesEnlacesFirmados[id].clasificacion);
						break;
					case ModeloPaquetes::Tipos::Descarga:
						_listadoSolicitudesEnlacesFirmados[id].reintentos++;
						xmppSolicitarEnlaceDescarga(id);
						break;
				}
			}
		}

		if (_estado == Estados::Desconectado) {
			iniciarSesion();
		}
	}
}
/*
void toDus::iniciar() {
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	_idSesion = generarIDSesion(5);
	_socaloSSL = new QSslSocket();
	_socaloSSL->setPeerVerifyMode(QSslSocket::VerifyNone);
//	_temporizadorMantenerSesionActiva.setInterval(5000);

	connect(_socaloSSL, &QSslSocket::stateChanged, this, &toDus::eventoCambiarEstado);
	connect(_socaloSSL, &QSslSocket::encrypted, this, &toDus::eventoConexionLista);
	connect(_socaloSSL, &QSslSocket::readyRead, this, &toDus::eventoDatosRecibidos);
//	connect(&_temporizadorMantenerSesionActiva, &QTimer::timeout, this, &toDus::xmppMantenerSesionActiva);
}
*/
/**
 * @brief Obtiene el estado de la sesión toDus
 * @return Devuelve el estado de la sesión toDus
 */
toDus::Estados toDus::obtenerEstado() {
	return _estado;
}

/**
 * @brief Desconecta de toDus
 */
void toDus::desconectar() {
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
	QString telefono = _configuraciones.valor("todus/telefono").toString();
	QByteArray fichaAcceso = _configuraciones.valor("todus/fichaAcceso", "").toByteArray();
//	std::time_t tiempoActual = std::time(nullptr);
//	qint32 fichaAccesoTiempoExpiracion = _configuraciones.valor("todus/fichaAccesoTiempoExpiracion", (long long)tiempoActual).toInt();

	_idSesion = generarIDSesion(5);
	_dominioJID = _configuraciones.valor("avanzadas/servidorMensajeria", "im.todus.cu").toString();
	_idInicioSesion = generarIDSesion(150);
	_socaloWebFichaAportada = false;
	_fichaAccesoRenovada = false;
	_desconexionSolicitada = false;

	if (_fichaAccesoActual.isEmpty() == true && fichaAcceso.isEmpty() == false) {
		_fichaAccesoActual = fichaAcceso;
	}

	if (_fichaAccesoActual.isEmpty() == false && fichaAcceso.isEmpty() == false) {
		emitirRegistro(TiposRegistro::Informacion, "toDus") << "Iniciando sesion" << std::endl;

		iniciarSesionConFichaAcceso();
	} else if (telefono.isEmpty() == false) {
		emitirRegistro(TiposRegistro::Informacion, "toDus") << "Iniciando sesion" << std::endl;

		solicitarCodigoSMS(telefono);
	}
}

void toDus::obtenerEnlaceFirmado(int tipo, int clasificacion, qint64 paquete, qint64 id, const QString &enlace, qint64 tamano) {
	_listadoSolicitudesEnlacesFirmados[id] = { tipo, clasificacion, paquete, enlace, tamano, "", 0 };
}

void toDus::eliminarSolicitudEnlaceFirmado(qint64 id) {
	if (_listadoSolicitudesEnlacesFirmados.find(id) != _listadoSolicitudesEnlacesFirmados.end()) {
		_listadoSolicitudesEnlacesFirmados.remove(id);
	}
}

void toDus::eliminarTodasSolicitudesEnlaceFirmado(qint64 paquete) {
	for (qint64 id : _listadoSolicitudesEnlacesFirmados.keys()) {
		if (_listadoSolicitudesEnlacesFirmados[id].paquete == paquete) {
			_listadoSolicitudesEnlacesFirmados.remove(id);
		}
	}
}

void toDus::eventoFinalizadaCodigoSMS() {
	if (_httpCodigoSMS->codigoHTTP() == 200 || _httpCodigoSMS->codigoHTTP() == 204) {
		emitirRegistro(TiposRegistro::Informacion, "toDus") << "Codigo de verificacion por SMS solicitado." << std::endl;

		QMetaObject::invokeMethod(_qmlRaiz, "mostrarPantallaCodigoVerificacion", Qt::QueuedConnection);
	} else {
		emitirRegistro(TiposRegistro::Informacion, "toDus") << "Ocurrio un error al solicitar el codigo de verificacion por SMS" << std::endl;

		QMetaObject::invokeMethod(_qmlRaiz, "mostrarPantallaFalloInicioSesion", Qt::QueuedConnection, Q_ARG(QVariant, _httpCodigoSMS->codigoHTTP()));
	}
}

void toDus::eventoFinalizadaFichaSolicitud() {
	if (_httpFichaSolicitud->codigoHTTP() == 200) {
		toDusPB::RecepcionFichaSolicitud pbRecepcionFichaSolicitud;

		if (pbRecepcionFichaSolicitud.ParseFromArray(_buferDescargaFichaSolicitud.constData(), _buferDescargaFichaSolicitud.size()) == true) {
			emitirRegistro(TiposRegistro::Informacion, "toDus") << "Codigo verificado correctamente. Obteniendo ficha de solicitud" << std::endl;

			_configuraciones.establecerValor("todus/fichaSolicitud", QByteArray(pbRecepcionFichaSolicitud.fichasolicitud().c_str()));
/*
			if (pbRecepcionFichaSolicitud.usuario().has_nombre() == true) {
				_configuraciones.establecerValor("todus/nombre", pbRecepcionFichaSolicitud.usuario().nombre().c_str());
			}
			if (pbRecepcionFichaSolicitud.usuario().has_enlacefoto() == true) {
				_configuraciones.establecerValor("todus/enlaceAvatar", pbRecepcionFichaSolicitud.usuario().enlacefoto().c_str());
			}
			if (pbRecepcionFichaSolicitud.usuario().has_biografia() == true) {
				_configuraciones.establecerValor("todus/biografia", pbRecepcionFichaSolicitud.usuario().biografia().c_str());
			}
*/
			solicitarFichaAcceso();
		}
	} else {
		emitirRegistro(TiposRegistro::Informacion, "toDus") << "Ocurrio un error al verificar el codigo" << std::endl;

		desconectar();

		_estado = Estados::Desconectado;
		emit estadoCambiado(_estado);
	}
}

void toDus::eventoFinalizadaFichaAcceso() {
	desconectar();
	_estado = Estados::Desconectado;

	if (_httpFichaAcceso->codigoHTTP() == 200) {
		toDusPB::RecepcionFichaAcceso pbRecepcionFichaAcceso;

		if (pbRecepcionFichaAcceso.ParseFromArray(_buferDescargaFichaAcceso.constData(), _buferDescargaFichaAcceso.size()) == true) {
			emitirRegistro(TiposRegistro::Informacion, "toDus") << "Ficha de acceso recibida" << std::endl;

			_fichaAccesoActual = QByteArray::fromStdString(pbRecepcionFichaAcceso.fichaacceso());
			_configuraciones.establecerValor("todus/fichaAcceso", _fichaAccesoActual);
			_configuraciones.establecerValor("todus/fichaAccesoTiempoExpiracion", pbRecepcionFichaAcceso.tiempoexpiracion());

			_solicitudesFichaAcceso = 0;

			iniciarSesionConFichaAcceso();
		}
	} else {
		emitirRegistro(TiposRegistro::Informacion, "toDus") << "Ocurrio un error al recibir la ficha de acceso" << std::endl;

		_configuraciones.eliminar("todus/fichaSolicitud");
	}
}

void toDus::eventoCambiarEstado(QAbstractSocket::SocketState estado) {
	switch (estado) {
		case QAbstractSocket::UnconnectedState:
			if (_estado != Estados::SolicitandoFichaAcceso) {
				_estado = Estados::Desconectado;
				emitirRegistro(TiposRegistro::Informacion, "toDus") << "Desconectado" << std::endl;
			}
			break;
		case QAbstractSocket::HostLookupState:
			_estado = Estados::ResolviendoNombreDNS;
			break;
		case QAbstractSocket::ConnectingState:
		{
			QString ipServidorSesion = _configuraciones.valor("avanzadas/servidorMensajeria", "").toString();
			QString nombreDNSServidorSesion = "im.todus.cu";
			int puertoServidorSesion = _configuraciones.valor("avanzadas/servidorMensajeriaPuerto", 1756).toInt();

			if (ipServidorSesion.size() > 0) {
				nombreDNSServidorSesion = ipServidorSesion;
			}

			emitirRegistro(TiposRegistro::Informacion, "toDus") << "Conectando a " << nombreDNSServidorSesion.toStdString() << ":" << puertoServidorSesion << std::endl;

			if (_configuraciones.valor("todus/programaPiscinaFichas", false).toBool() == true) {
				if (_estadoSocaloWeb == Estados::Desconectado) {
					iniciarPPF();
				}
			}

			_estado = Estados::Conectando;
			break;
		}
		case QAbstractSocket::ConnectedState:
			emitirRegistro(TiposRegistro::Informacion, "toDus") << "Conectado" << std::endl;

			_estado = Estados::Conectado;
			break;
		case QAbstractSocket::ClosingState:
//			_temporizadorMantenerSesionActiva.stop();
			_estado = Estados::Desconectando;
			break;
		default:
			break;
	}

	emit estadoCambiado(_estado);
}

void toDus::eventoConexionLista() {
	_estado = Estados::IniciandoSesion;
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

//		emitirRegistro(TiposRegistro::Depuracion, "toDus") << ">> " << bufer.toStdString() << std::endl;

		switch (_progresoInicioSesion) {
			case ProgresoInicioSesion::Ninguno:
				break;
			case ProgresoInicioSesion::Saludo: // Respuesta al saludo
				emitirRegistro(TiposRegistro::Informacion, "toDus") << "Autentificando" << std::endl;

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
					emitirRegistro(TiposRegistro::Informacion, "toDus") << "Sesion iniciada satisfactoriamente" << std::endl;

					_progresoInicioSesion = ProgresoInicioSesion::Autenficicacion;
					xmppSaludar();
				} else {
					if (_solicitudesFichaAcceso > 1) {
						emitirRegistro(TiposRegistro::Informacion, "toDus") << "Fallo el inicio de sesion. Reintentando" << std::endl;

						_configuraciones.eliminar("todus/fichaSolicitud");
						_configuraciones.eliminar("todus/fichaAccesoTiempoExpiracion");
						_configuraciones.eliminar("todus/fichaAcceso");
						_fichaAccesoActual.clear();
						_solicitudesFichaAcceso = 0;
						_progresoInicioSesion = ProgresoInicioSesion::Ninguno;
						desconectar();
					} else {
						if (_fichaAccesoActual == _configuraciones.valor("todus/fichaAcceso", "").toByteArray()) {
							emitirRegistro(TiposRegistro::Informacion, "toDus") << "Fallo el inicio de sesion. Intentando solicitar una nueva ficha de acceso a toDus" << std::endl;

							solicitarFichaAcceso();
						} else {
							emitirRegistro(TiposRegistro::Informacion, "PPF") << "La ficha de acceso obtenida no es valida." << std::endl;

							_estado = Estados::SolicitandoFichaAcceso;
							desconectar();
							socaloWebSolicitarFicha();
						}
					}
				}
				break;
			case ProgresoInicioSesion::Autenficicacion: // Respuesta satisfactoria al intento de establecer sesión
///				re = QRegularExpression(".*<stream:stream i='.+' v='1.0' xml:lang='en' xmlns:stream='x1' f='im.todus.cu' xmlns='jc'>.*", QRegularExpression::CaseInsensitiveOption);
				re = QRegularExpression(".*<stream:features><b1 xmlns='x4'/>.+</stream:features>", QRegularExpression::CaseInsensitiveOption);
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
					_progresoInicioSesion = ProgresoInicioSesion::SesionIniciada;
					//_temporizadorMantenerSesionActiva.start();
					xmppEstablecerSesion();
				}
				break;
			case ProgresoInicioSesion::SesionIniciada: // Respuesta a algún comando
				// Sesion establecida
				re = QRegularExpression("<iq t='result' i='.+'><b1 xmlns='x4'><jid>(.+)</jid><sid>.+</sid></b1></iq>", QRegularExpression::CaseInsensitiveOption);
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
					_jID = rem.captured(1);
//					xmppEstablecerTiempoInactividad();
					_estado = Estados::Listo;
					if (_listadoSolicitudesEnlacesFirmados.size() > 0) {
						procesarColaSolicitudesEnlacesFirmados();
					}
					break;
				}
				// Tiempo de inactividad establecido
				re = QRegularExpression("<ed u='(.+)' max='(.+)' i='(.+)' xmlns='x7'/>", QRegularExpression::CaseInsensitiveOption);
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
//					xmppEstablecerPresencia();
					break;
				}
				// Presencia establecida
				re = QRegularExpression("<p xml:lang='(.+)' o='(.+)' f='(.+)' i='(.+)'/>", QRegularExpression::CaseInsensitiveOption);
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
					xmppSolicitarUsuariosBloqueados();
					xmppSolicitarListadoGruposCanales();
					_estado = Estados::Listo;
					break;
				}
				// Corrección del contador del PONG
				re = QRegularExpression("<a h='(.+)' xmlns='x7'/>", QRegularExpression::CaseInsensitiveOption);
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
					_contadorPING = rem.captured(1).toUInt() + 1;
					break;
				}
/*
				// PING
				re = QRegularExpression("<r xmlns='x7'/>", QRegularExpression::CaseInsensitiveOption);
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
					// PONG
					if (_estado == Estados::Listo) {
						xmppPONG();
					}
					break;
				}
*/
				// Comando: GURL
				re = QRegularExpression("<iq.+i='(.+)'.*><query xmlns='todus:gurl' du='(.*)' status='(.*)'/></iq>", QRegularExpression::CaseInsensitiveOption);
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
					ModeloPaquetes *modeloPaquetes = qobject_cast<ModeloPaquetes *>(parent());
					QString enlaceFirmado;

					for (int id : _listadoSolicitudesEnlacesFirmados.keys()) {
						if (_listadoSolicitudesEnlacesFirmados[id].idSesion == rem.captured(1)) {
							if (rem.captured(3) == "200") {
								enlaceFirmado = rem.captured(2);
								enlaceFirmado.replace("&amp;", "&");
								modeloPaquetes->iniciarDescarga(_listadoSolicitudesEnlacesFirmados[id].paquete, id, enlaceFirmado);
								_listadoSolicitudesEnlacesFirmados.remove(id);
							} else {
								if (_configuraciones.valor("todus/programaPiscinaFichas", false).toBool() == true) {
									socaloWebSolicitarFicha();
								}
							}
						}
					}
					break;
				}
				// Comando: PURL
				re = QRegularExpression("<iq.+i='(.+)'.*><query xmlns='todus:purl' put='(.*)' get='(.*)' status='(.*)'/></iq>", QRegularExpression::CaseInsensitiveOption);
				rem = re.match(bufer);
				if (rem.hasMatch() == true) {
					ModeloPaquetes *modeloPaquetes = qobject_cast<ModeloPaquetes *>(parent());
					QString enlace;
					QString enlaceFirmado;

					for (int id : _listadoSolicitudesEnlacesFirmados.keys()) {
						if (_listadoSolicitudesEnlacesFirmados[id].idSesion == rem.captured(1)) {
							if (rem.captured(4) == "200") {
								enlaceFirmado = rem.captured(2);
								enlaceFirmado.replace("&amp;", "&");
								enlace = rem.captured(3);
								modeloPaquetes->iniciarPublicacion(_listadoSolicitudesEnlacesFirmados[id].paquete, id, enlace, enlaceFirmado);
								_listadoSolicitudesEnlacesFirmados.remove(id);
							} else {
								if (_configuraciones.valor("todus/programaPiscinaFichas", false).toBool() == true) {
									socaloWebSolicitarFicha();
								}
							}
						}
					}
					break;
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
	_contadorComandos++;

	return _contadorComandos;
}

void toDus::solicitarCodigoSMS(const QString &telefono) {
	QString servidorAutentificacion = _configuraciones.valor("avanzadas/servidorAutentificacion", "auth.todus.cu").toString();
	std::uint16_t servidorAutentificacionPuerto = _configuraciones.valor("avanzadas/servidorAutentificacionPuerto", 443).toUInt();
	std::string datos;
	toDusPB::SolicitudSMS pbSolicitudSMS;

	emitirRegistro(TiposRegistro::Informacion, "toDus") << "Solicitando codigo de verificacion por SMS" << std::endl;

	_httpCodigoSMS = new HTTP(this);

	_httpCodigoSMS->establecerMetodo(HTTP::Metodo::PUBLICAR);
	_httpCodigoSMS->establecerAnfitrion(servidorAutentificacion);
	_httpCodigoSMS->establecerPuerto(servidorAutentificacionPuerto);
	_httpCodigoSMS->establecerRuta("v2/auth/users.reserve");

	pbSolicitudSMS.set_telefono(QString("53%1").arg(telefono).toStdString());
	pbSolicitudSMS.set_id(_idInicioSesion.toStdString());
	pbSolicitudSMS.SerializeToString(&datos);

	_buferCargaCodigoSMS = QByteArray::fromStdString(datos);
	_httpCodigoSMS->establecerOrigenCarga(&_buferCargaCodigoSMS);
	_buferDescargaCodigoSMS.clear();
	_httpCodigoSMS->establecerDestinoDescarga(&_buferDescargaCodigoSMS);

	_httpCodigoSMS->agregarCabeceraSolicitud("Content-Length", QString::number(datos.size()));
	_httpCodigoSMS->agregarCabeceraSolicitud("Content-Type", "application/x-protobuf");
	_httpCodigoSMS->agregarCabeceraSolicitud("Host", servidorAutentificacion);
	_httpCodigoSMS->agregarCabeceraSolicitud("User-Agent", _configuraciones.valor("avanzadas/agenteUsuario", _agenteUsuarioTodus).toString() + " Auth");

	connect(_httpCodigoSMS, &HTTP::finalizado, this, &toDus::eventoFinalizadaCodigoSMS);

	_estado = Estados::SolicitandoCodigoSMS;
	_httpCodigoSMS->ejecutar();
}

void toDus::solicitarFichaSolicitud(const QString &codigo) {
	QString telefono = _configuraciones.valor("todus/telefono").toString();
	QString servidorAutentificacion = _configuraciones.valor("avanzadas/servidorAutentificacion", "auth.todus.cu").toString();
	std::uint16_t servidorAutentificacionPuerto = _configuraciones.valor("avanzadas/servidorAutentificacionPuerto", 443).toUInt();
	std::string datos;
	toDusPB::SolicitudFichaSolicitud pbSolicitudFichaSolicitud;

	emitirRegistro(TiposRegistro::Informacion, "toDus") << "Verificando el codigo de verificacion suministrado" << std::endl;

	_httpFichaSolicitud = new HTTP(this);

	_httpFichaSolicitud->establecerMetodo(HTTP::Metodo::PUBLICAR);
	_httpFichaSolicitud->establecerAnfitrion(servidorAutentificacion);
	_httpFichaSolicitud->establecerPuerto(servidorAutentificacionPuerto);
	_httpFichaSolicitud->establecerRuta("v2/auth/users.register");

	_configuraciones.eliminar("todus/fichaSolicitud");

	pbSolicitudFichaSolicitud.set_telefono(QString("53%1").arg(telefono).toStdString());
	pbSolicitudFichaSolicitud.set_id(_idInicioSesion.toStdString());
	pbSolicitudFichaSolicitud.set_codigo(codigo.toStdString());
	pbSolicitudFichaSolicitud.SerializeToString(&datos);

	_buferCargaFichaSolicitud = QByteArray::fromStdString(datos);
	_httpFichaSolicitud->establecerOrigenCarga(&_buferCargaFichaSolicitud);
	_buferDescargaFichaSolicitud.clear();
	_httpFichaSolicitud->establecerDestinoDescarga(&_buferDescargaFichaSolicitud);

	_httpFichaSolicitud->agregarCabeceraSolicitud("Content-Length", QString::number(datos.size()));
	_httpFichaSolicitud->agregarCabeceraSolicitud("Content-Type", "application/x-protobuf");
	_httpFichaSolicitud->agregarCabeceraSolicitud("Host", servidorAutentificacion);
	_httpFichaSolicitud->agregarCabeceraSolicitud("User-Agent", _configuraciones.valor("avanzadas/agenteUsuario", _agenteUsuarioTodus).toString() + " Auth");

	connect(_httpFichaSolicitud, &HTTP::finalizado, this, &toDus::eventoFinalizadaFichaSolicitud);

	_estado = Estados::SolicitandoFichaSolicitud;
	_httpFichaSolicitud->ejecutar();
}

void toDus::solicitarFichaAcceso() {
	QString telefono = _configuraciones.valor("todus/telefono").toString();
	QByteArray fichaSolicitud = _configuraciones.valor("todus/fichaSolicitud").toByteArray();
	QString numeroVersion = _configuraciones.valor("toDusNumeroVersion", _numeroVersionTodus).toString();
	QString servidorAutentificacion = _configuraciones.valor("avanzadas/servidorAutentificacion", "auth.todus.cu").toString();
	std::uint16_t servidorAutentificacionPuerto = _configuraciones.valor("avanzadas/servidorAutentificacionPuerto", 443).toUInt();
	std::string datos;
	toDusPB::SolicitudFichaAcceso pbSolicitudFichaAcceso;

	emitirRegistro(TiposRegistro::Informacion, "toDus") << "Solicitando ficha de acceso a la red toDus" << std::endl;

	_httpFichaAcceso = new HTTP(this);

	_httpFichaAcceso->establecerMetodo(HTTP::Metodo::PUBLICAR);
	_httpFichaAcceso->establecerAnfitrion(servidorAutentificacion);
	_httpFichaAcceso->establecerPuerto(servidorAutentificacionPuerto);
	_httpFichaAcceso->establecerRuta("v2/auth/token");

	_configuraciones.eliminar("todus/fichaAcceso");
	_configuraciones.eliminar("todus/fichaAccesoTiempoExpiracion");

	pbSolicitudFichaAcceso.set_telefono(QString("53%1").arg(telefono).toStdString());
	pbSolicitudFichaAcceso.set_fichasolicitud(fichaSolicitud.toStdString());
	pbSolicitudFichaAcceso.set_numeroversion(numeroVersion.toStdString());
	pbSolicitudFichaAcceso.SerializeToString(&datos);

	_buferCargaFichaAcceso = QByteArray::fromStdString(datos);
	_httpFichaAcceso->establecerOrigenCarga(&_buferCargaFichaAcceso);
	_buferDescargaFichaAcceso.clear();
	_httpFichaAcceso->establecerDestinoDescarga(&_buferDescargaFichaAcceso);

	_httpFichaAcceso->agregarCabeceraSolicitud("Content-Length", QString::number(datos.size()));
	_httpFichaAcceso->agregarCabeceraSolicitud("Content-Type", "application/x-protobuf");
	_httpFichaAcceso->agregarCabeceraSolicitud("Host", servidorAutentificacion);
	_httpFichaAcceso->agregarCabeceraSolicitud("User-Agent", _configuraciones.valor("avanzadas/agenteUsuario", _agenteUsuarioTodus).toString() + " Auth");

	connect(_httpFichaAcceso, &HTTP::finalizado, this, &toDus::eventoFinalizadaFichaAcceso);

	_estado = Estados::SolicitandoFichaAcceso;
	_httpFichaAcceso->ejecutar();
	_solicitudesFichaAcceso++;
}

void toDus::iniciarSesionConFichaAcceso() {
	QString servidorMensajeria = _configuraciones.valor("avanzadas/servidorMensajeria", "im.todus.cu").toString();
	int servidorMensajeriaPuerto = _configuraciones.valor("avanzadas/servidorMensajeriaPuerto", 1756).toInt();

	//emitirRegistro(TiposRegistro::Depuracion, "toDus") << "Conectando a " << nombreDNSServidorSesion.toStdString() << ":" << puertoServidorSesion << std::endl;

//	_dominioJID = servidorMensajeriaPuerto;

	_estado = Estados::Conectando;
	_socaloSSL->setProxy(_obtenerProxy());
	_socaloSSL->connectToHostEncrypted(servidorMensajeria, servidorMensajeriaPuerto);
}

void toDus::xmppEnviar(const QString &buferAEnviar) {
//	emitirRegistro(TiposRegistro::Depuracion, "toDus") << "<< " << buferAEnviar.toStdString() << std::endl;

	_socaloSSL->write(buferAEnviar.toLocal8Bit());
}

void toDus::xmppSaludar() {
	QString buferAEnviar = "<stream:stream xmlns='jc' o='" + _dominioJID.toLocal8Bit() + "' xmlns:stream='x1' v='1.0'>";

	xmppEnviar(buferAEnviar);
}

void toDus::xmppIniciarSesion() {
//	QByteArray fichaAcceso = _configuraciones.valor("todus/fichaAcceso").toByteArray();
	QByteArray b64Telefono = QByteArray::fromBase64(_fichaAccesoActual.split('.')[1]);
	QJsonDocument json = QJsonDocument::fromJson(b64Telefono);
	QString telefono = json.object().value("username").toString();
	QString credenciales = QChar('\0') + telefono + QChar('\0') + _fichaAccesoActual;
	QString buferAEnviar = "<ah xmlns='ah:ns' e='PLAIN'>" + credenciales.toLocal8Bit().toBase64() + "</ah>";

	xmppEnviar(buferAEnviar);
}

void toDus::xmppEstablecerSesion() {
	QByteArray b64Telefono = QByteArray::fromBase64(_fichaAccesoActual.split('.')[1]);
	QJsonDocument json = QJsonDocument::fromJson(b64Telefono);
	QString telefono = json.object().value("username").toString();
	QString buferAEnviar = "<iq i='" + _idSesion.toUtf8() + "-" + QByteArray::number(obtenerProximoIDComando()) + "' t='set'><b1 xmlns='x4'><re>" + QCryptographicHash::hash(telefono.toUtf8(), QCryptographicHash::Md5).toHex() + "</re></b1></iq>";

	xmppEnviar(buferAEnviar);
}

void toDus::xmppEstablecerTiempoInactividad() {
	QString buferAEnviar = "<en xmlns='x7' u='true' max='300'/>";

	xmppEnviar(buferAEnviar);
}

void toDus::xmppEstablecerPresencia() {
	QString buferAEnviar = "<p i='" + _idSesion.toLocal8Bit() + "-" + QByteArray::number(obtenerProximoIDComando()) + "'></p>";

	xmppEnviar(buferAEnviar);
}

void toDus::xmppSolicitarUsuariosBloqueados() {
	QString buferAEnviar = "<iq i='" + _idSesion.toLocal8Bit() + "-" + QByteArray::number(obtenerProximoIDComando()) + "' t='get'><query xmlns='todus:block:get'></query></iq>";

	xmppEnviar(buferAEnviar);
}

void toDus::xmppSolicitarListadoGruposCanales() {
	QString buferAEnviar = "<iq o='muclight.im.todus.cu' i='" + _idSesion.toLocal8Bit() + "-" + QByteArray::number(obtenerProximoIDComando()) + "' t='get'><query xmlns='todus:muclight:my_mucs'></query></iq>";

	xmppEnviar(buferAEnviar);
}

void toDus::xmppMantenerSesionActiva() {
//	QByteArray buferAEnviar = "<iq i='" + _idSesion.toLocal8Bit() + "-" + QByteArray::number(obtenerProximoIDComando()) + "' t='get'><query xmlns='todus:roster:hash' hash='CB9C29EED34978062620B4734C36CAA3AC4EE2A36A72C0C7A6D899AE47828490'></query></iq>\n";
//	QByteArray buferAEnviar = "<iq i='" + _idSesion.toLocal8Bit() + "-" + QByteArray::number(obtenerProximoIDComando()) + "' t='get'><query xmlns='todus:block:get'></query></iq>\n";
	QString buferAEnviar = "<p i='" + _idSesion.toLocal8Bit() + "-" + QByteArray::number(obtenerProximoIDComando()) + "'></p>";

	xmppEnviar(buferAEnviar);
}

void toDus::xmppPONG() {
//	QString buferAEnviar = "<iq from='" + _jID + "' to='" + _dominioJID + "' id='" + _idSesion + "-" + QString::number(obtenerProximoIDComando()) + "' type='get'><ping xmlns='urn:xmpp:ping'/></iq>\n";
	QString buferAEnviar = "<a xmlns='x7' h='" + QString::number(_contadorPING++) + "'/>";

	xmppEnviar(buferAEnviar);
}

void toDus::xmppSolicitarEnlaceDescarga(qint64 id) {
	_listadoSolicitudesEnlacesFirmados[id].idSesion = _idSesion + "-" + QString::number(obtenerProximoIDComando());
	QString buferAEnviar = "<iq i='" + _listadoSolicitudesEnlacesFirmados[id].idSesion + "' t='get' from='" + _jID + "' to='" + _dominioJID + "'><query xmlns='todus:gurl' url='" + _listadoSolicitudesEnlacesFirmados[id].enlace + "'></query></iq>";

	xmppEnviar(buferAEnviar);
}

void toDus::xmppSolicitarEnlaceSubida(qint64 id, int clasificacion) {
	_listadoSolicitudesEnlacesFirmados[id].idSesion = _idSesion + "-" + QString::number(obtenerProximoIDComando());
	QString buferAEnviar = "<iq i='" + _listadoSolicitudesEnlacesFirmados[id].idSesion + "' t='get' from='" + _jID + "' to='" + _dominioJID + "'><query xmlns='todus:purl' persistent='false' room='' type='" + QString::number(clasificacion) + "' size='" + QString::number(_listadoSolicitudesEnlacesFirmados[id].tamano) + "'></query></iq>";

	xmppEnviar(buferAEnviar);
}

void toDus::iniciarPPF() {
	emitirRegistro(TiposRegistro::Informacion, "PPF") << "Conectando a atds3.herokuapp.com:443" << std::endl;

	_socaloWebConexionActiva = false;
	_socaloWebMismaFichaRecibida = false;
	_temporizadorSocaloWebPING.start();
	_socaloWeb.setProxy(_obtenerProxy());
	_socaloWeb.open(QUrl("wss://atds3.herokuapp.com"));
//	_socaloWeb.open(QUrl("ws://[::1]:8001"));
	_estadoSocaloWeb = Estados::Conectando;
}

void toDus::eventoPPFConectado() {
	emitirRegistro(TiposRegistro::Informacion, "PPF") << "Conectado" << std::endl;

	_socaloWebConexionActiva = true;
	_estadoSocaloWeb = Estados::Conectado;

	if (_fichaAccesoActual == _configuraciones.valor("todus/fichaAcceso", "").toByteArray() && _fichaAccesoActual.size() > 0) {
		socaloWebAportarFicha();
		return;
	}

	if (_listadoSolicitudesEnlacesFirmados.size() == 0) {
		_socaloWeb.close();
	}
}

void toDus::eventoPPFPong() {
	_socaloWebConexionActiva = true;
}

void toDus::eventoPPFMensajeRecibido(const QString &mensaje) {
	QJsonDocument json = QJsonDocument::fromJson(mensaje.toLocal8Bit());
	QByteArray ficha;

	if (json.object().value("accion").toString() == "entregarFicha") {
		ficha = json.object().value("ficha").toString().toLocal8Bit();
		_estado = Estados::Desconectado;

		if (ficha != _fichaAccesoActual && ficha != _configuraciones.valor("todus/fichaAcceso", "").toByteArray()) {
			emitirRegistro(TiposRegistro::Informacion, "PPF") << "Recibida una ficha de acceso nueva" << std::endl;

			_socaloWebMismaFichaRecibida = false;
			_fichaAccesoActual = ficha;
		} else {
			emitirRegistro(TiposRegistro::Informacion, "PPF") << "Recibida la misma ficha de acceso, ignorando" << std::endl;

			if (_socaloWebMismaFichaRecibida == false) {
				_socaloWebMismaFichaRecibida = true;
			}
		}
	}
}

void toDus::eventoPPFDesconectado() {
	emitirRegistro(TiposRegistro::Informacion, "PPF") << "Desconectado" << std::endl;

	_socaloWebConexionActiva = false;
	_temporizadorSocaloWebPING.stop();
	_estadoSocaloWeb = Estados::Desconectado;
}

void toDus::eventoPPFSocaloWebPING() {
	if (_estadoSocaloWeb == Estados::Conectado) {
		if (_listadoSolicitudesEnlacesFirmados.size() == 0) {
			_socaloWeb.close();
		} else {
			_socaloWeb.ping();
		}
	}
}

void toDus::eventoPPFSocaloWebVerificarConexion() {
	if (_socaloWebConexionActiva == false) {
		if (_estadoSocaloWeb == Estados::Conectado) {
			emitirRegistro(TiposRegistro::Informacion, "PPF") << "Detectada una conexión fantasma, cerrando" << std::endl;
			_socaloWeb.close();
		}
	} else {
		_socaloWebConexionActiva = false;
	}
}

void toDus::socaloWebAportarFicha() {
	if (_estadoSocaloWeb == Estados::Conectado) {
		emitirRegistro(TiposRegistro::Informacion, "PPF") << "Aportando la ficha de acceso del usuario configurado" << std::endl;

		_socaloWeb.sendTextMessage(QString("{\"accion\":\"aportarFicha\",\"ficha\":\"%1\",\"expiracion\":%2}").arg(_configuraciones.valor("todus/fichaAcceso").toString()).arg(_configuraciones.valor("todus/fichaAccesoTiempoExpiracion").toLongLong()));
		_socaloWebFichaAportada = true;

		if (_listadoSolicitudesEnlacesFirmados.size() == 0) {
			_socaloWeb.close();
		}
	}
}

void toDus::socaloWebSolicitarFicha() {
	if (_configuraciones.valor("todus/programaPiscinaFichas", false).toBool() == true) {
		if (_configuraciones.valor("todus/programaPiscinaFichasLocal", "").toString().size() > 0) {
			QStringList listadoFichasAcceso = _configuraciones.valor("todus/programaPiscinaFichasLocal", "").toString().split("\n");

			for (int i = 0; i < listadoFichasAcceso.size();) {
				if (listadoFichasAcceso[i].trimmed().isEmpty() == true) {
					listadoFichasAcceso.removeAt(i);
					continue;
				}
				i++;
			}
			if (_configuraciones.valor("todus/fichaAcceso", "").toByteArray().isEmpty() == false) {
				listadoFichasAcceso.push_front(_configuraciones.valor("todus/fichaAcceso", "").toByteArray());
			}
			if (listadoFichasAcceso.size() > 0) {
				if (_socaloWebIndiceFichaLocal >= listadoFichasAcceso.size() && _configuraciones.valor("todus/programaPiscinaFichasInternet", true).toBool() == false) {
					_socaloWebIndiceFichaLocal = 0;
				}
				if (_fichaAccesoActual != listadoFichasAcceso[_socaloWebIndiceFichaLocal]) {
					_fichaAccesoActual = listadoFichasAcceso[_socaloWebIndiceFichaLocal++].toUtf8();

					emitirRegistro(TiposRegistro::Informacion, "PPF") << "Tomada una ficha de acceso nueva desde la Piscina de Fichas de Acceso local" << std::endl;
				}
			}
		}
		if (_configuraciones.valor("todus/programaPiscinaFichasInternet", true).toBool() == true) {
			if (_estadoSocaloWeb == Estados::Conectado) {
				_estado = Estados::SolicitandoFichaAcceso;
				desconectar();

				emitirRegistro(TiposRegistro::Informacion, "PPF") << "Solicitando una ficha de acceso nueva" << std::endl;

				for (qint64 id : _listadoSolicitudesEnlacesFirmados.keys()) {
					_listadoSolicitudesEnlacesFirmados[id].reintentos = 0;
				}

				_socaloWeb.sendTextMessage(QString("{\"accion\":\"solicitarFicha\"}"));
			} else {
				iniciarPPF();
			}
		}
	}
}
