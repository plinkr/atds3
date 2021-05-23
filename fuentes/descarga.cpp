#include "descarga.hpp"
#include "modeloentradas.hpp"
#include "ventanaprincipal.hpp"
#include "main.hpp"
#include <type_traits>
#include <functional>
#include <QSettings>
#include <QFileInfo>
#include <QTimer>


Descarga::Descarga(unsigned int id, QSharedPointer<ModeloEntradas> modelo, QSharedPointer<ModeloEntradas> modeloDescargando, GestorDescargas *padre)
	: QObject(), _padre(padre), _iniciado(false), _deteniendo(false), _error(false), _id(id), _filaModelo(0), _filaModeloDescargando(0), _modelo(modelo), _modeloDescargas(modeloDescargando), _respuesta(nullptr), _ultimoTiempoRecepcion(0), _ultimoTamanoRecibido(0), _tamanoPrevio(0) {
	moveToThread(padre->thread());
}

void Descarga::descargaIniciada() {
	if (_deteniendo == true) {
		_respuesta->abort();
		return;
	}

	if (modelosValido() == false) {
		_filaModelo = encontrarFilaDesdeId(_modelo);
		_filaModeloDescargando = encontrarFilaDesdeId(_modeloDescargas);
	}

	_modelo->setData(_modelo->index(_filaModelo, 1), _ListadoEstados::Iniciada);
	_modelo->submitAll();
	_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargando, 1), _ListadoEstados::Iniciada);

	_iniciado = true;
}

void Descarga::eventoRecepcionDatos() {
	_archivo.write(_respuesta->readAll());

	if (_deteniendo == true) {
		_respuesta->abort();
	}
}

void Descarga::progresoDescarga(qint64 recibidos, qint64 total) {
	std::time_t tiempoRecepcion = std::time(nullptr);

	if (tiempoRecepcion - _ultimoTiempoRecepcion >= 1) {
		if (modelosValido() == false) {
			_filaModelo = encontrarFilaDesdeId(_modelo);
			_filaModeloDescargando = encontrarFilaDesdeId(_modeloDescargas);
		}

		recibidos += _tamanoPrevio;
		total += _tamanoPrevio;

		if (total > 0) {
			_modelo->setData(_modelo->index(_filaModelo, 3), (recibidos * 100) / total);
			_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargando, 3), (recibidos * 100) / total);
		}
		_modelo->setData(_modelo->index(_filaModelo, 4), recibidos - _ultimoTamanoRecibido);
		_modelo->setData(_modelo->index(_filaModelo, 8), total);
		_modelo->setData(_modelo->index(_filaModelo, 9), recibidos);

		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargando, 4), recibidos - _ultimoTamanoRecibido);
		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargando, 8), total);
		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargando, 9), recibidos);

		_ultimoTiempoRecepcion = tiempoRecepcion;
		_ultimoTamanoRecibido = recibidos;
	}
}

void Descarga::eventoError(QNetworkReply::NetworkError codigo) {
	_archivo.close();

	switch (codigo) {
		case QNetworkReply::NoError:
			break;
		case QNetworkReply::RemoteHostClosedError:
		case QNetworkReply::HostNotFoundError:
		case QNetworkReply::TemporaryNetworkFailureError:
		case QNetworkReply::TimeoutError:
		case QNetworkReply::ProxyTimeoutError:
			_iniciado = false;
			_error = true;

			if (_deteniendo == true) { return; }

			if (modelosValido() == false) {
				_filaModelo = encontrarFilaDesdeId(_modelo);
				_filaModeloDescargando = encontrarFilaDesdeId(_modeloDescargas);
			}
			_modelo->setData(_modelo->index(_filaModelo, 1), _ListadoEstados::EnEsperaIniciar);
			_modelo->submitAll();

			QTimer::singleShot(2000, this, &Descarga::iniciar);
			break;
		default:
			_iniciado = false;
			_error = true;
			break;
	}
}

void Descarga::descargaTerminada() {
	_archivo.close();

	if (modelosValido() == false) {
		_filaModelo = encontrarFilaDesdeId(_modelo);
		_filaModeloDescargando = encontrarFilaDesdeId(_modeloDescargas);
	}

	_modelo->setData(_modelo->index(_filaModelo, 4), 0);

	if (_respuesta->error() == QNetworkReply::NoError) {
		_modelo->setData(_modelo->index(_filaModelo, 1), _ListadoEstados::Finalizada);
		_modelo->setData(_modelo->index(_filaModelo, 3), 100);
		_modelo->setData(_modelo->index(_filaModelo, 9), _modelo->index(_filaModelo, 8));
		_modelo->submitAll();

		_iniciado = false;

		emit terminada(_id);
	}
}

bool Descarga::iniciado() {
	return _iniciado;
}

void Descarga::iniciar() {
	if (_deteniendo == true) { return; }

	if (_toDus->obtenerEstado() == toDus::Estado::Listo) {
		if (modelosValido() == false) {
			_filaModelo = encontrarFilaDesdeId(_modelo);
			_filaModeloDescargando = encontrarFilaDesdeId(_modeloDescargas);
		}
		_enlaceNoFirmado = _modelo->data(_modelo->index(_filaModelo, 7)).toString();

		if (_iniciado == false) {
			_toDus->solicitarEnlaceFirmado(_enlaceNoFirmado, std::bind(&Descarga::procesarRespuestaDesdeTodus, this, std::placeholders::_1));
		}
	} else {
		QTimer::singleShot(2000, this, &Descarga::iniciar);
	}
}

