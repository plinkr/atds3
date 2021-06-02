#include "delegaciontamano.hpp"
#include <QApplication>
#include <QPainter>


DelegacionTamano::DelegacionTamano(bool velocidad, QObject *padre) : QItemDelegate(padre), _velocidad(velocidad) {}

void DelegacionTamano::paint(QPainter *pintor, const QStyleOptionViewItem &opcion, const QModelIndex &indice) const {
	QVector<QString> listadoUnidadesTamano {"B", "KiB", "MiB", "GiB", "TiB"};
	QVector<QString> listadoUnidadesVelocidad {"B/s", "KiB/s", "MiB/s", "GiB/s", "TiB/s"};
	int unidadTamano = 0;
	double tamano = indice.data().toDouble();
	QString representacion;
	QStyleOptionViewItem opcionVisualizacion = opcion;

	while (tamano > 1024) {
		tamano /= 1024;
		unidadTamano++;
	}

	if (unidadTamano < 2) {
		if (_velocidad == true) {
			opcionVisualizacion.displayAlignment = Qt::AlignVCenter;
			representacion = QString("%1 %2").arg(QString::number((unsigned int)tamano), listadoUnidadesVelocidad[unidadTamano]);
		} else {
			opcionVisualizacion.displayAlignment = Qt::AlignCenter;
			representacion = QString("%1 %2").arg(QString::number((unsigned int)tamano), listadoUnidadesTamano[unidadTamano]);
		}
	} else {
		if (_velocidad == true) {
			opcionVisualizacion.displayAlignment = Qt::AlignVCenter;
			representacion = QString("%1 %2").arg(QString::number(tamano, 'g', 3), listadoUnidadesVelocidad[unidadTamano]);
		} else {
			opcionVisualizacion.displayAlignment = Qt::AlignCenter;
			representacion = QString("%1 %2").arg(QString::number(tamano, 'g', 3), listadoUnidadesTamano[unidadTamano]);
		}
	}

	drawBackground(pintor, opcionVisualizacion, indice);
	if (opcion.state & QStyle::State_HasFocus) {
		drawFocus(pintor, opcionVisualizacion, opcionVisualizacion.rect);
	}
	drawDisplay(pintor, opcionVisualizacion, opcionVisualizacion.rect, representacion);
}
