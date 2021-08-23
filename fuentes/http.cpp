#include "http.hpp"
#include "main.hpp"
#include <QDataStream>
#include <QTextStream>
#include <QNetworkRequest>


HTTP::HTTP(QObject *padre)
	: QNetworkAccessManager(padre), _id(0), _error(false), _detenido(false), _detenidoParaReiniciar(false), _ejecutado(false), _cabecerasRecibidas(false), _metodo(Metodo::OBTENER), _puerto(443), _tiempoUltimaCarga(std::time(nullptr)), _tiempoUltimaDescarga(std::time(nullptr)), _bytesCargados(0), _bytesCargadosInicialmente(0), _bytesDescargados(0), _bytesDescargadosInicialmente(0) {
	_cabecerasSolicitud["accept"] = "*/*";
	_cabecerasSolicitud["connection"] = "close";

	setAutoDeleteReplies(true);
	setTransferTimeout(15000);
	setStrictTransportSecurityEnabled(true);
	setProxy(_obtenerProxy());
}

void HTTP::establecerId(int id) {
	_id = id;
}

bool HTTP::error() {
	return _error;
}

HTTP::Metodo HTTP::metodo() {
	return _metodo;
}
void HTTP::establecerMetodo(HTTP::Metodo metodo) {
	_metodo = metodo;
}

QString HTTP::anfitrion() {
	return _anfitrion;
}
void HTTP::establecerAnfitrion(const QString &anfitrion) {
	_anfitrion = anfitrion;
}

std::uint16_t HTTP::puerto() {
	return _puerto;
}
void HTTP::establecerPuerto(std::uint16_t puerto) {
	_puerto = puerto;
}

QString HTTP::ruta() {
	return _ruta;
}
void HTTP::establecerRuta(const QString &ruta) {
	_ruta = ruta;
}

QMap<QString, QString> HTTP::cabecerasSolicitud() {
	return _cabecerasSolicitud;
}
void HTTP::agregarCabeceraSolicitud(const QString &llave, const QString &valor) {
	if (llave.trimmed().size() > 0 && valor.trimmed().size() > 0) {
		_cabecerasSolicitud[llave.trimmed().toLower()] = valor;
	}
}

QString HTTP::cabeceraRespuesta(const QString &cabecera) {
	return _cabecerasRespuesta[cabecera.toLower()];
}
QMap<QString, QString> HTTP::cabecerasRespuesta() {
	return _cabecerasRespuesta;
}

std::int64_t HTTP::bytesCargados() {
	return _bytesCargados;
}
void HTTP::establecerBytesCargados(std::int64_t bytes) {
	_bytesCargadosInicialmente = bytes;
	_bytesCargados = _bytesCargadosInicialmente;
}

std::int64_t HTTP::bytesDescargados() {
	return _bytesDescargados;
}
void HTTP::establecerBytesDescargados(std::int64_t bytes) {
	_bytesDescargadosInicialmente = bytes;
	_bytesDescargados = _bytesDescargadosInicialmente;
}

void HTTP::establecerOrigenCarga(QByteArray *origen) {
	_datosPublicar = origen;
}
void HTTP::establecerOrigenCarga(QIODevice *origen) {
	_archivoPublicar = origen;
}

void HTTP::establecerDestinoDescarga(QByteArray *destino) {
	_datosDescarga = destino;
}
void HTTP::establecerDestinoDescarga(QIODevice *destino) {
	_archivoDescarga = destino;
}

std::uint16_t HTTP::codigoHTTP() {
	return _codigoHTTP;
}

bool HTTP::ejecutar() {
	QUrl enlace(QString("https://%1/%2").arg(_anfitrion).arg(_ruta));
	enlace.setPort(_puerto);
	QNetworkRequest solicitud(enlace);
	QSslConfiguration configuracionSSL = solicitud.sslConfiguration();

	solicitud.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
	solicitud.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);
	solicitud.setAttribute(QNetworkRequest::BackgroundRequestAttribute, true);
	solicitud.setPriority(QNetworkRequest::Priority::HighPriority);

	for (const QString &cabecera : _cabecerasSolicitud.keys()) {
		solicitud.setRawHeader(cabecera.toUtf8(), _cabecerasSolicitud[cabecera].toUtf8());
	}

	_error = false;
	_detenido = false;
	_ejecutado = false;
	_cabecerasRecibidas = false;
	_cabecerasRespuesta.clear();
	_codigoHTTP = 502;
	_tiempoUltimaCarga = std::time(nullptr);
	_tiempoUltimaDescarga = _tiempoUltimaCarga;

	switch (_metodo) {
		case Metodo::OBTENER:
			_respuesta = get(solicitud);

			connect(_respuesta, &QNetworkReply::readyRead, this, &HTTP::eventoDescargarDatos);
			break;
		case Metodo::PUBLICAR:
			_respuesta = post(solicitud, *_datosPublicar);

			connect(_respuesta, &QNetworkReply::readyRead, this, &HTTP::eventoDescargarDatos);
			break;
		case Metodo::PONER:
			_respuesta = put(solicitud, _archivoPublicar);

			connect(_respuesta, &QNetworkReply::uploadProgress, this, &HTTP::eventoProgresoCargarDatos);
			break;
	}

	_respuesta->ignoreSslErrors();

	_ejecutado = true;

	connect(_respuesta, &QNetworkReply::metaDataChanged, this, &HTTP::eventoCabecerasDescargadas);
	connect(_respuesta, &QNetworkReply::errorOccurred, this, &HTTP::eventoError);
	connect(_respuesta, &QNetworkReply::finished, this, &HTTP::eventoFinalizado);

	emitirRegistro(TiposRegistro::Informacion, "HTTP") << "[" << _id << "] " << "Conectando a " << _anfitrion.toStdString() << ":" << _puerto << std::endl;

	return true;
}

