#include "todus.hpp"
#include <QSettings>
#include <QRegExp>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QRandomGenerator>
#include <iostream>


toDus::toDus(QObject *padre)
	: QSslSocket(padre), _progresoInicioSesion(ProgresoInicioSesion::Ninguno), _contadorComandos(1), _reconexionSolicitada(false) {
	generarIDSesion();
	setPeerVerifyMode(QSslSocket::VerifyNone);
	_temporizadorMantenerSesionActiva.setInterval(60000);

	connect(this, &QSslSocket::stateChanged, this, &toDus::eventoCambiarEstado);
	connect(this, &QSslSocket::encrypted, this, &toDus::eventoConexionLista);
	connect(this, &QSslSocket::readyRead, this, &toDus::eventoDatosRecibidos);
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
	disconnectFromHost();
}

/**
 * @brief Reconectar
 */
void toDus::reconectar() {
	_reconexionSolicitada = true;
	disconnectFromHost();
}

/**
 * @brief Inicia la sesión en toDus partiendo del número de teléfono o ficha de acceso configurada
 */
void toDus::iniciarSesion() {
	QSettings configuracion;
	QString telefono = configuracion.value("todus/telefono").toString();
	QString fichaAcceso = configuracion.value("todus/fichaAcceso").toString();

	if (fichaAcceso.size() > 0) {
		iniciarSesionConFicha();
	} else if (telefono.size() > 0) {
		iniciarSesionConTelefono();
	}
}

/**
 * @brief Solicita a toDus el enlace firmado para obtener un archivo
 * @param enlace Enlace no firmado
 */
void toDus::solicitarEnlaceFirmado(const QString &enlace) {
	if (_progresoInicioSesion == ProgresoInicioSesion::SesionIniciada) {
		xmppSolicitarEnlaceDescarga(enlace);
	}
}

