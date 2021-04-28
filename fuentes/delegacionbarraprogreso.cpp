#include "delegacionbarraprogreso.hpp"
#include <QApplication>
#include <QItemDelegate>


DelegacionBarraProgreso::DelegacionBarraProgreso(QObject *padre) : QItemDelegate(padre) {}

void DelegacionBarraProgreso::paint(QPainter *pintor, const QStyleOptionViewItem &opcion, const QModelIndex &indice) const {
	QStyleOptionProgressBar elemento;

	elemento.fontMetrics = opcion.fontMetrics;
	elemento.minimum = 0;
	elemento.maximum = 100;
	elemento.palette = opcion.palette;
	elemento.progress = indice.data().toInt();
	elemento.rect = opcion.rect;
	elemento.text = QString("%1%").arg(elemento.progress);
	elemento.textAlignment = Qt::AlignHCenter;
	elemento.textVisible = true;
	elemento.version = opcion.version;

	drawBackground(pintor, opcion, indice);
	QApplication::style()->drawControl(QStyle::CE_ProgressBar, &elemento, pintor, 0);
}
