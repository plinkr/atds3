#include "descarga.hpp"
#include "modeloentradas.hpp"
#include "ventanaprincipal.hpp"
#include "main.hpp"
#include <QSettings>
#include <QRegExp>
#include <QFileInfo>

#include <iostream>


Descarga::Descarga(unsigned int id, QSharedPointer<ModeloEntradas> modelo, QSharedPointer<ModeloEntradas> modeloDescargando, QObject *padre)
	:  QNetworkAccessManager(padre), _iniciado(false), _id(id), _filaModelo(0), _filaModeloDescargando(0), _modelo(modelo), _modeloDescargando(modeloDescargando), _ultimoTiempoRecepcion(0), _ultimoTamanoRecibido(0) {
	connect(this, &Descarga::finished, this, &Descarga::deleteLater);

	_filaModelo = encontrarFilaDesdeId(_modelo);
	_modelo->setData(_modelo->index(_filaModelo, 1), _ListadoEstados::EnEsperaIniciar);
	_modelo->submitAll();

	_modeloDescargando->select();
	_filaModeloDescargando = encontrarFilaDesdeId(_modeloDescargando);
}

void Descarga::descargaIniciada() {
	if (modelosValido() == false) {
		_filaModelo = encontrarFilaDesdeId(_modelo);
		_filaModeloDescargando = encontrarFilaDesdeId(_modeloDescargando);
	}

	_modelo->setData(_modelo->index(_filaModelo, 1), _ListadoEstados::Iniciada);
	_modeloDescargando->setData(_modeloDescargando->index(_filaModeloDescargando, 1), _ListadoEstados::Iniciada);
}

void Descarga::eventoRecepcionDatos() {
	while (_respuesta->bytesAvailable() > 0) {
		_archivo.write(_respuesta->readAll());
	}
}

void Descarga::progresoDescarga(qint64 recibidos, qint64 total) {
	std::time_t tiempoRecepcion = std::time(nullptr);

	if (tiempoRecepcion - _ultimoTiempoRecepcion >= 1) {
		if (modelosValido() == false) {
			_filaModelo = encontrarFilaDesdeId(_modelo);
			_filaModeloDescargando = encontrarFilaDesdeId(_modeloDescargando);
		}

		_modelo->setData(_modelo->index(_filaModelo, 3), (recibidos * 100) / total);
		_modelo->setData(_modelo->index(_filaModelo, 4), recibidos - _ultimoTamanoRecibido);
		_modelo->setData(_modelo->index(_filaModelo, 8), total);
		_modelo->setData(_modelo->index(_filaModelo, 9), recibidos);

		_modeloDescargando->setData(_modeloDescargando->index(_filaModeloDescargando, 3), (recibidos * 100) / total);
		_modeloDescargando->setData(_modeloDescargando->index(_filaModeloDescargando, 4), recibidos - _ultimoTamanoRecibido);
		_modeloDescargando->setData(_modeloDescargando->index(_filaModeloDescargando, 8), total);
		_modeloDescargando->setData(_modeloDescargando->index(_filaModeloDescargando, 9), recibidos);

		_ultimoTiempoRecepcion = tiempoRecepcion;
		_ultimoTamanoRecibido = recibidos;
	}
}

void Descarga::eventoError(QNetworkReply::NetworkError codigo) {
	switch (codigo) {
		case QNetworkReply::TimeoutError:
			iniciarDescarga();
			break;
		default:
			std::cout << "Descargar Error: " << codigo << std::endl;
			break;
	}
}

void Descarga::descargaTerminada() {
	_archivo.close();

	if (modelosValido() == false) {
		_filaModelo = encontrarFilaDesdeId(_modelo);
		_filaModeloDescargando = encontrarFilaDesdeId(_modeloDescargando);
	}

	_modelo->setData(_modelo->index(_filaModelo, 1), _ListadoEstados::Finalizada);
	_modelo->setData(_modelo->index(_filaModelo, 3), 100);
	_modelo->setData(_modelo->index(_filaModelo, 4), 0);
	_modelo->setData(_modelo->index(_filaModelo, 9), _modelo->index(_filaModelo, 8));
	_modelo->submitAll();

	_modeloDescargando->select();

	_iniciado = false;

	emit terminada(_id);
}

