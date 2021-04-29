#ifndef MODELOCATEGORIAS_HPP
#define MODELOCATEGORIAS_HPP

#include <QStandardItemModel>


class ModeloCategorias : public QStandardItemModel {
	Q_OBJECT

	public:
		ModeloCategorias(QObject *padre = nullptr);
		Qt::ItemFlags flags(const QModelIndex &) const override;
};

#endif // MODELOCATEGORIAS_HPP
