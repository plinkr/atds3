#ifndef DELEGACIONTAMANO_HPP
#define DELEGACIONTAMANO_HPP

#include <QItemDelegate>


class DelegacionTamano: public QItemDelegate {
	Q_OBJECT

	public:
		DelegacionTamano(bool velocidad = false, QObject *padre = nullptr);

		void paint(QPainter *pintor, const QStyleOptionViewItem &opcion, const QModelIndex &indice) const;

	private:
		bool _velocidad;
};

#endif // DELEGACIONTAMANO_HPP
