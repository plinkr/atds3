#include "gestordescargas.hpp"
#include "modeloentradas.hpp"
#include "ventanaprincipal.hpp"
#include "main.hpp"
#include <QSettings>
#include <QSharedPointer>


GestorDescargas::GestorDescargas(QObject *padre)
	: QThread(padre), _configuracionTotalDescargasParalelas(0), _totalDescargasActivas(0) {
}

void GestorDescargas::run() {
	QSettings configuracion;

	while (isInterruptionRequested() == false) {
		_configuracionTotalDescargasParalelas = configuracion.value("descargas/descargasParalelas").toInt();
		iniciarProximaDescargaDisponible();
		sleep(1);
	}
}

void GestorDescargas::agregarDescarga(unsigned int id, QSharedPointer<ModeloEntradas> modelo, QSharedPointer<ModeloEntradas> modeloDescargando) {
	if (_descargasActivas.find(id) == _descargasActivas.end()) {
		_descargasActivas[id] = new Descarga(id, modelo, modeloDescargando, this);
		connect(_descargasActivas[id], &Descarga::terminada, this, &GestorDescargas::procesarTerminacionDescarga);

		if (_totalDescargasActivas < _configuracionTotalDescargasParalelas && _toDus->obtenerEstado() == toDus::Estado::Listo) {
			_descargasActivas[id]->iniciar();
			_totalDescargasActivas++;
		}
	}
}

void GestorDescargas::detenerDescarga(unsigned int id) {
	if (_descargasActivas.find(id) != _descargasActivas.end()) {
		bool iniciado = _descargasActivas[id]->iniciado();

		_descargasActivas[id]->detener();

		if (iniciado == true) {
			_totalDescargasActivas--;
		}

		_descargasActivas.remove(id);
	}
}

void GestorDescargas::procesarTerminacionDescarga(unsigned int id) {
	QSettings configuracion;

	if (_descargasActivas.find(id) != _descargasActivas.end()) {
		/*if (configuracion.value("descargas/descomprimirAlFinalizar").toBool() == true) {
			QString nombreArchivoOriginal = _descargasActivas[id]->modelo()->data(_descargasActivas[id]->modelo()->index(_descargasActivas[id]->fila(), 2)).toString();

			if (nombreArchivoOriginal.indexOf(".rar") == true) {
				if (nombreArchivoOriginal.indexOf(".part") == true) {
					nombreArchivoOriginal.remove(nombreArchivoOriginal.lastIndexOf("."));
				}
			} else {

			}

			for (int f = 0; f < _descargasActivas[id]->modelo()->rowCount(); f++) {
				if ()
			}
		}*/
		if (configuracion.value("descargas/eliminarAlFinalizar").toBool() == true) {
			_descargasActivas[id]->modelo()->removeRow(_descargasActivas[id]->fila());
			_descargasActivas[id]->modelo()->submitAll();
		}

		_totalDescargasActivas--;
		_descargasActivas.remove(id);

		iniciarProximaDescargaDisponible();
	}
}

void GestorDescargas::iniciarProximaDescargaDisponible() {
	if (_totalDescargasActivas < _configuracionTotalDescargasParalelas && _toDus->obtenerEstado() == toDus::Estado::Listo) {
		for (auto d : _descargasActivas.values()) {
			if (d->iniciado() == false) {
				_totalDescargasActivas++;
				d->iniciar();
				break;
			}
		}
	}
}
