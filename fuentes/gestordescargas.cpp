#include "gestordescargas.hpp"
#include "ventanaprincipal.hpp"
#include "main.hpp"


GestorDescargas::GestorDescargas(VentanaPrincipal *padre) : QThread(padre) {
	connect(padre, &VentanaPrincipal::categoriaSeleccionada, this, &GestorDescargas::eventoCategoriaSeleccionada);
}

void GestorDescargas::eventoCategoriaSeleccionada(QSharedPointer<ModeloEntradas> modeloActivo) {
	_mutexModeloActivo.lock();
	_modeloActivo = modeloActivo;
	_mutexModeloActivo.unlock();
}

void GestorDescargas::run() {
	while (isInterruptionRequested() == false) {
		_mutexModeloActivo.lock();

		_mutexModeloActivo.unlock();

		sleep(2);
	}
}
