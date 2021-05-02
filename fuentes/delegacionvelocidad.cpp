#include "delegacionvelocidad.hpp"
#include <QApplication>
#include <QItemDelegate>


DelegacionVelocidad::DelegacionVelocidad(QObject *padre) : QItemDelegate(padre) {}

void DelegacionVelocidad::paint(QPainter *pintor, const QStyleOptionViewItem &opcion, const QModelIndex &indice) const {
	QVector<QString> listadoUnidadesVelocidad {"B/s", "KiB/s", "MiB/s", "GiB/s"};
	int unidadVelocidad = 0;
	double velocidad = indice.data().toDouble();
	QString velocidadTexto;

	while (velocidad > 1000) {
		velocidad /= 1000;
		unidadVelocidad++;
	}

	if (unidadVelocidad < 2) {
		velocidadTexto = QString("%1 %2").arg(QString::number((unsigned int)velocidad), listadoUnidadesVelocidad[unidadVelocidad]);
	} else {
		velocidadTexto = QString("%1 %2").arg(QString::number(velocidad, 'g', 3), listadoUnidadesVelocidad[unidadVelocidad]);
	}

	drawBackground(pintor, opcion, indice);
	drawDisplay(pintor, opcion, opcion.rect, velocidadTexto);
}
