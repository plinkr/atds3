#include "descarga.hpp"
#include "main.hpp"
#include "gestordescargas.hpp"
#include "modeloentradas.hpp"
#include "ventanaprincipal.hpp"
#include <type_traits>
#include <functional>
#include <QSettings>
#include <QFileInfo>
#include <QTimer>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QApplication>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <iostream>


Descarga::Descarga(unsigned int id, QSharedPointer<ModeloEntradas> modelo, QSharedPointer<ModeloEntradas> modeloDescargas, GestorDescargas *padre)
	: QObject(), _padre(padre), _baseDatos(NULL), _iniciado(false), _deteniendo(false), _error(false), _id(id), _filaModelo(0), _filaModeloDescargas(0), _modelo(modelo), _modeloDescargas(modeloDescargas), _respuesta(nullptr), _ultimoTiempoRecepcion(0), _ultimoTamanoRecibido(0), _tamanoPrevio(0) {
	moveToThread(padre->thread());
}
Descarga::~Descarga() {
	cerrarConexionBaseDatos(_baseDatos);
}

void Descarga::descargaIniciada() {
	if (_deteniendo == true) {
		_respuesta->abort();
		return;
	}

	corregirFilaModelos();

	baseDatosEjecutar(_baseDatos, "UPDATE entradas SET estado = " + QString::number(_ListadoEstados::Iniciada) + " WHERE (id = " + QString::number(_id) + ")");

	_modelo->setData(_modelo->index(_filaModelo, 1), _ListadoEstados::Iniciada);
	_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 1), _ListadoEstados::Iniciada);

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
		unsigned int completado = 0;
		unsigned int velocidad = 0;

		corregirFilaModelos();

		recibidos += _tamanoPrevio;
		total += _tamanoPrevio;

		if (total > 0) {
			completado = (recibidos * 100) / total;

			_modelo->setData(_modelo->index(_filaModelo, 3), completado);
			_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 3), completado);
		}
		velocidad = recibidos - _ultimoTamanoRecibido;

		baseDatosEjecutar(_baseDatos, "UPDATE entradas SET completado = " + QString::number(completado) + ", velocidad = " + QString::number(velocidad) + ", totalADescargar = " + QString::number(total) + ", totalDescargado = " + QString::number(recibidos) + " WHERE (id = " + QString::number(_id) + ")");

		_modelo->setData(_modelo->index(_filaModelo, 4), velocidad);
		_modelo->setData(_modelo->index(_filaModelo, 8), total);
		_modelo->setData(_modelo->index(_filaModelo, 9), recibidos);

		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 4), velocidad);
		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 8), total);
		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 9), recibidos);

		_ultimoTiempoRecepcion = tiempoRecepcion;
		_ultimoTamanoRecibido = recibidos;
	}
}

void Descarga::eventoError(QNetworkReply::NetworkError codigo) {
	_archivo.close();

	switch (codigo) {
		case QNetworkReply::NoError:
			break;
		case QNetworkReply::OperationCanceledError:
			_error = true;
			break;
		case QNetworkReply::RemoteHostClosedError:
		case QNetworkReply::HostNotFoundError:
		case QNetworkReply::TemporaryNetworkFailureError:
		case QNetworkReply::TimeoutError:
		case QNetworkReply::ProxyTimeoutError:
		{
			_iniciado = false;
			_error = true;

			if (_deteniendo == true) { return; }

			corregirFilaModelos();

			baseDatosEjecutar(_baseDatos, "UPDATE entradas SET estado = " + QString::number(_ListadoEstados::EnEsperaIniciar) + ", velocidad = 0 WHERE (id = " + QString::number(_id) + ")");

			_modelo->setData(_modelo->index(_filaModelo, 1), _ListadoEstados::EnEsperaIniciar);
			_modelo->setData(_modelo->index(_filaModelo, 4), 0);

			_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 1), _ListadoEstados::EnEsperaIniciar);
			_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 4), 0);

			QTimer::singleShot(2000, this, &Descarga::iniciar);
			break;
		}
		default:
			baseDatosEjecutar(_baseDatos, "UPDATE entradas SET estado = " + QString::number(_ListadoEstados::Error) + ", velocidad = 0 WHERE (id = " + QString::number(_id) + ")");

			corregirFilaModelos();

			_modelo->setData(_modelo->index(_filaModelo, 1), _ListadoEstados::Error);
			_modelo->setData(_modelo->index(_filaModelo, 4), 0);

			_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 1), _ListadoEstados::Error);
			_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 4), 0);

			_iniciado = false;
			_error = true;

			emit terminada(_id);

			break;
	}
}

