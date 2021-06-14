#include "descarga.hpp"
#include "http1.hpp"
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
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDir>


Descarga::Descarga(unsigned int id, QSharedPointer<ModeloEntradas> modelo, QSharedPointer<ModeloEntradas> modeloDescargas, GestorDescargas *padre)
	: QObject(), _padre(padre), _iniciado(false), _deteniendo(false), _error(false), _id(id), _filaModelo(0), _filaModeloDescargas(0), _modelo(modelo), _modeloDescargas(modeloDescargas), _bytesRecibidos(0), _bytesTotal(0), _ultimoTiempoRecepcion(0), _ultimoTamanoRegistrado(0), _temporizadorMonitorizacionDescarga(nullptr) {
	moveToThread(padre->thread());
}

void Descarga::eventoDescargaIniciada() {
	if (_deteniendo == true || _iniciado == false) {
		if (_archivo.isOpen() == true) {
			_archivo.close();
		}

		if (_http.isNull() == false) {
			_http->abort();
		}
		return;
	}

	corregirFilaModelos();

	_modelo->setData(_modelo->index(_filaModelo, 2), _ListadoEstados::Iniciada);
	_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 2), _ListadoEstados::Iniciada);

	_modeloDescargas->submitAll();
}

void Descarga::eventoRecepcionDatos(std::uint32_t total) {
	if (_deteniendo == true || _iniciado == false || _archivo.isOpen() == false) {
		if (_http.isNull() == false) {
			_http->abort();
		}
		return;
	}
	_bytesRecibidos += total;
}

void Descarga::eventoProgresoDescarga(qint64 recibidos, qint64 total) {
	Q_UNUSED(recibidos);
	Q_UNUSED(total);
/*
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

		_modelo->setData(_modelo->index(_filaModelo, 4), velocidad);
		_modelo->setData(_modelo->index(_filaModelo, 8), total);
		_modelo->setData(_modelo->index(_filaModelo, 9), recibidos);

		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 4), velocidad);
		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 8), total);
		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 9), recibidos);

		_ultimoTiempoRecepcion = tiempoRecepcion;
		_ultimoTamanoRecibido = recibidos;
	}
*/
}

void Descarga::eventoMonitorizarDescarga() {
	std::time_t tiempoRecepcion = std::time(nullptr);
	unsigned int completado = 0;
	unsigned int velocidad = 0;

	if (_deteniendo == true || _iniciado == false) {
		_temporizadorMonitorizacionDescarga->stop();
		return;
	}

	corregirFilaModelos();

	if (_bytesTotal == 0 && _http.isNull() == false) {
		_bytesTotal = _http->cabeceraRespuesta("content-length").toUInt();
		_modelo->setData(_modelo->index(_filaModelo, 6), _bytesTotal);
		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 6), _bytesTotal);
	}

	if (_bytesTotal > 0) {
		completado = (_bytesRecibidos * 100) / _bytesTotal;
		if (completado > 100) {
			completado = 100;
		}
		_modelo->setData(_modelo->index(_filaModelo, 6), _bytesTotal);
		_modelo->setData(_modelo->index(_filaModelo, 8), completado);
		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 6), _bytesTotal);
		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 8), completado);
	}

	velocidad = _bytesRecibidos - _ultimoTamanoRegistrado;
	_ultimoTamanoRegistrado = _bytesRecibidos;
	if (velocidad > 0) {
		_ultimoTiempoRecepcion = tiempoRecepcion;
	}

	_modelo->setData(_modelo->index(_filaModelo, 9), velocidad);
	_modelo->setData(_modelo->index(_filaModelo, 7), _bytesRecibidos);

	_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 9), velocidad);
	_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 7), _bytesRecibidos);

	if (tiempoRecepcion - _ultimoTiempoRecepcion > 10) {
		if (_http.isNull() == false) {
			_http->abort();
		}

		_modelo->setData(_modelo->index(_filaModelo, 2), _ListadoEstados::EnEsperaIniciar);
		_modelo->setData(_modelo->index(_filaModelo, 9), 0);

		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 2), _ListadoEstados::EnEsperaIniciar);
		_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 9), 0);

		if (_archivo.isOpen() == true) {
			_archivo.close();
		}

		_deteniendo = false;
		_iniciado = false;
		_error = true;

		iniciar();
	}
}

