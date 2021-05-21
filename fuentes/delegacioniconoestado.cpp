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
	}
/*
	if (opcion.state & QStyle::State_Selected) {
		pintor->fillRect(opcion.rect, opcion.palette.highlight());
	}

	qApp->style()->drawItemPixmap(pintor, opcion.rect, Qt::AlignCenter, icono.pixmap(QSize(24, 24)));
*/
	//pintor->drawPixmap(QRect(opcion.rect.center() - QPoint(12, 12), opcion.rect.center() + QPoint(12, 12)), icono.pixmap(QSize(24, 24)));
	drawBackground(pintor, opcion, indice);
	drawFocus(pintor, opcion, opcion.rect);
	drawDecoration(pintor, opcion, opcion.rect, icono.pixmap(QSize(24, 24)));
}