void Descarga::detener() {
	_deteniendo = true;

	if (_iniciado == true) {
		_respuesta->abort();
	}

	if (modelosValido() == false) {
		_filaModelo = encontrarFilaDesdeId(_modelo);
		_filaModeloDescargando = encontrarFilaDesdeId(_modeloDescargas);
	}

	_modelo->setData(_modelo->index(_filaModelo, 1), _ListadoEstados::Pausada);
	_modelo->setData(_modelo->index(_filaModelo, 4), 0);
	_modelo->submitAll();
}

int Descarga::fila() {
	return _filaModelo;
}

QSharedPointer<ModeloEntradas> Descarga::modelo() {
	return _modelo;
}

QSharedPointer<ModeloEntradas> Descarga::modeloDescargas() {
	return _modeloDescargas;
}

bool Descarga::error() {
	return _error;
}

void Descarga::procesarRespuestaDesdeTodus(const QString &firmado) {
	QString enlaceProcesado = firmado;

	if (_deteniendo == true) { return; }

	enlaceProcesado.replace("&amp;", "&");
	_enlaceFirmado = enlaceProcesado;

	QTimer::singleShot(0, this, &Descarga::iniciarDescarga);
}

void Descarga::iniciarDescarga() {
	QSettings configuracion;
	QString ipServidorS3 = configuracion.value("avanzadas/ipServidorS3", "").toString();
	QString nombreDNSServidorS3 = configuracion.value("avanzadas/nombreDNSServidorS3", "s3.todus.cu").toString();
	int puertoServidorS3 = configuracion.value("avanzadas/puertoServidorS3", 443).toInt();
	QUrl url = QUrl(_enlaceFirmado);
	QNetworkRequest solicitud;
	QString autorizacion = "Bearer " + configuracion.value("todus/fichaAcceso").toString();
	QFileInfo informacionArchivo;

	if (_deteniendo == true) { return; }

	_tamanoPrevio = 0;

	if (modelosValido() == false) {
		_filaModelo = encontrarFilaDesdeId(_modelo);
		_filaModeloDescargando = encontrarFilaDesdeId(_modeloDescargas);
	}

	QString rutaArchivo = _modelo->data(_modelo->index(_filaModelo, 5)).toString() + "/" + _modelo->data(_modelo->index(_filaModelo, 2)).toString();

	informacionArchivo.setFile(rutaArchivo);
	if (informacionArchivo.exists() == true) {
		_tamanoPrevio = informacionArchivo.size();
	}

	solicitud.setAttribute(QNetworkRequest::AutoDeleteReplyOnFinishAttribute, true);
	solicitud.setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);

	if (ipServidorS3.size() > 0) {
		url.setHost(ipServidorS3);
	}
	url.setPort(puertoServidorS3);

	solicitud.setUrl(url);
	solicitud.setHeader(QNetworkRequest::UserAgentHeader, configuracion.value("avanzadas/agenteUsuario", _agenteUsuarioTodus).toString() + " HTTP-Download");
	solicitud.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
	solicitud.setRawHeader("Authorization", autorizacion.toLocal8Bit());
	solicitud.setRawHeader("Host", nombreDNSServidorS3.toLocal8Bit());

	if (_tamanoPrevio > 0) {
		solicitud.setRawHeader("Range", "bytes=" + QString::number(_tamanoPrevio).toLocal8Bit() + "-");
	}

	_archivo.setFileName(rutaArchivo);
	_archivo.open(QIODevice::Append);

	_ultimoTiempoRecepcion = std::time(nullptr);

	_administradorAccesoRed = new QNetworkAccessManager(this);
	_administradorAccesoRed->setProxy(obtenerConfiguracionProxy());
	_administradorAccesoRed->setTransferTimeout(10000);
	_respuesta = _administradorAccesoRed->get(solicitud);
	connect(_respuesta, &QNetworkReply::encrypted, this, &Descarga::descargaIniciada);
	connect(_respuesta, &QIODevice::readyRead, this, &Descarga::eventoRecepcionDatos);
	connect(_respuesta, &QNetworkReply::downloadProgress, this, &Descarga::progresoDescarga);
	connect(_respuesta, &QNetworkReply::errorOccurred, this, &Descarga::eventoError);
	connect(_respuesta, &QNetworkReply::finished, this, &Descarga::descargaTerminada);
}

bool Descarga::modelosValido() {
	if (_modelo->data(_modelo->index(_filaModelo, 0)).toUInt() != _id) {
		return false;
	}
	if (_modeloDescargas->data(_modeloDescargas->index(_filaModeloDescargando, 0)).toUInt() != _id) {
		return false;
	}

	return true;
}

unsigned int Descarga::encontrarFilaDesdeId(QSharedPointer<ModeloEntradas> modelo) {
	for (int i = 0; i < modelo->rowCount(); i++) {
		if (modelo->data(modelo->index(i, 0)).toUInt() == _id) {
			return i;
		}
	}

	return -1;
}
