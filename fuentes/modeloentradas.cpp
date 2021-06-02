#include "modeloentradas.hpp"
#include <QSqlTableModel>
#include <QSqlDatabase>


ModeloEntradas::ModeloEntradas(QObject *padre, const QSqlDatabase &baseDatos)
	: QSqlTableModel(padre, baseDatos) {
	setEditStrategy(QSqlTableModel::OnManualSubmit);
	setTable("entradas");

	setHeaderData(2, Qt::Horizontal, "Estado");
	setHeaderData(4, Qt::Horizontal, "Nombre");
	setHeaderData(6, Qt::Horizontal, "Tamaño");
	setHeaderData(7, Qt::Horizontal, "Descargado");
	setHeaderData(8, Qt::Horizontal, "Completado");
	setHeaderData(9, Qt::Horizontal, "Velocidad");
}

void ModeloEntradas::eliminarFila(unsigned int id) {
	int fila = 0;

	for (fila = 0; fila < rowCount(); fila++) {
		if (data(index(fila, 0)).toUInt() == id) {
			break;
		}
	}

	beginRemoveRows(QModelIndex(), fila, fila);
	removeRow(fila, QModelIndex());
	endRemoveRows();
}
