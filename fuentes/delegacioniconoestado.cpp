#include "delegacioniconoestado.hpp"
#include "main.hpp"
#include <QApplication>
#include <QIcon>
#include <QPainter>


DelegacionIconoEstado::DelegacionIconoEstado(QObject *padre) : QItemDelegate(padre) {}

void DelegacionIconoEstado::paint(QPainter *pintor, const QStyleOptionViewItem &opcion, const QModelIndex &indice) const {
	QIcon icono;

	switch (indice.data().toInt()) {
		case _ListadoEstados::EnEsperaPausar:
		case _ListadoEstados::Pausada:
			icono = QIcon(obtenerRutaIcono() + "pausar.svg");
			break;
		case _ListadoEstados::EnEsperaIniciar:
			icono = QIcon(obtenerRutaIcono() + "reloj.svg");
			break;
		case _ListadoEstados::Iniciada:
			icono = QIcon(obtenerRutaIcono() + "iniciar.svg");
			break;
		case _ListadoEstados::Finalizada:
			icono = QIcon(obtenerRutaIcono() + "finalizado.svg");
			break;
		case _ListadoEstados::Error:
			icono = QIcon(obtenerRutaIcono() + "cancelar.svg");
			break;
	}

	drawBackground(pintor, opcion, indice);
/*
	if (opcion.state & QStyle::State_MouseOver) {
		pintor->fillRect(opcion.rect, QBrush(opcion.palette.highlight().color().darker()));
	} else {
		drawBackground(pintor, opcion, indice);
	}
*/
	if (opcion.state & QStyle::State_HasFocus) {
		drawFocus(pintor, opcion, opcion.rect);
	}
	drawDecoration(pintor, opcion, opcion.rect, icono.pixmap(QSize(24, 24)));
}
