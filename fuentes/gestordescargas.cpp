#include "gestordescargas.hpp"
#include "modeloentradas.hpp"
#include "ventanaprincipal.hpp"
#include "main.hpp"
#include <QSettings>
#include <QSharedPointer>


GestorDescargas::GestorDescargas(VentanaPrincipal *padre)
	: QThread(padre), _padre(padre), _configuracionTotalDescargasParalelas(0), _totalDescargasActivas(0), _deteniendoDescargas(false) {
}

void GestorDescargas::run() {
	QSettings configuracion;

	while (isInterruptionRequested() == false) {
		if (_deteniendoDescargas == false) {
			_configuracionTotalDescargasParalelas = configuracion.value("descargas/descargasParalelas").toInt();
			iniciarProximaDescargaDisponible();
		}
		sleep(2);
	}
}

void GestorDescargas::agregarDescarga(int fila, unsigned int id, QSharedPointer<ModeloEntradas> modelo, QSharedPointer<ModeloEntradas> modeloDescargando) {
	_mutexDescargasEnCola.lockForWrite();
	if (_descargasEnCola.find(id) == _descargasEnCola.end()) {
		_descargaEnCola descargaEnCola {fila, modelo, modeloDescargando};

		modelo->setData(modelo->index(fila, 1), _ListadoEstados::EnEsperaIniciar);
		_descargasEnCola[id] = std::move(descargaEnCola);
	}
	_mutexDescargasEnCola.unlock();
}

void GestorDescargas::detenerDescarga(unsigned int id) {
	_mutexTotalDescargasActivas.lockForWrite();
	if (_descargasActivas.find(id) != _descargasActivas.end()) {
		_descargasActivas[id]->detener();
		_descargasActivas.remove(id);
	}
	_mutexTotalDescargasActivas.unlock();

	_mutexDescargasEnCola.lockForWrite();
	if (_descargasEnCola.find(id) != _descargasEnCola.end()) {
		_descargasEnCola[id].modelo->setData(_descargasEnCola[id].modelo->index(_descargasEnCola[id].fila, 1), _ListadoEstados::Pausada);
		_descargasEnCola.remove(id);
	}
	_mutexDescargasEnCola.unlock();
}

void GestorDescargas::detenerDescargas() {
	_deteniendoDescargas = true;

	for (auto it = _descargasActivas.begin(); it != _descargasActivas.end(); it++) {
		it.value()->detener();
	}
	_mutexTotalDescargasActivas.lockForWrite();
	_descargasActivas.clear();
	_mutexTotalDescargasActivas.unlock();

	for (auto it = _descargasEnCola.begin(); it != _descargasEnCola.end(); it++) {
		_descargasEnCola[it.key()].modelo->setData(_descargasEnCola[it.key()].modelo->index(_descargasEnCola[it.key()].fila, 1), _ListadoEstados::Pausada);
	}
	_mutexDescargasEnCola.lockForWrite();
	_descargasEnCola.clear();
	_mutexDescargasEnCola.unlock();

	_deteniendoDescargas = false;
}

void GestorDescargas::procesarTerminacionDescarga(unsigned int id) {
	QSettings configuracion;

	_mutexTotalDescargasActivas.lockForWrite();
	if (_descargasActivas.find(id) != _descargasActivas.end()) {
		if (_descargasActivas[id]->error() == false) {
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
				_descargasActivas[id]->modelo()->select();
				_descargasActivas[id]->modeloDescargas()->select();
			}

			_descargasActivas.remove(id);
		}
	}
	_mutexTotalDescargasActivas.unlock();
}

void GestorDescargas::iniciarProximaDescargaDisponible() {
	_mutexDescargasEnCola.lockForWrite();
	_mutexTotalDescargasActivas.lockForWrite();

	if (_descargasActivas.size() < _configuracionTotalDescargasParalelas && _toDus->obtenerEstado() == toDus::Estado::Listo && _descargasEnCola.size() > 0) {
		for (auto it = _descargasEnCola.begin(); it != _descargasEnCola.end();) {
			if (_descargasActivas.find(it.key()) == _descargasActivas.end()) {
				_descargasActivas[it.key()] = new Descarga(it.key(), it.value().modelo, it.value().modeloDescargando, this);
				connect(_descargasActivas[it.key()], &Descarga::terminada, this, &GestorDescargas::procesarTerminacionDescarga);

				_descargasActivas[it.key()]->iniciar();

				it = _descargasEnCola.erase(it);

				if (_descargasActivas.size() == _configuracionTotalDescargasParalelas) {
					break;
				}
			}
		}
	}

	_mutexTotalDescargasActivas.unlock();
	_mutexDescargasEnCola.unlock();
}
