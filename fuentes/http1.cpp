#include "http1.hpp"
#include "main.hpp"
#include <cstring>
#include <QMutexLocker>
#include <QTextStream>


HTTP1::HTTP1(QObject *padre)
	: QSslSocket(padre) {
	_cabecerasSolicitud["accept"] = "*/*";
	_cabecerasSolicitud["connection"] = "close";

	connect(this, &QSslSocket::encrypted, this, &HTTP1::eventoConectado);
	connect(this, &QSslSocket::readyRead, this, &HTTP1::eventoRecibirDatos);
	connect(this, &QSslSocket::disconnected, this, &HTTP1::eventoDesconectado);
}

HTTP1::Metodo HTTP1::metodo() {
	QMutexLocker b(&_mutexMetodo);
	return _metodo;
}
void HTTP1::establecerMetodo(HTTP1::Metodo metodo) {
	QMutexLocker b(&_mutexMetodo);
	_metodo = metodo;
}

QString HTTP1::anfitrion() {
	QMutexLocker b(&_mutexAnfitrion);
	return _anfitrion;
}
void HTTP1::establecerAnfitrion(const QString &anfitrion) {
	QMutexLocker b(&_mutexAnfitrion);
	_anfitrion = anfitrion;
}

std::uint16_t HTTP1::puerto() {
	QMutexLocker b(&_mutexPuerto);
	return _puerto;
}
void HTTP1::establecerPuerto(std::uint16_t puerto) {
	QMutexLocker b(&_mutexPuerto);
	_puerto = puerto;
}

QString HTTP1::ruta() {
	QMutexLocker b(&_mutexRuta);
	return _ruta;
}
void HTTP1::establecerRuta(const QString &ruta) {
	QMutexLocker b(&_mutexRuta);
	_ruta = ruta;
}

QMap<QString, QString> HTTP1::cabecerasSolicitud() {
	QMutexLocker b(&_mutexCabecerasSolicitud);
	return _cabecerasSolicitud;
}
void HTTP1::agregarCabeceraSolicitud(const QString &llave, const QString &valor) {
	QMutexLocker b(&_mutexCabecerasSolicitud);

	if (llave.trimmed().size() > 0 && valor.trimmed().size() > 0) {
		_cabecerasSolicitud[llave.trimmed().toLower()] = valor;
	}
}

QString HTTP1::cabeceraRespuesta(const QString &cabecera) {
	QMutexLocker b(&_mutexCabecerasRespuesta);
	return _cabecerasRespuesta[cabecera];
}
QMap<QString, QString> HTTP1::cabecerasRespuesta() {
	QMutexLocker b(&_mutexCabecerasRespuesta);
	return _cabecerasRespuesta;
}

void HTTP1::establecerOrigenCarga(QByteArray *origen) {
	QMutexLocker b(&_mutexFlujoCarga);

	_flujoCarga = std::make_unique<QDataStream>(origen, QIODevice::ReadOnly);
}
void HTTP1::establecerOrigenCarga(QIODevice *origen) {
	QMutexLocker b(&_mutexFlujoCarga);

	_flujoCarga = std::make_unique<QDataStream>(origen);
}

void HTTP1::establecerDestinoDescarga(QByteArray *destino) {
	QMutexLocker b(&_mutexFlujoDescarga);

	_flujoDescarga = std::make_unique<QDataStream>(destino, QIODevice::WriteOnly);
}
void HTTP1::establecerDestinoDescarga(QIODevice *destino) {
	QMutexLocker b(&_mutexFlujoDescarga);

	_flujoDescarga = std::make_unique<QDataStream>(destino);
}

std::uint16_t HTTP1::codigoHTTP() {
	QMutexLocker b(&_mutexCodigoHTTP);
	return _codigoHTTP;
}

