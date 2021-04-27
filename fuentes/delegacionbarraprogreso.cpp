#include "delegacionbarraprogreso.hpp"
#include <QApplication>
#include <QItemDelegate>


DelegacionBarraProgreso::DelegacionBarraProgreso(QObject *padre)
	: QItemDelegate(padre) {}

void DelegacionBarraProgreso::paint(QPainter *pintor, const QStyleOptionViewItem &opcion, const QModelIndex &indice) const {
	QStyleOptionProgressBar elemento;

	elemento.rect = opcion.rect;
	elemento.minimum = 0;
	elemento.maximum = 100;
	elemento.progress = indice.data().toInt();
	elemento.text = QString("%1%").arg(elemento.progress);
	elemento.textAlignment = Qt::AlignHCenter;
	elemento.textVisible = true;

	QApplication::style()->drawControl(QStyle::CE_ProgressBar, &elemento, pintor, 0);
}
