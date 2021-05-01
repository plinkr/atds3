#include "gestordescargas.hpp"
#include "ventanaprincipal.hpp"
#include "main.hpp"
#include <QSettings>


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

void GestorDescargas::procesarTerminacionDescarga(unsigned int id) {
	if (_descargasActivas.find(id) != _descargasActivas.end()) {
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