bool HTTP1::ejecutar() {
	QMutexLocker ba(&_mutexAnfitrion);
	QMutexLocker bp(&_mutexPuerto);

	if (state() != QAbstractSocket::UnconnectedState) {
		return false;
	}

	_error = false;
	_cabecerasRecibidas = false;
	_tamanoRecibido = 0;
	_tiempoUltimaRecepcion = std::time(nullptr);
	setPeerVerifyMode(QSslSocket::VerifyNone);
	connectToHostEncrypted(_anfitrion, _puerto);

	return true;
}

void HTTP1::eventoConectado() {
	QMutexLocker bm(&_mutexMetodo);
	QMutexLocker ba(&_mutexAnfitrion);
	QMutexLocker bp(&_mutexPuerto);
	QMutexLocker br(&_mutexRuta);
	QMutexLocker bcs(&_mutexCabecerasSolicitud);
	QString mensajeHTTP;
	QTextStream flujo(&mensajeHTTP);
	char bufer[1048576];
	int tamanoBufer = 0;

	switch (_metodo) {
		case Metodo::OBTENER:
			flujo << "GET ";
			break;
		case Metodo::PUBLICAR:
			flujo << "POST ";
			break;
		case Metodo::PONER:
			flujo << "PUT ";
			break;
	}
	flujo << _ruta << " HTTP/1.1\r\n";
	if (_cabecerasSolicitud.contains("host") == false) {
		flujo << "Host: " << _anfitrion;
		if ((std::uint16_t)_anfitrion.at(0).toLatin1() < 40) {
			flujo << ":" << _puerto;
		}
		flujo << "\r\n";
	}
	for (const auto &cabecera : _cabecerasSolicitud.keys()) {
		flujo << cabecera << ": " << _cabecerasSolicitud[cabecera] << "\r\n";
	}
	flujo << "\r\n";

	write(mensajeHTTP.toLocal8Bit());

	emit conectado();

	if ((_metodo == Metodo::PUBLICAR || _metodo == Metodo::PONER) && _flujoCarga) {
		if (_flujoCarga->device()->seek(0) == true) {
			while (_flujoCarga->atEnd() == false) {
				std::memset(bufer, '\0', 1048576);
				tamanoBufer = _flujoCarga->readRawData(bufer, 1048576);
				write(bufer, tamanoBufer);
				emit bytesCargados(tamanoBufer);
			}
		}
	}
}

void HTTP1::eventoRecibirDatos() {
	if (_cabecerasRecibidas == false) {
		QByteArray datos = readAll();
		QTextStream flujo(&datos);
		QString versionHTTP, codigoHTTP, textoCodigoHTTP;
		QString cabeceraNombre, cabeceraValor;
		QByteArray cuerpo;
		int transferido = 0;

		flujo >> versionHTTP >> codigoHTTP >> textoCodigoHTTP;
		if (versionHTTP.toLower() == "http/1.1") {
			QMutexLocker b(&_mutexCodigoHTTP);
			_codigoHTTP = codigoHTTP.toUInt();
		}

		while (true) {
			flujo >> cabeceraNombre;
			if (cabeceraNombre.size() == 0) {
				break;
			}
			cabeceraNombre.chop(1);
			cabeceraValor = flujo.readLine().trimmed();
			_cabecerasRespuesta[cabeceraNombre] = cabeceraValor;
		}
		transferido = datos.indexOf("\r\n\r\n") + 4;
		if (transferido > 4 && datos.size() > transferido) {
			cuerpo = datos.mid(transferido);
			transferido = _flujoDescarga->writeRawData(cuerpo.constData(), cuerpo.size());
			if (transferido != -1) {
				emit bytesDescargados(transferido);
			}
		}

		_cabecerasRecibidas = true;
		emit cabecerasRecibidas();
	} else {
		_tamanoDescargado = bytesAvailable();

		if (_flujoDescarga->writeRawData(readAll().constData(), _tamanoDescargado) != -1) {
			emit bytesDescargados(_tamanoDescargado);
		}
	}
}

void HTTP1::eventoDesconectado() {
	emit finalizado();
}
