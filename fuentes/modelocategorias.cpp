#include "modelocategorias.hpp"


ModeloCategorias::ModeloCategorias(QObject *padre) : QStandardItemModel(padre) {}

Qt::ItemFlags ModeloCategorias::flags(const QModelIndex &) const {
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