bool Descarga::iniciado() {
	return _iniciado;
}

void Descarga::iniciar() {
	_iniciado = true;

	if (modelosValido() == false) {
		_filaModelo = encontrarFilaDesdeId(_modelo);
		_filaModeloDescargando = encontrarFilaDesdeId(_modeloDescargando);
	}
	_enlaceNoFirmado = _modelo->data(_modelo->index(_filaModelo, 7)).toString();

	connect(_toDus.get(), &toDus::enlaceFirmadoObtenido, this, &Descarga::procesarRespuestaDesdeTodus);
	_toDus->solicitarEnlaceFirmado(_enlaceNoFirmado);
}

void Descarga::procesarRespuestaDesdeTodus(const QString &noFirmado, const QString &firmado) {
	if (noFirmado == _enlaceNoFirmado) {
		QString enlaceProcesado = firmado;

		enlaceProcesado.replace("&amp;", "&");
		_enlaceFirmado = enlaceProcesado;

		disconnect(_toDus.get(), &toDus::enlaceFirmadoObtenido, this, &Descarga::procesarRespuestaDesdeTodus);
		iniciarDescarga();
	}
}

void Descarga::iniciarDescarga() {
	QSettings configuracion;
	QString ipServidorS3 = configuracion.value("todus/ipServidorS3", "").toString();
	QString nombreDNSServidorS3 = configuracion.value("todus/nombreDNSServidorS3", "s3.todus.cu").toString();
	int puertoServidorS3 = configuracion.value("todus/puertoServidorS3", 443).toInt();
	QUrl url = QUrl(_enlaceFirmado);
	QNetworkRequest solicitud;
	QSslConfiguration configuracionSSL = QSslConfiguration::defaultConfiguration();
	QString autorizacion = "Bearer " + configuracion.value("todus/fichaAcceso").toString();
	qint64 tamanoArchivoDestino;

	if (modelosValido() == false) {
		_filaModelo = encontrarFilaDesdeId(_modelo);
		_filaModeloDescargando = encontrarFilaDesdeId(_modeloDescargando);
	}

	QString rutaArchivo = _modelo->data(_modelo->index(_filaModelo, 5)).toString() + "/" + _modelo->data(_modelo->index(_filaModelo, 2)).toString();
	QFileInfo informacionArchivoDestino(rutaArchivo);
	if (informacionArchivoDestino.exists() == true) {
		tamanoArchivoDestino = informacionArchivoDestino.size();
	}

	configuracionSSL.setPeerVerifyMode(QSslSocket::VerifyNone);
	solicitud.setSslConfiguration(configuracionSSL);

	if (ipServidorS3.size() > 0) {
		url.setHost(ipServidorS3);
	} else {
		url.setHost(nombreDNSServidorS3);
	}
	url.setPort(puertoServidorS3);

	solicitud.setUrl(url);
	solicitud.setHeader(QNetworkRequest::UserAgentHeader, configuracion.value("todus/agente", "ToDus 0.38.35").toString() + " HTTP-Download");
	solicitud.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
	solicitud.setRawHeader("Authorization", autorizacion.toLocal8Bit());
	solicitud.setRawHeader("Host", nombreDNSServidorS3.toLocal8Bit());

	_archivo.setFileName(rutaArchivo);
	_archivo.open(QIODevice::WriteOnly);

	_ultimoTiempoRecepcion = std::time(nullptr);

	_respuesta = get(solicitud);
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
	if (_modeloDescargando->data(_modeloDescargando->index(_filaModeloDescargando, 0)).toUInt() != _id) {
		return false;
	}

	return true;
}

unsigned int Descarga::encontrarFilaDesdeId(QSharedPointer<ModeloEntradas> modelo) {
	for (unsigned int i = 0; modelo->rowCount(); i++) {
		if (modelo->data(modelo->index(i, 0)).toUInt() == _id) {
			return i;
		}
	}

	return -1;
}