void Descarga::eventoDescargaTerminada() {
	if (_archivo.isOpen() == true) {
		_archivo.close();
	}

	_iniciado = false;
	_deteniendo = true;

	corregirFilaModelos();

	_modelo->setData(_modelo->index(_filaModelo, 9), 0);
	_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 9), 0);

	if (_http.isNull() == false) {
		switch (_http->error()) {
			case QAbstractSocket::UnknownSocketError:
				return;
			case QAbstractSocket::RemoteHostClosedError:
			case QAbstractSocket::ProxyConnectionClosedError:
				if (_http->codigoHTTP() == 200 || _http->codigoHTTP() == 206) {
					_error = false;

					_modelo->setData(_modelo->index(_filaModelo, 2), _ListadoEstados::Finalizada);
					_modelo->setData(_modelo->index(_filaModelo, 7), _modelo->data(_modelo->index(_filaModelo, 6)).toUInt());
					_modelo->setData(_modelo->index(_filaModelo, 8), 100);

					_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 2), _ListadoEstados::Finalizada);
					_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 7), _modeloDescargas->data(_modeloDescargas->index(_filaModeloDescargas, 6)).toUInt());
					_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 8), 100);
				} else if (_http->codigoHTTP() >= 400 && _http->codigoHTTP() <= 600) {
					_error = true;

					_modelo->setData(_modelo->index(_filaModelo, 2), _ListadoEstados::Error);
					_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 2), _ListadoEstados::Error);
				}
				break;
			default:
				_error = true;

				_modelo->setData(_modelo->index(_filaModelo, 2), _ListadoEstados::Error);
				_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 2), _ListadoEstados::Error);
				break;
		}
	}

	emit terminada(_id);
}

bool Descarga::iniciado() {
	return _iniciado;
}

void Descarga::iniciar() {
	if (_deteniendo == true || _iniciado == true) { return; }

	if (_toDus->obtenerEstado() == toDus::Estado::Listo) {
		_iniciado = true;
		_error = false;
		_ultimoTiempoRecepcion = std::time(nullptr);
		if (_temporizadorMonitorizacionDescarga.isNull() == true) {
			_temporizadorMonitorizacionDescarga = new QTimer(this);
			connect(_temporizadorMonitorizacionDescarga, &QTimer::timeout, this, &Descarga::eventoMonitorizarDescarga);
			_temporizadorMonitorizacionDescarga->start(1000);
		}

		corregirFilaModelos();

		_enlaceNoFirmado = _modelo->data(_modelo->index(_filaModelo, 3)).toString();
		_toDus->solicitarEnlaceFirmado(_enlaceNoFirmado, std::bind(&Descarga::eventoProcesarRespuestaDesdeTodus, this, std::placeholders::_1));
	} else {
		QTimer::singleShot(2000, this, &Descarga::iniciar);
	}
}

void Descarga::detener() {
	_deteniendo = true;

	_temporizadorMonitorizacionDescarga->stop();

	if (_iniciado == true) {
		_iniciado = false;
		if (_http.isNull() == false) {
			_http->abort();
		}
	}

	if (_http.isNull() == false) {
		switch (_http->error()) {
			case QAbstractSocket::RemoteHostClosedError:
			case QAbstractSocket::ProxyConnectionClosedError:
			case QAbstractSocket::UnknownSocketError:
				corregirFilaModelos();

				_modelo->setData(_modelo->index(_filaModelo, 2), _ListadoEstados::Pausada);
				_modelo->setData(_modelo->index(_filaModelo, 9), 0);

				_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 2), _ListadoEstados::Pausada);
				_modeloDescargas->setData(_modeloDescargas->index(_filaModeloDescargas, 9), 0);
				break;
			default:
				break;
		}
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

void Descarga::eventoProcesarRespuestaDesdeTodus(const QString &firmado) {
	QString enlaceProcesado = firmado;

	if (_deteniendo == true || _iniciado == false) { return; }

	enlaceProcesado.replace("&amp;", "&");
	_enlaceFirmado = enlaceProcesado;

	QTimer::singleShot(0, this, &Descarga::iniciarDescarga);
}

