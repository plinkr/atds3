#include "modeloentradas.hpp"
#include <QSqlTableModel>
#include <QSqlDatabase>


ModeloEntradas::ModeloEntradas(QObject *padre, const QSqlDatabase &baseDatos)
	: QSqlTableModel(padre, baseDatos) {
	setTable("entradas");
	setHeaderData(0, Qt::Horizontal, "ID");
	setHeaderData(1, Qt::Horizontal, "Categoría");
	setHeaderData(2, Qt::Horizontal, "Estado");
	setHeaderData(3, Qt::Horizontal, "Nombre");
	setHeaderData(4, Qt::Horizontal, "Completado");
	setHeaderData(5, Qt::Horizontal, "Velocidad");
	setHeaderData(6, Qt::Horizontal, "Ruta de guardado");
	setHeaderData(7, Qt::Horizontal, "Enlace");
}
