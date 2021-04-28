#include "delegacionvelocidad.hpp"
#include <QApplication>
#include <QItemDelegate>


DelegacionVelocidad::DelegacionVelocidad(QObject *padre) : QItemDelegate(padre) {}

void DelegacionVelocidad::paint(QPainter *pintor, const QStyleOptionViewItem &opcion, const QModelIndex &indice) const {
	QVector<QString> listadoUnidadesVelocidad {"B/s", "KiB/s", "MiB/s", "GiB/s"};
	int unidadVelocidad = 0;
	double velocidad = indice.data().toDouble();
	QString velocidadTexto = QString("%1 %2").arg(velocidad).arg(listadoUnidadesVelocidad[unidadVelocidad++]);

	while (velocidad > 1000) {
		velocidad /= 1000;
		velocidadTexto = QString("%1 %2").arg(velocidad).arg(listadoUnidadesVelocidad[unidadVelocidad++]);
	}

	drawBackground(pintor, opcion, indice);
	drawDisplay(pintor, opcion, opcion.rect, velocidadTexto);
}
