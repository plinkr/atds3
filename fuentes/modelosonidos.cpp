#include "modelosonidos.hpp"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTextStream>


ModeloSonidos::ModeloSonidos(QObject *padre)
	: QSqlQueryModel(padre) {
	QSqlDatabase bd = QSqlDatabase::database();
	QSqlQuery solicitudSQL;
	QString instruccionSQL;

	if (bd.tables().indexOf("tema_sonidos") == -1) {
		solicitudSQL.exec("CREATE TABLE sonidos (id INTEGER PRIMARY KEY, titulo TEXT NOT NULL)");
		instruccionSQL = "INSERT INTO sonidos (id, titulo) VALUES\n";
		instruccionSQL += "(1, 'Encantado');";
		solicitudSQL.exec(instruccionSQL);
	}
	if (bd.tables().indexOf("sonidos") == -1) {
		solicitudSQL.exec("CREATE TABLE sonidos (id INTEGER PRIMARY KEY, tema INTEGER NOT NULL, titulo TEXT NOT NULL, ruta TEXT NOT NULL)");
		instruccionSQL = "INSERT INTO sonidos (tema, titulo, ruta) VALUES\n";
		instruccionSQL += "(1, 'ConexionDisponibleTodus', 'conexion-disponible-todus.ogg'),\n";
		instruccionSQL += "(1, 'ConexionPerdidaTodus', 'conexion-perdida-todus.ogg'),\n";
		instruccionSQL += "(1, 'InicializacionPaquete', 'incializacion-paquete.ogg'),\n";
		instruccionSQL += "(1, 'FinalizacionExitosaPaquete', 'finalizacion-exitosa-paquete.ogg'),\n";
		instruccionSQL += "(1, 'FinalizacionErroneaPaquete', 'finalizacion-erronea-paquete.ogg'),\n";
		instruccionSQL += "(1, 'InicializacionTarea', 'incializacion-tarea.ogg'),\n";
		instruccionSQL += "(1, 'FinalizacionExitosaTarea', 'finalizacion-exitosa-tarea.ogg'),\n";
		instruccionSQL += "(1, 'FinalizacionErroneaTarea', 'finalizacion-erronea-tarea.ogg');";
		solicitudSQL.exec(instruccionSQL);
	}

	solicitudSQL.exec("SELECT * FROM sonidos ORDER BY titulo");
	setQuery(solicitudSQL);
}

QHash<int, QByteArray> ModeloSonidos::roleNames() const {
	QHash<int, QByteArray> roles;

	for (int i = 0; i < this->record().count(); i++) {
		roles.insert(Qt::UserRole + i + 1, record().fieldName(i).toUtf8());
	}

	return roles;
}

QVariant ModeloSonidos::data(const QModelIndex &indice, int rol) const {
	QVariant value;

	if (indice.isValid()) {
		if (rol < Qt::UserRole) {
			value = QSqlQueryModel::data(indice, rol);
		} else {
			value = QSqlQueryModel::data(index(indice.row(), rol - Qt::UserRole - 1), Qt::DisplayRole);
		}
	}

	return value;
}

QVariantMap ModeloSonidos::obtener(int fila) const {
	QVariantMap resultados;

	if (fila < 0 || fila >= rowCount()) {
		return resultados;
	}

	resultados["id"] = QSqlQueryModel::data(index(fila, 0));
	resultados["titulo"] = QSqlQueryModel::data(index(fila, 1));
	resultados["ruta"] = QSqlQueryModel::data(index(fila, 2));

	return resultados;
}
