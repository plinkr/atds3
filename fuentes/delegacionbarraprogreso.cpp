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
	qApp->style()->drawControl(QStyle::CE_ProgressBar, &elemento, pintor);
#endif
#ifdef Q_OS_WIN
	QStyleOptionViewItem o = opcion;

	o.displayAlignment = Qt::AlignCenter;

	drawBackground(pintor, o, indice);
/*
	if (opcion.state & QStyle::State_MouseOver) {
		pintor->fillRect(o.rect, QBrush(o.palette.highlight().color().darker()));
	} else {
		drawBackground(pintor, o, indice);
	}
*/
	if (o.state & QStyle::State_HasFocus) {
		drawFocus(pintor, o, o.rect);
	}
	drawDisplay(pintor, o, o.rect, QString("%1%").arg(indice.data().toInt()));
#endif
}
