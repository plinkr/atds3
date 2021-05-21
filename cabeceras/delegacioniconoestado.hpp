#ifndef DELEGACIONICONOESTADO_HPP
#define DELEGACIONICONOESTADO_HPP

#include <QItemDelegate>


class DelegacionIconoEstado : public QItemDelegate {
	Q_OBJECT

	public:
		DelegacionIconoEstado(QObject *padre = nullptr);

		void paint(QPainter *pintor, const QStyleOptionViewItem &opcion, const QModelIndex &indice) const;
};

#endif // DELEGACIONICONOESTADO_HPP