void Descarga::iniciarDescarga() {
	QSettings configuracion;
	QString ipServidorS3 = configuracion.value("avanzadas/ipServidorS3", "").toString();
	QString nombreDNSServidorS3 = configuracion.value("avanzadas/nombreDNSServidorS3", "s3.todus.cu").toString();
	int puertoServidorS3 = configuracion.value("avanzadas/puertoServidorS3", 443).toInt();
	QString rutaHTTP = _enlaceFirmado;
	QString autorizacion = "Bearer " + configuracion.value("todus/fichaAcceso").toString();
	QString nombreArchivo;
	QRegularExpression re;
	QRegularExpressionMatch rem;
	QDir directorioDescarga;
	QString rutaArchivo;
	QFileInfo informacionArchivo;

	if (_deteniendo == true || _iniciado == false) { return; }

	rutaHTTP.remove(0, 8 + nombreDNSServidorS3.size());
	_bytesRecibidos = 0;
	_bytesTotal = 0;
	_ultimoTamanoRegistrado = 0;
	_ultimoTiempoRecepcion = std::time(nullptr);

	corregirFilaModelos();

	rutaArchivo = _modelo->data(_modelo->index(_filaModelo, 5)).toString();
	nombreArchivo = _modelo->data(_modelo->index(_filaModelo, 4)).toString();
	re = QRegularExpression("^(.+)(\\.\\w{0,4}\\d{1,5})(\\.(zip|7z|rar|gz|xz|tar|tgz))$", QRegularExpression::CaseInsensitiveOption);
	rem = re.match(nombreArchivo);
	if (rem.hasMatch() == true) {
		rutaArchivo += "/" + rem.captured(1);
	}
	re = QRegularExpression("^(.+)(\\.(zip|7z|rar|gz|xz|tar|tgz))(\\.\\w{0,4}\\d{1,5})$", QRegularExpression::CaseInsensitiveOption);
	rem = re.match(nombreArchivo);
	if (rem.hasMatch() == true) {
		rutaArchivo += "/" + rem.captured(1);
	}
	if (directorioDescarga.exists(rutaArchivo) == false) {
		directorioDescarga.mkdir(rutaArchivo);
	}
	rutaArchivo += "/" + nombreArchivo;

	informacionArchivo.setFile(rutaArchivo);
	if (informacionArchivo.exists() == true) {
		_bytesTotal = _modelo->data(_modelo->index(_filaModelo, 6)).toUInt();
		_bytesRecibidos = informacionArchivo.size();
		_ultimoTamanoRegistrado = _bytesRecibidos;
	}

	_http = new HTTP1(this);

	_http->establecerMetodo(HTTP1::Metodo::OBTENER);
	if (ipServidorS3.size() > 0) {
		_http->establecerAnfitrion(ipServidorS3);
	} else {
		_http->establecerAnfitrion(nombreDNSServidorS3);
	}
	_http->establecerPuerto(puertoServidorS3);
	_http->establecerRuta(rutaHTTP);

	_archivo.setFileName(rutaArchivo);
	_archivo.open(QIODevice::Append);
	_http->establecerDestinoDescarga(&_archivo);

	_http->agregarCabeceraSolicitud("Accept", "*/*");
	_http->agregarCabeceraSolicitud("Authorization", autorizacion);
	_http->agregarCabeceraSolicitud("Content-Type", "application/octet-stream");
	_http->agregarCabeceraSolicitud("Host", nombreDNSServidorS3);
	_http->agregarCabeceraSolicitud("User-Agent", configuracion.value("avanzadas/agenteUsuario", _agenteUsuarioTodus).toString() + " HTTP-Download");
	if (_bytesRecibidos > 0) {
		_http->agregarCabeceraSolicitud("Range", "bytes=" + QString::number(_bytesRecibidos).toLocal8Bit() + "-");
	}

	connect(_http, &HTTP1::conectado, this, &Descarga::eventoDescargaIniciada);
	connect(_http, &HTTP1::bytesDescargados, this, &Descarga::eventoRecepcionDatos);
	connect(_http, &HTTP1::finalizado, this, &Descarga::eventoDescargaTerminada);

	_http->ejecutar();
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

	if (_http.isNull() == false) {
		_http->abort();
	}

	_deteniendo = true;
	_iniciado = false;
	_error = true;

	emit terminada(_id);

	return -1;
}