void Descarga::descargaTerminada() {
	_archivo.close();

	corregirFilaModelos();

	_modelo->setData(_modelo->index(_filaModelo, 4), 0);

	if (_error == false) {
		baseDatosEjecutar(_baseDatos, "UPDATE entradas SET estado = " + QString::number(_ListadoEstados::Finalizada) + ", completado = 100, velocidad = 0, totalDescargado = totalADescargar WHERE (id = " + QString::number(_id) + ")");
		_modelo->setData(_modelo->index(_filaModelo, 1), _ListadoEstados::Finalizada);
		_modelo->setData(_modelo->index(_filaModelo, 3), 100);
		_modelo->setData(_modelo->index(_filaModelo, 9), _modelo->index(_filaModelo, 8));

		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 1), _ListadoEstados::Finalizada);
		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 3), 100);
		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 9), _modeloDescargas->index(_filaModeloDescargas, 8));

		_iniciado = false;

		emit terminada(_id);
	}
}

bool Descarga::iniciado() {
	return _iniciado;
}

void Descarga::iniciar() {
	QSharedPointer<toDus> todus = _toDus;

	if (_deteniendo == true) { return; }

	if (todus->obtenerEstado() == toDus::Estado::Listo) {
		corregirFilaModelos();

		_enlaceNoFirmado = _modelo->data(_modelo->index(_filaModelo, 7)).toString();

		if (_iniciado == false) {
			todus->solicitarEnlaceFirmado(_enlaceNoFirmado, std::bind(&Descarga::procesarRespuestaDesdeTodus, this, std::placeholders::_1));
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

	if (_error == false) {
		corregirFilaModelos();

		baseDatosEjecutar(_baseDatos, "UPDATE entradas SET estado = " + QString::number(_ListadoEstados::Pausada) + ", velocidad = 0 WHERE (id = " + QString::number(_id) + ")");

		_modelo->setData(_modelo->index(_filaModelo, 1), _ListadoEstados::Pausada);
		_modelo->setData(_modelo->index(_filaModelo, 4), 0);

		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 1), _ListadoEstados::Pausada);
		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 4), 0);
	}
}

unsigned int Descarga::id() {
	return _id;
}

int Descarga::fila() {
	return _filaModelo;
}

int Descarga::filaDescargas() {
	return _filaModeloDescargas;
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

	_baseDatos = iniciarConexionBaseDatos();

	_tamanoPrevio = 0;

	corregirFilaModelos();

	QString rutaArchivo = _modelo->data(_modelo->index(_filaModelo, 5)).toString() + "/" + _modelo->data(_modelo->index(_filaModelo, 2)).toString();

	informacionArchivo.setFile(rutaArchivo);
	if (informacionArchivo.exists() == true) {
		_tamanoPrevio = informacionArchivo.size();
	}

#if QT_VERSION >= 0x050e00
	solicitud.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
	solicitud.setTransferTimeout(10000);
#endif
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
#if QT_VERSION >= 0x050e00
	_administradorAccesoRed->setAutoDeleteReplies(true);
#endif
	_administradorAccesoRed->setProxy(obtenerConfiguracionProxy());
	_respuesta = _administradorAccesoRed->get(solicitud);
	connect(_respuesta, &QNetworkReply::encrypted, this, &Descarga::descargaIniciada);
	connect(_respuesta, &QIODevice::readyRead, this, &Descarga::eventoRecepcionDatos);
	connect(_respuesta, &QNetworkReply::downloadProgress, this, &Descarga::progresoDescarga);
#if QT_VERSION >= 0x050e00
	connect(_respuesta, &QNetworkReply::errorOccurred, this, &Descarga::eventoError);
#else
	connect(_respuesta, SIGNAL(errorOccurred), this, SLOT(eventoError));
#endif
	connect(_respuesta, &QNetworkReply::finished, this, &Descarga::descargaTerminada);
}

void Descarga::corregirFilaModelos() {
	if (_modelo->data(_modelo->index(_filaModelo, 0)).toUInt() != _id) {
		_filaModelo = encontrarFilaDesdeId(_modelo);
	}
	if (_modeloDescargas->data(_modeloDescargas->index(_filaModeloDescargas, 0)).toUInt() != _id) {
		_filaModeloDescargas = encontrarFilaDesdeId(_modeloDescargas);
	}
}

unsigned int Descarga::encontrarFilaDesdeId(QSharedPointer<ModeloEntradas> modelo) {
	for (int i = 0; i < modelo->rowCount(); i++) {
		if (modelo->data(modelo->index(i, 0)).toUInt() == _id) {
			return i;
		}
	}

	return -1;
}
