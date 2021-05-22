#ifndef DELEGACIONVELOCIDAD_HPP
#define DELEGACIONVELOCIDAD_HPP

#include <QItemDelegate>


class DelegacionVelocidad : public QItemDelegate {
	Q_OBJECT

	public:
		DelegacionVelocidad(QObject *padre = nullptr);

		void paint(QPainter *pintor, const QStyleOptionViewItem &opcion, const QModelIndex &indice) const;
};

#endif // DELEGACIONVELOCIDAD_HPP
