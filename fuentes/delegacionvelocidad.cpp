#include "delegacionvelocidad.hpp"
#include <QApplication>
#include <QPainter>


DelegacionVelocidad::DelegacionVelocidad(QObject *padre) : QItemDelegate(padre) {}

void DelegacionVelocidad::paint(QPainter *pintor, const QStyleOptionViewItem &opcion, const QModelIndex &indice) const {
	QVector<QString> listadoUnidadesVelocidad {"B/s", "KiB/s", "MiB/s", "GiB/s", "TiB/s"};
	int unidadVelocidad = 0;
	double velocidad = indice.data().toDouble();
	QString velocidadTexto;

	while (velocidad > 1024) {
		velocidad /= 1024;
		unidadVelocidad++;
	}

	if (unidadVelocidad < 2) {
		velocidadTexto = QString("%1 %2").arg(QString::number((unsigned int)velocidad), listadoUnidadesVelocidad[unidadVelocidad]);
	} else {
		velocidadTexto = QString("%1 %2").arg(QString::number(velocidad, 'g', 3), listadoUnidadesVelocidad[unidadVelocidad]);
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
	drawDisplay(pintor, opcion, opcion.rect, velocidadTexto);
}
