#ifndef DELEGACIONBARRAPROGRESO_HPP
#define DELEGACIONBARRAPROGRESO_HPP

#include <QItemDelegate>


class DelegacionBarraProgreso : public QItemDelegate {
	Q_OBJECT

	public:
		DelegacionBarraProgreso(QObject *padre = nullptr);

		void paint(QPainter *pintor, const QStyleOptionViewItem &opcion, const QModelIndex &indice) const;
};

#endif // DELEGACIONBARRAPROGRESO_HPP
