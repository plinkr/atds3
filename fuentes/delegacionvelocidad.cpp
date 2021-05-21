#include "delegacionvelocidad.hpp"
#include <QApplication>
#include <QPainter>


DelegacionVelocidad::DelegacionVelocidad(QObject *padre) : QItemDelegate(padre) {}

void DelegacionVelocidad::paint(QPainter *pintor, const QStyleOptionViewItem &opcion, const QModelIndex &indice) const {
	QVector<QString> listadoUnidadesVelocidad {"B/s", "KiB/s", "MiB/s", "GiB/s", "PiB/s"};
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

	if (opcion.state & QStyle::State_Selected) {
		pintor->fillRect(opcion.rect, opcion.palette.highlight());
	}

	//qApp->style()->drawItemText(pintor, opcion.rect, Qt::AlignCenter, opcion.palette, true, velocidadTexto);
	drawBackground(pintor, opcion, indice);
	drawFocus(pintor, opcion, opcion.rect);
	drawDisplay(pintor, opcion, opcion.rect, velocidadTexto);
}
