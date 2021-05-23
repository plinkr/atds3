#include "modeloentradas.hpp"
#include <QSqlTableModel>
#include <QSqlDatabase>


ModeloEntradas::ModeloEntradas(QObject *padre, const QSqlDatabase &baseDatos)
	: QSqlTableModel(padre, baseDatos) {
	setEditStrategy(QSqlTableModel::OnManualSubmit);
	setTable("entradas");
	setHeaderData(1, Qt::Horizontal, "Estado");
	setHeaderData(2, Qt::Horizontal, "Nombre");
	setHeaderData(3, Qt::Horizontal, "Completado");
	setHeaderData(4, Qt::Horizontal, "Velocidad");
	setHeaderData(5, Qt::Horizontal, "Ruta de guardado");
}
