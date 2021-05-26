#include "delegacionbarraprogreso.hpp"
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QPen>


DelegacionBarraProgreso::DelegacionBarraProgreso(QObject *padre) : QItemDelegate(padre) {}

void DelegacionBarraProgreso::paint(QPainter *pintor, const QStyleOptionViewItem &opcion, const QModelIndex &indice) const {
#ifdef Q_OS_UNIX
	QStyleOptionProgressBar elemento;

	elemento.bottomToTop = false;
	elemento.minimum = 0;
	elemento.maximum = 100;
	elemento.progress = indice.data().toInt();
	elemento.rect = opcion.rect;
	elemento.text = QString("%1%").arg(elemento.progress);
	elemento.textAlignment = Qt::AlignCenter;
	elemento.textVisible = true;

	drawBackground(pintor, opcion, indice);
	if ((opcion.state & QStyle::State_HasFocus) == QStyle::State_HasFocus) {
		drawFocus(pintor, opcion, opcion.rect);
	}
	qApp->style()->drawControl(QStyle::CE_ProgressBar, &elemento, pintor);
#endif
#ifdef Q_OS_WIN
	QRect r = opcion.rect;

	r.adjust(1.5, 1.5, -1.5, -1.5);
	r.setWidth(((r.width() * indice.data().toInt()) / 100));

	drawBackground(pintor, opcion, indice);
	if ((opcion.state & QStyle::State_HasFocus) == QStyle::State_HasFocus) {
		drawFocus(pintor, opcion, opcion.rect);
	}

	pintor->save();
	if (indice.data().toInt() > 0) {
		pintor->setRenderHint(QPainter::Antialiasing, true);

		if ((opcion.state & QStyle::State_Selected) == QStyle::State_Selected) {
			pintor->setPen(QPen(opcion.palette.highlight(), 1));
			pintor->setBrush(QBrush(opcion.palette.highlight().color().lighter()));
		} else {
			pintor->setPen(QPen(QBrush(opcion.palette.highlight().color().darker()), 1));
			pintor->setBrush(opcion.palette.highlight());
		}

		pintor->drawRoundedRect(r, 2, 2);
	}
	pintor->setBrush(Qt::NoBrush);
	if ((opcion.state & QStyle::State_Selected) == QStyle::State_Selected) {
		pintor->setPen(opcion.palette.highlightedText().color());
	} else {
		pintor->setPen(opcion.palette.text().color());
	}
	pintor->drawText(opcion.rect, Qt::AlignCenter, QString("%1%").arg(indice.data().toInt()));
	pintor->restore();
#endif
}