void toDus::eventoCambiarEstado(QAbstractSocket::SocketState estado) {
	switch (estado) {
		case QAbstractSocket::UnconnectedState:
			_estado = Estado::Desconectado;
			if (_reconexionSolicitada == true) {
				_reconexionSolicitada = false;
				iniciarSesion();
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
	QRegExp re;

	while (bytesAvailable() > 0) {
		bufer = readAll();

		switch (_progresoInicioSesion) {
			case ProgresoInicioSesion::Saludo: // Respuesta al saludo
				re = QRegExp("<stream:features>.+<e>PLAIN</e>.+</stream:features>");
				if (re.indexIn(bufer) != -1) {
					_progresoInicioSesion = ProgresoInicioSesion::MecanismosAutentificacion;
					xmppIniciarSesion();
				}
				break;
			case ProgresoInicioSesion::MecanismosAutentificacion: // Respuesta al intento de autentificación
				re = QRegExp("<ok xmlns='x2'/>");
				if (re.indexIn(bufer) != -1) {
					_progresoInicioSesion = ProgresoInicioSesion::Autenficicacion;
					xmppSaludar();
					xmppEstablecerSesion();
				} else {
					re = QRegExp("<failure xmlns='x2'><not-authorized/>.+</failure>");
					if (re.indexIn(bufer) != -1) {
						_progresoInicioSesion = ProgresoInicioSesion::Ninguno;
						desconectar();
					}
				}
				break;
			case ProgresoInicioSesion::Autenficicacion: // Respuesta satisfactoria al intento de establecer sesión
				re = QRegExp("<iq t='result' i='" + _idSesion + "-" + QString::fromStdString(std::to_string(_contadorComandos - 1)) + "'>.+<jid>(.+)</jid>.+</iq>");
				if (re.indexIn(bufer) != -1) {
					_progresoInicioSesion = ProgresoInicioSesion::SesionIniciada;
					_jID = re.cap(1);
					_temporizadorMantenerSesionActiva.start();
					_estado = Estado::Listo;
					emit estadoCambiado(_estado);
				}
				break;
			case ProgresoInicioSesion::SesionIniciada: // Respuesta a algún comando
				// La respuesta está dirigida a esta sesión?
				re = QRegExp("<iq.+o='" + _jID + "'.+t='result'.+>.+</iq>");
				if (re.indexIn(bufer) != -1) {
					// Comando: GURL
					re = QRegExp("<iq.+i='(.+)'.*><query xmlns='todus:gurl' du='(.+)' status='200'/></iq>");
					if (re.indexIn(bufer) != -1) {
						QString idSesion = re.cap(1);
						QString enlaceNoFirmado = _listadoEnlacesFirmados[idSesion];
						_listadoEnlacesFirmados.remove(idSesion);
						emit enlaceFirmadoObtenido(enlaceNoFirmado, re.cap(2));
					}
				}
				break;
			default:
//				std::cout << bufer.toStdString() << std::endl;
				break;
		}
	}
}

void toDus::generarIDSesion() {
	int codigoAleatorio = 0;
	unsigned int caractersObtenidos = 0;

	while (caractersObtenidos < 5) {
		codigoAleatorio = QRandomGenerator::global()->bounded(48, 122);
		if (codigoAleatorio >= 48 && codigoAleatorio <= 57) {
			_idSesion.push_back(QChar(codigoAleatorio));
			caractersObtenidos++;
			continue;
		}
		if (codigoAleatorio >= 65 && codigoAleatorio <= 90) {
			_idSesion.push_back(QChar(codigoAleatorio));
			caractersObtenidos++;
			continue;
		}
		if (codigoAleatorio >= 97 && codigoAleatorio <= 122) {
			_idSesion.push_back(QChar(codigoAleatorio));
			caractersObtenidos++;
			continue;
		}
	}
}

void toDus::iniciarSesionConTelefono() {

}

void toDus::iniciarSesionConFicha() {
	QSettings configuracion;
	QString ipServidorSesion = configuracion.value("todus/ipServidorSesion", "").toString();
	QString nombreDNSServidorSesion = configuracion.value("todus/nombreDNSServidorSesion", "im.todus.cu").toString();
	int puertoServidorSesion = configuracion.value("todus/puertoServidorSesion", 1756).toInt();

	_dominioJID = nombreDNSServidorSesion;
	if (ipServidorSesion.size() > 0) {
		nombreDNSServidorSesion = ipServidorSesion;
	}

	connectToHostEncrypted(nombreDNSServidorSesion, puertoServidorSesion);
}

void toDus::xmppSaludar() {
	QSettings configuracion;
	QString nombreDNSServidorSesion = configuracion.value("todus/nombreDNSServidorSesion", "im.todus.cu").toString();
	QByteArray buferAEnviar = "<stream:stream xmlns='jc' o='" + nombreDNSServidorSesion.toLocal8Bit() + "' xmlns:stream='x1' v='1.0'>\n";

	write(buferAEnviar);
}

void toDus::xmppIniciarSesion() {
	QSettings configuracion;
	QString nombreDNSServidorSesion = configuracion.value("todus/nombreDNSServidorSesion", "im.todus.cu").toString();
	QString fichaAcceso = configuracion.value("todus/fichaAcceso").toString();
	QString b64Telefono = QByteArray::fromBase64(fichaAcceso.split(".")[1].toLocal8Bit());
	QJsonDocument json = QJsonDocument::fromJson(b64Telefono.toLocal8Bit());
	QString telefono = json.object().value("username").toString();
	QString credenciales = '\0' + telefono + '\0' + fichaAcceso;
	QByteArray buferAEnviar = "<ah xmlns='ah:ns' e='PLAIN'>" + credenciales.toLocal8Bit().toBase64() + "</ah>\n";

	write(buferAEnviar);
}

void toDus::xmppEstablecerSesion() {
	QByteArray buferAEnviar = "<iq i='" + _idSesion.toLocal8Bit() + "-" + QByteArray::fromStdString(std::to_string(_contadorComandos++)) + "' t='set'><b1 xmlns='x4'></b1></iq>\n";

	write(buferAEnviar);
}

void toDus::xmppMantenerSesionActiva() {
	QString buferAEnviar = "<iq from='" + _jID + "' to='" + _dominioJID + "' id='" + _idSesion + "-" + QString::fromStdString(std::to_string(_contadorComandos++)) + "' type='get'><ping xmlns='urn:xmpp:ping'/></iq>\n";

	write(buferAEnviar.toLocal8Bit());
}

void toDus::xmppSolicitarEnlaceDescarga(const QString &enlace) {
	QString idSesion = _idSesion + "-" + QString::fromStdString(std::to_string(_contadorComandos++));
	QString buferAEnviar = "<iq i='" + idSesion + "' t='get' from='" + _jID + "' to='" + _dominioJID + "' ><query xmlns='todus:gurl' url='" + enlace + "'></query></iq>\n";

	_listadoEnlacesFirmados[idSesion] = enlace;

	write(buferAEnviar.toLocal8Bit());
}
