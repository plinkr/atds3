#include "delegacionbarraprogreso.hpp"
#include <QApplication>
#include <QPainter>


DelegacionBarraProgreso::DelegacionBarraProgreso(QObject *padre) : QItemDelegate(padre) {}

void DelegacionBarraProgreso::paint(QPainter *pintor, const QStyleOptionViewItem &opcion, const QModelIndex &indice) const {
#ifdef Q_OS_UNIX
	QStyleOptionProgressBar elemento;

	elemento.direction = qApp->layoutDirection();
	elemento.fontMetrics = qApp->fontMetrics();
	elemento.minimum = 0;
	elemento.maximum = 100;
	elemento.palette = opcion.palette;
	elemento.progress = indice.data().toInt();
	elemento.state = opcion.state;
	elemento.rect = opcion.rect;
	elemento.text = QString("%1%").arg(elemento.progress);
	elemento.textAlignment = Qt::AlignCenter;
	elemento.textVisible = true;

	qApp->style()->drawControl(QStyle::CE_ProgressBar, &elemento, pintor);
#endif
#ifdef Q_OS_WIN
/*
	if (opcion.state & QStyle::State_Selected) {
		pintor->fillRect(opcion.rect, opcion.palette.highlight());
	}

	qApp->style()->drawItemText(pintor, opcion.rect, Qt::AlignCenter, opcion.palette, true, QString("%1%").arg(indice.data().toInt()));
*/
	QStyleOptionViewItem o = opcion;

	o.displayAlignment = Qt::AlignCenter;

	drawBackground(pintor, o, indice);
	drawFocus(pintor, o, o.rect);
	drawDisplay(pintor, o, o.rect, QString("%1%").arg(indice.data().toInt()));
#endif
}
