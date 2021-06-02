#include "gestordescargas.hpp"
#include "main.hpp"
#include "descarga.hpp"
#include "modeloentradas.hpp"
#include "ventanaprincipal.hpp"
#include <QSettings>
#include <QSharedPointer>
#include <QSqlQuery>
#include <QTreeView>
#include <QStandardPaths>
#include <QMap>


GestorDescargas::GestorDescargas(VentanaPrincipal *padre)
	: QThread(padre), _deteniendoDescargas(false), _padre(padre), _configuracionTotalDescargasParalelas(0), _totalDescargasActivas(0) {
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

	detenerDescargas();
}

void GestorDescargas::agregarDescarga(int fila, unsigned int id, QSharedPointer<ModeloEntradas> modelo, QSharedPointer<ModeloEntradas> modeloDescargando) {
	_mutexDescargasEnCola.lock();
	if (_descargasEnCola.find(id) == _descargasEnCola.end()) {
		_descargaEnCola descargaEnCola {fila, id, modelo, modeloDescargando};
		_descargasEnCola[id] = std::move(descargaEnCola);
	}
	_mutexDescargasEnCola.unlock();
}

void GestorDescargas::detenerDescarga(unsigned int id) {
	if (_descargasActivas.find(id) != _descargasActivas.end()) {
		QTimer::singleShot(0, _descargasActivas[id], &Descarga::detener);
		_descargasActivas.remove(id);
	}

	_mutexDescargasEnCola.lock();
	if (_descargasEnCola.find(id) != _descargasEnCola.end()) {
		_descargasEnCola.remove(id);
	}
	_mutexDescargasEnCola.unlock();
}

void GestorDescargas::detenerDescargas() {
	_deteniendoDescargas = true;

	for (auto it = _descargasActivas.begin(); it != _descargasActivas.end(); it++) {
		QTimer::singleShot(0, it.value(), &Descarga::detener);
	}
	_descargasActivas.clear();

	_mutexDescargasEnCola.lock();
	/*for (auto it = _descargasEnCola.begin(); it != _descargasEnCola.end(); it++) {
		_descargasEnCola[it.key()].modelo->setData(_descargasEnCola[it.key()].modelo->index(_descargasEnCola[it.key()].fila, 1), _ListadoEstados::Pausada);
	}*/
	_descargasEnCola.clear();
	_mutexDescargasEnCola.unlock();

	_deteniendoDescargas = false;
}

void GestorDescargas::procesarTerminacionDescarga(unsigned int id) {
	if (_descargasActivas.find(id) != _descargasActivas.end()) {
		if (_descargasActivas[id]->error() == false) {
			/*if (_configuracion.value("descargas/descomprimirAlFinalizar").toBool() == true) {
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
			if (_configuracion.value("descargas/eliminarAlFinalizar").toBool() == true) {
				_descargasActivas[id]->modelo()->eliminarFila(id);
				_descargasActivas[id]->modeloDescargas()->eliminarFila(id);
			}
		}

		_descargasActivas[id]->modeloDescargas()->submitAll();
		_descargasActivas[id]->modelo()->select();

		_descargasActivas.remove(id);
	}
}

void GestorDescargas::iniciarProximaDescargaDisponible() {
	_mutexDescargasEnCola.lock();

	if (_descargasActivas.size() < _configuracionTotalDescargasParalelas && _toDus->obtenerEstado() == toDus::Estado::Listo && _descargasEnCola.size() > 0) {
		for (auto it = _descargasEnCola.begin(); it != _descargasEnCola.end();) {
			if (_descargasActivas.find(it.key()) == _descargasActivas.end()) {
				_descargasActivas[it.key()] = new Descarga(it.key(), it.value().modelo, it.value().modeloDescargando, this);
				connect(_descargasActivas[it.key()], &Descarga::terminada, this, &GestorDescargas::procesarTerminacionDescarga);

				QTimer::singleShot(0, _descargasActivas[it.key()], &Descarga::iniciar);

				it = _descargasEnCola.erase(it);

				if (_descargasActivas.size() == _configuracionTotalDescargasParalelas) {
					break;
				}
			}
		}
	}

	_mutexDescargasEnCola.unlock();
}

int GestorDescargas::totalDescargas() {
	return _descargasActivas.size() + _descargasEnCola.size();
}