void HTTP::detener() {
	_detenido = true;

	if (_respuesta) {
		_respuesta->abort();
	}
}

void HTTP::eventoCabecerasDescargadas() {
	if (_ejecutado == false || _detenido == true) {
		return;
	}

	for (const QPair<QByteArray, QByteArray> &cabecera : _respuesta->rawHeaderPairs()) {
		_cabecerasRespuesta[cabecera.first.toLower()] = cabecera.second;
//		emitirRegistro(TiposRegistro::Depuracion, "HTTP") << "[" << _id << "] " << cabecera.first.toStdString() << ": " << cabecera.second.toStdString() << std::endl;
	}

	_codigoHTTP = _respuesta->attribute(QNetworkRequest::HttpStatusCodeAttribute).toUInt();
	if (_codigoHTTP >= 400) {
		switch (_codigoHTTP) {
			case 416:
				_detenido = true;
				break;
			default:
				_error = true;
				break;
		}
	}

	_cabecerasRecibidas = true;
	emit cabecerasRecibidas(_id);
}

void HTTP::eventoProgresoCargarDatos(qint64 enviado, qint64 total) {
	if (_ejecutado == false || _detenido == true || _error == true) {
		return;
	}

	std::time_t tiempoActual = std::time(nullptr);

	Q_UNUSED(total);

	_bytesCargados = _bytesCargadosInicialmente + enviado;

	if (tiempoActual - _tiempoUltimaCarga >= 1) {
		emit bytesCargaTransferidos(_id, _bytesCargados);
		_tiempoUltimaCarga = tiempoActual;
	}
}

void HTTP::eventoDescargarDatos() {
	if (_ejecutado == false || _cabecerasRecibidas == false || _detenido == true || _error == true || (_codigoHTTP != 200 && _codigoHTTP != 206)) {
		return;
	}

	std::time_t tiempoActual = std::time(nullptr);
	QByteArray datosRecibidos = _respuesta->readAll();

	_bytesDescargados += datosRecibidos.size();

	if (tiempoActual - _tiempoUltimaDescarga >= 1) {
		emit bytesDescargaTransferidos(_id, _bytesDescargados);
		_tiempoUltimaDescarga = tiempoActual;
	}

	switch (_metodo) {
		case Metodo::OBTENER:
			if (_archivoDescarga) {
				_archivoDescarga->write(datosRecibidos);
			}
			break;
		case Metodo::PUBLICAR:
			if (_datosDescarga) {
				*_datosDescarga += datosRecibidos;
			}
			break;
		default:
			break;
	}
}

void HTTP::eventoFinalizado() {
	bool terminacionExitosa = false;

	emitirRegistro(TiposRegistro::Informacion, "HTTP") << "[" << _id << "] " << "Desconectado" << std::endl;

	if (_detenido == false && _cabecerasRecibidas == true && _error == false) {
		switch (_metodo) {
			case Metodo::OBTENER:
			case Metodo::PUBLICAR:
				if (_bytesDescargados - _bytesDescargadosInicialmente >= _cabecerasRespuesta["content-length"].toLongLong()) {
					terminacionExitosa = true;
				}
				break;
			case Metodo::PONER:
				terminacionExitosa = true;
				if (_bytesCargados >= _cabecerasSolicitud["content-length"].toLongLong()) {
					terminacionExitosa = true;
				}
				break;
		}
	}

	if (terminacionExitosa == true) {
		emit finalizado(_id);
	} else {
		if (_codigoHTTP >= 200 && _codigoHTTP < 300) {
			if (_detenido == true) {
				emit detenido(_id);
			} else {
				emit detenidoParaReiniciar(_id);
			}
		} else {
			if (_codigoHTTP == 416) {
				_error = false;
				emit finalizado(_id);
			} else {
				if (_metodo == HTTP::Metodo::OBTENER && (_codigoHTTP < 300 || _codigoHTTP == 502)) {
					emit detenidoParaReiniciar(_id);
				} else {
					emit detenido(_id);
				}
			}
		}
	}
}

void HTTP::eventoError(QNetworkReply::NetworkError errorSocalo) {
	switch (errorSocalo) {
		case QNetworkReply::OperationCanceledError:
			return;
		default:
			_error = true;
			break;
	}

	emitirRegistro(TiposRegistro::Informacion, "HTTP") << "[" << _id << "] " << "Error en socalo: " << errorSocalo << "; Codigo HTTP: " << _codigoHTTP << std::endl;

	if (_id == 0) {
		emit finalizado(_id);
	}
}
