#include "modelocategorias.hpp"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTextStream>


ModeloCategorias::ModeloCategorias(QObject *padre)
	: QSqlTableModel(padre) {
	QSqlDatabase bd = QSqlDatabase::database();
	QSqlQuery solicitudSQL;
	QString instruccionSQL;

	if (bd.tables().indexOf("categorias") == -1) {
		solicitudSQL.exec("CREATE TABLE categorias (id INTEGER PRIMARY KEY, titulo TEXT NOT NULL, icono TEXT NOT NULL, canalTodus TEXT DEFAULT '', descargarAutomaticamente BOOLEAN DEFAULT false)");
		instruccionSQL = "INSERT INTO categorias (titulo, icono, canalTodus, descargarAutomaticamente) VALUES\n";
		instruccionSQL += "('Descargas', 'cloud-download-alt.svg', '', false),\n";
		instruccionSQL += "('Publicaciones', 'cloud-upload-alt.svg', '', false),\n";
		instruccionSQL += "('Finalizadas', 'check-square.svg', '', false),\n";
		instruccionSQL += "('Programas', 'window-maximize.svg', '', false),\n";
		instruccionSQL += "('Música', 'music.svg', '', false),\n";
		instruccionSQL += "('Videos', 'video.svg', '', false),\n";
		instruccionSQL += "('Otros', 'question-circle.svg', '', false);";
		solicitudSQL.exec(instruccionSQL);
	}

	setTable("categorias");
	setSort(0, Qt::AscendingOrder);
	select();
}
ModeloCategorias::~ModeloCategorias() {
}

QHash<int, QByteArray> ModeloCategorias::roleNames() const {
	QHash<int, QByteArray> roles;

	for (int i = 0; i < record().count(); i++) {
		roles.insert(Qt::UserRole + i + 1, record().fieldName(i).toUtf8());
	}

	return roles;
}

QVariant ModeloCategorias::data(const QModelIndex &indice, int rol) const {
	QVariant value;

	if (indice.isValid()) {
		if (rol < Qt::UserRole) {
			value = QSqlTableModel::data(indice, rol);
		} else {
			value = QSqlTableModel::data(index(indice.row(), rol - Qt::UserRole - 1), Qt::DisplayRole);
		}
	}

	return value;
}

bool ModeloCategorias::setData(const QModelIndex &indice, const QVariant &valor, int rol) {
	bool resultados = false;
	int rolModificado = rol;

	if (indice.isValid() == false) {
		return resultados;
	} else {
		if (rol < Qt::UserRole) {
			resultados = QSqlTableModel::setData(indice, valor, rolModificado);
		} else {
			rolModificado = rol - Qt::UserRole - 1;
			resultados = QSqlTableModel::setData(index(indice.row(), rolModificado), valor, Qt::EditRole);
		}
	}

	submitAll();
	emit dataChanged(indice, indice, {rolModificado});

	return true;
}

QVariantMap ModeloCategorias::obtener(int fila) const {
	QVariantMap resultados;

	if (fila < 0 || fila >= rowCount()) {
		return resultados;
	}

	resultados["id"] = QSqlTableModel::data(index(fila, fieldIndex("id")));
	resultados["titulo"] = QSqlTableModel::data(index(fila, fieldIndex("titulo")));
	resultados["icono"] = QSqlTableModel::data(index(fila, fieldIndex("icono")));
	resultados["canalTodus"] = QSqlTableModel::data(index(fila, fieldIndex("canalTodus")));
	resultados["descargarAutomaticamente"] = QSqlTableModel::data(index(fila, fieldIndex("descargarAutomaticamente")));

	return resultados;
}

QVariantMap ModeloCategorias::obtenerDesdeId(int id) const {
	QSqlQuery solicitudSQL;
	QVariantMap resultados;

	solicitudSQL.exec(QString("SELECT * FROM categorias WHERE (id = %1) LIMIT 1").arg(id));

	if (solicitudSQL.next() == true) {
		resultados["id"] = solicitudSQL.value("id");
		resultados["titulo"] = solicitudSQL.value("titulo");
		resultados["icono"] = solicitudSQL.value("icono");
		resultados["canalTodus"] = solicitudSQL.value("canalTodus");
		resultados["descargarAutomaticamente"] = solicitudSQL.value("descargarAutomaticamente");
	}

	return resultados;
}

void ModeloCategorias::agregar(const QString &titulo, const QString &icono, const QString &canalTodus, bool descargarAutomaticamente) {
	QSqlRecord registro;

	beginInsertRows(QModelIndex(), rowCount(), rowCount());
	registro = record();
	registro.remove(0);
	registro.setValue("titulo", titulo);
	registro.setValue("icono", icono);
	registro.setValue("canalTodus", canalTodus);
	registro.setValue("descargarAutomaticamente", descargarAutomaticamente);
	insertRecord(-1, registro);
	endInsertRows();
	select();
}

void ModeloCategorias::actualizar(int fila, const QString &titulo, const QString &icono, const QString &canalTodus, bool descargarAutomaticamente) {
	if (fila < 0 || fila >= rowCount()) {
		return;
	}

	QSqlTableModel::setData(index(fila, fieldIndex("titulo")), titulo, Qt::EditRole);
	QSqlTableModel::setData(index(fila, fieldIndex("icono")), icono, Qt::EditRole);
	QSqlTableModel::setData(index(fila, fieldIndex("canalTodus")), canalTodus, Qt::EditRole);
	QSqlTableModel::setData(index(fila, fieldIndex("descargarAutomaticamente")), descargarAutomaticamente, Qt::EditRole);
	submitAll();

	emit dataChanged(index(fila, 0), index(fila, record().count() - 1));
}

void ModeloCategorias::eliminar(int fila) {
	if (fila < 0 || fila >= rowCount()) {
		return;
	}

	beginRemoveRows(QModelIndex(), fila, fila);
	removeRow(fila);
	submitAll();
	endRemoveRows();
}
