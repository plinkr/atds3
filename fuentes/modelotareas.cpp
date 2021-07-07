#include "modelotareas.hpp"
#include "main.hpp"
#include <QSqlQuery>
#include <QSqlRecord>


ModeloTareas::ModeloTareas(QObject *padre)
	: QSqlTableModel(padre) {
	QSqlQuery solicitudSQL;
	QString instruccionSQL;

	setTable("tareas");
	setSort(0, Qt::AscendingOrder);
	select();
}
ModeloTareas::~ModeloTareas() {
}

QHash<int, QByteArray> ModeloTareas::roleNames() const {
	QHash<int, QByteArray> roles;

	for (int i = 0; i < record().count(); i++) {
		roles.insert(Qt::UserRole + i + 1, record().fieldName(i).toUtf8());
	}

	return roles;
}

QVariant ModeloTareas::data(const QModelIndex &indice, int rol) const {
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

bool ModeloTareas::setData(const QModelIndex &indice, const QVariant &valor, int rol) {
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

void ModeloTareas::establecerFiltroPaquete(int paquete) {
	setFilter(QString("paquete = %1").arg(paquete));
	select();
	while (canFetchMore() == true) {
		fetchMore();
	}
}

QVariantMap ModeloTareas::obtener(int fila) const {
	QVariantMap resultados;

	if (fila < 0 || fila >= rowCount()) {
		return resultados;
	}

	resultados["id"] = QSqlTableModel::data(index(fila, fieldIndex("id")));
	resultados["paquete"] = QSqlTableModel::data(index(fila, fieldIndex("paquete")));
	resultados["estado"] = QSqlTableModel::data(index(fila, fieldIndex("estado")));
	resultados["nombre"] = QSqlTableModel::data(index(fila, fieldIndex("nombre")));
	resultados["ruta"] = QSqlTableModel::data(index(fila, fieldIndex("ruta")));
	resultados["enlace"] = QSqlTableModel::data(index(fila, fieldIndex("enlace")));
	resultados["enlaceFirmado"] = QSqlTableModel::data(index(fila, fieldIndex("enlaceFirmado")));
	resultados["tamano"] = QSqlTableModel::data(index(fila, fieldIndex("tamano")));
	resultados["tamanoTransferido"] = QSqlTableModel::data(index(fila, fieldIndex("tamanoTransferido")));
	resultados["completado"] = QSqlTableModel::data(index(fila, fieldIndex("completado")));
	resultados["velocidad"] = QSqlTableModel::data(index(fila, fieldIndex("velocidad")));
	resultados["error"] = QSqlTableModel::data(index(fila, fieldIndex("error")));

	return resultados;
}

QVariantMap ModeloTareas::obtenerDesdeId(int id) const {
	QSqlQuery solicitudSQL;
	QVariantMap resultados;

	solicitudSQL.exec(QString("SELECT * FROM tareas WHERE (id = %1) LIMIT 1").arg(id));

	if (solicitudSQL.next() == true) {
		resultados["id"] = solicitudSQL.value("id");
		resultados["paquete"] = solicitudSQL.value("paquete");
		resultados["estado"] = solicitudSQL.value("estado");
		resultados["nombre"] = solicitudSQL.value("nombre");
		resultados["ruta"] = solicitudSQL.value("ruta");
		resultados["enlace"] = solicitudSQL.value("enlace");
		resultados["enlaceFirmado"] = solicitudSQL.value("enlaceFirmado");
		resultados["tamano"] = solicitudSQL.value("tamano");
		resultados["tamanoTransferido"] = solicitudSQL.value("tamanoTransferido");
		resultados["completado"] = solicitudSQL.value("completado");
		resultados["velocidad"] = solicitudSQL.value("velocidad");
		resultados["error"] = solicitudSQL.value("error");
	}

	return resultados;
}

void ModeloTareas::actualizarEnlaces(int id, const QString &enlace, const QString &enlaceFirmado) {
	QSqlQuery solicitudSQL;

	solicitudSQL.exec(QString("UPDATE tareas SET enlace = '%1', enlaceFirmado = '%2' WHERE (id = %3)").arg(enlace).arg(QString(enlaceFirmado.toLocal8Bit().toBase64())).arg(id));
}

void ModeloTareas::actualizarEnlaceFirmado(int id, const QString &enlace) {
	QSqlQuery solicitudSQL;

	solicitudSQL.exec(QString("UPDATE tareas SET enlaceFirmado = '%1' WHERE (id = %2)").arg(QString(enlace.toLocal8Bit().toBase64())).arg(id));
}

void ModeloTareas::actualizarEstado(int id, int estado) {
	int fila = 0;

	corregirFila(fila, id);
	actualizarCampo(fila, "estado", estado);
}

void ModeloTareas::actualizarCampo(int fila, const QString &campo, const QVariant &valor) {
	if (fila < 0 || fila >= rowCount()) {
		return;
	}

	int indiceCampo = fieldIndex(campo);

	QSqlTableModel::setData(index(fila, indiceCampo), valor, Qt::EditRole);
	submitAll();

	emit dataChanged(index(fila, indiceCampo), index(fila, indiceCampo));
}

void ModeloTareas::actualizarCampos(int fila, const QVariantMap &campos) {
	if (fila < 0 || fila >= rowCount()) {
		return;
	}

	int indiceCampo = 0;
	int indiceCampoMenor = 0;
	int indiceCampoMayor = 0;

	for (const QString &c : campos.keys()) {
		indiceCampo = fieldIndex(c);
		if (indiceCampo < indiceCampoMenor) {
			indiceCampoMenor = indiceCampo;
		}
		if (indiceCampo > indiceCampoMayor) {
			indiceCampoMayor = indiceCampo;
		}

		QSqlTableModel::setData(index(fila, indiceCampo), campos[c], Qt::EditRole);
	}

	submitAll();

	emit dataChanged(index(fila, indiceCampoMenor), index(fila, indiceCampoMayor));
}

bool ModeloTareas::corregirFila(int &fila, int id) {
	if (QSqlTableModel::data(index(fila, 0)).toInt() != id) {
		for (int f = 0; f < rowCount(); f++) {
			if (QSqlTableModel::data(index(f, 0)).toInt() == id) {
				fila = f;
				return true;
			}
		}

		return false;
	}

	return true;
}
