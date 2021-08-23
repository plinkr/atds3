#include "modelopaquetes.hpp"
#include "modelocategorias.hpp"
#include "modelotareas.hpp"
#include "utiles.hpp"
#include "main.hpp"
#include <ctime>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlResult>
#include <QSqlRecord>
#include <QSqlError>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStandardPaths>
#include <QQmlFile>


ModeloPaquetes::ModeloPaquetes(QObject *padre)
	: QSqlTableModel(padre), _categoria(0) {
	QSqlDatabase bd = QSqlDatabase::database();
	QSqlQuery solicitudSQL;
	QString instruccionSQL;

	_toDus.setParent(this);

	if (bd.tables().indexOf("paquetes") == -1) {
		solicitudSQL.exec("CREATE TABLE paquetes (id INTEGER PRIMARY KEY, tipo INTEGER NOT NULL, formato INTEGER NOT NULL, clasificacion INTEGER NOT NULL, categoria INTEGER NOT NULL, estado INTEGER NOT NULL, nombre TEXT NOT NULL, tamano INTEGER NOT NULL, tamanoTransferido INTEGER NOT NULL, completado INTEGER NOT NULL, velocidad INTEGER NOT NULL, error BOOLEAN NOT NULL)");
		solicitudSQL.exec("CREATE INDEX paquetes_tipo ON paquetes (tipo)");
		solicitudSQL.exec("CREATE INDEX paquetes_categoria ON paquetes (categoria)");
		solicitudSQL.exec("CREATE INDEX paquetes_estado ON paquetes (estado)");
	}
	if (bd.tables().indexOf("tareas") == -1) {
		solicitudSQL.exec("CREATE TABLE tareas (id INTEGER PRIMARY KEY, paquete INTEGER NOT NULL, estado INTEGER NOT NULL, nombre TEXT NOT NULL, ruta TEXT NOT NULL, enlace TEXT NOT NULL, enlaceFirmado TEXT NOT NULL, tamano INTEGER NOT NULL, tamanoTransferido INTEGER NOT NULL, completado INTEGER NOT NULL, velocidad INTEGER NOT NULL, error BOOLEAN NOT NULL)");
		solicitudSQL.exec("CREATE INDEX tareas_paquete ON tareas (paquete)");
		solicitudSQL.exec("CREATE INDEX tareas_estado ON tareas (estado)");
	}

	setTable("paquetes");
	setSort(0, Qt::AscendingOrder);
	select();

	procesarColaEjecucion();
}

QHash<int, QByteArray> ModeloPaquetes::roleNames() const {
	QHash<int, QByteArray> roles;

	for (int i = 0; i < record().count(); i++) {
		roles.insert(Qt::UserRole + i + 1, record().fieldName(i).toUtf8());
	}

	return roles;
}

QVariant ModeloPaquetes::data(const QModelIndex &indice, int rol) const {
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

bool ModeloPaquetes::setData(const QModelIndex &indice, const QVariant &valor, int rol) {
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

void ModeloPaquetes::establecerObjetoUtiles(Utiles *objeto) {
	_utiles = objeto;
}

void ModeloPaquetes::establecerFiltroCategoria(int categoria) {
	setFilter(obtenerFiltroCategoria(categoria));
	select();
}

void ModeloPaquetes::establecerModeloTareas(qint64 paquete, ModeloTareas *modelo) {
	_modelosTareas[paquete] = modelo;
}

void ModeloPaquetes::eliminarModeloTareas(qint64 paquete) {
	_modelosTareas[paquete] = nullptr;
}

void ModeloPaquetes::eliminarFiltros() {
	_categoria = 0;
	setFilter("");
	select();
}

QVariantMap ModeloPaquetes::obtener(int fila) const {
	QVariantMap resultados;

	if (fila < 0 || fila >= rowCount()) {
		return resultados;
	}

	resultados["id"] = QSqlTableModel::data(index(fila, fieldIndex("id")));
	resultados["tipo"] = QSqlTableModel::data(index(fila, fieldIndex("tipo")));
	resultados["formato"] = QSqlTableModel::data(index(fila, fieldIndex("formato")));
	resultados["clasificacion"] = QSqlTableModel::data(index(fila, fieldIndex("clasificacion")));
	resultados["categoria"] = QSqlTableModel::data(index(fila, fieldIndex("categoria")));
	resultados["estado"] = QSqlTableModel::data(index(fila, fieldIndex("estado")));
	resultados["nombre"] = QSqlTableModel::data(index(fila, fieldIndex("nombre")));
	resultados["tamano"] = QSqlTableModel::data(index(fila, fieldIndex("tamano")));
	resultados["tamanoTransferido"] = QSqlTableModel::data(index(fila, fieldIndex("tamanoTransferido")));
	resultados["completado"] = QSqlTableModel::data(index(fila, fieldIndex("completado")));
	resultados["velocidad"] = QSqlTableModel::data(index(fila, fieldIndex("velocidad")));
	resultados["error"] = QSqlTableModel::data(index(fila, fieldIndex("error")));

	return resultados;
}

QVariantMap ModeloPaquetes::obtenerDesdeId(qint64 id) const {
	QSqlQuery solicitudSQL;
	QVariantMap resultados;

	solicitudSQL.exec("SELECT * FROM paquetes WHERE (id = " + QString::number(id) + ") LIMIT 1");

	if (solicitudSQL.next() == true) {
		resultados["id"] = solicitudSQL.value("id");
		resultados["tipo"] = solicitudSQL.value("tipo");
		resultados["formato"] = solicitudSQL.value("formato");
		resultados["clasificacion"] = solicitudSQL.value("clasificacion");
		resultados["categoria"] = solicitudSQL.value("categoria");
		resultados["estado"] = solicitudSQL.value("estado");
		resultados["nombre"] = solicitudSQL.value("nombre");
		resultados["tamano"] = solicitudSQL.value("tamano");
		resultados["tamanoTransferido"] = solicitudSQL.value("tamanoTransferido");
		resultados["completado"] = solicitudSQL.value("completado");
		resultados["velocidad"] = solicitudSQL.value("velocidad");
		resultados["error"] = solicitudSQL.value("error");
	}

	return resultados;
}

QVariantMap ModeloPaquetes::obtenerTareaDesdeId(qint64 id) const {
	QSqlQuery solicitudSQL;
	QVariantMap resultados;

	solicitudSQL.exec("SELECT * FROM tareas WHERE (id = " + QString::number(id) + ") LIMIT 1");

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

void ModeloPaquetes::actualizarEnlaces(qint64 id, const QString &enlace, const QString &enlaceFirmado) {
	QSqlQuery solicitudSQL;

	solicitudSQL.exec("UPDATE tareas SET enlace = '" + enlace + "', enlaceFirmado = '" + enlaceFirmado + "' WHERE (id = " + QString::number(id) + ")");
}

void ModeloPaquetes::actualizarEnlaceFirmado(qint64 id, const QString &enlace) {
	QSqlQuery solicitudSQL;

	solicitudSQL.exec("UPDATE tareas SET enlaceFirmado = '" + enlace + "' WHERE (id = " + QString::number(id) + ")");
}

void ModeloPaquetes::actualizarEstado(qint64 id, Estados estado) {
	int fila = 0;

	corregirFila(fila, id);
	actualizarCampo(fila, "estado", estado);
}

void ModeloPaquetes::actualizarCampo(int fila, const QString &campo, const QVariant &valor) {
	if (fila < 0 || fila >= rowCount()) {
		return;
	}

	int indiceCampo = fieldIndex(campo);

	QSqlTableModel::setData(index(fila, indiceCampo), valor, Qt::EditRole);
	submitAll();

	emit dataChanged(index(fila, indiceCampo), index(fila, indiceCampo));
}

void ModeloPaquetes::actualizarCampos(int fila, const QVariantMap &campos) {
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

bool ModeloPaquetes::corregirFila(int &fila, qint64 id) {
	if (fila < rowCount()) {
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

	return false;
}

void ModeloPaquetes::eliminar(int fila) {
	if (fila < 0 || fila >= rowCount()) {
		return;
	}

	QSqlQuery solicitudSQL;
	qint64 idPaquete = QSqlTableModel::data(index(fila, fieldIndex("id"))).toLongLong();

	pausar(fila);

	solicitudSQL.exec("DELETE FROM tareas WHERE (paquete = " + QString::number(idPaquete) + ")");
	solicitudSQL.exec("DELETE FROM paquetes WHERE (id = " + QString::number(idPaquete) + ")");

	select();
}

void ModeloPaquetes::eliminarTodas() {
	QSqlQuery solicitudSQLPaquetes;
	QSqlQuery solicitudSQLTareas;
	QString filtroCategoria = obtenerFiltroCategoria(_categoria);

	pausarTodas();

	solicitudSQLPaquetes.exec("SELECT * FROM paquetes WHERE (" + filtroCategoria + ")");
	while (solicitudSQLPaquetes.next() == true) {
		solicitudSQLTareas.exec("DELETE FROM tareas WHERE (paquete = " + QString::number(solicitudSQLPaquetes.value("id").toLongLong()) + ")");
	}
	solicitudSQLPaquetes.exec("DELETE FROM paquetes WHERE (" + filtroCategoria + ")");

	select();
}

void ModeloPaquetes::agregarDescargasDesdeArchivos(QList<QUrl> archivos) {
	for (const QUrl &uri : archivos) {
		QString ruta = _utiles->rutaDesdeURI(uri.toString());

		if (ruta.endsWith(".txt") == true) {
			procesarArchivoDescargaClasico(ruta);
		}
	}
}

void ModeloPaquetes::agregarPublicacion(const QString &titulo, QAbstractItemModel *modeloArchivos, int formato, int clasificacion) {
	QSqlQuery solicitudSQL;
	QString instruccionSQL;
	bool primeraFila = true;
	qint64 idPaquete = 0;
	QString nombre;
	QString ruta;
	QFileInfo informacionArchivo;
	qint64 tamanoPaquete = 0;
	qint64 tamanoTarea = 0;
	QString tituloPaquete = titulo;

	tituloPaquete.replace(QRegularExpression("[/:*?\"'<>|()]"), "_");
	tituloPaquete.replace("\\", "_");
	instruccionSQL = "INSERT INTO tareas (paquete, estado, nombre, ruta, enlace, enlaceFirmado, tamano, tamanoTransferido, completado, velocidad, error) VALUES ";

	for (int f = 0; f < modeloArchivos->rowCount(); f++) {
		nombre = modeloArchivos->data(modeloArchivos->index(f, 0), 0).toByteArray();
		nombre.replace(QRegularExpression("[/:*?\"'<>|()]"), "_");
		nombre.replace("\\", "_");
		ruta = modeloArchivos->data(modeloArchivos->index(f, 0), 1).toString();

		informacionArchivo.setFile(ruta);
		if (informacionArchivo.exists() == true) {
			tamanoTarea = informacionArchivo.size();
			tamanoPaquete += tamanoTarea;
		} else {
			continue;
		}

		if (primeraFila == false) {
			instruccionSQL += ", ";
		}
		instruccionSQL += "(##ID_PAQUETE##, " + QString::number(Estados::Pausado) + ", '" + nombre.toUtf8().toPercentEncoding() + "', '" + ruta.toUtf8().toPercentEncoding() + "', '', '', " + QString::number(tamanoTarea) + ", 0, 0, 0, false)";
		if (primeraFila == true) {
			primeraFila = false;
		}
	}

	instruccionSQL += ";";

	if (primeraFila == false) {
		solicitudSQL.exec("INSERT INTO paquetes (tipo, formato, clasificacion, categoria, estado, nombre, tamano, tamanoTransferido, completado, velocidad, error) VALUES (" + QString::number(Tipos::Publicacion) + ", " + QString::number(formato) + ", " + QString::number(clasificacion) + ", 1, " + QString::number(Estados::Pausado) + ", '" + tituloPaquete.toUtf8().toPercentEncoding() + "', " + QString::number(tamanoPaquete) + ", 0, 0, 0, false)");
		if (solicitudSQL.lastInsertId().isValid() == true) {
			idPaquete = solicitudSQL.lastInsertId().toInt();
			instruccionSQL.replace("##ID_PAQUETE##", QString::number(idPaquete));
			solicitudSQL.exec(instruccionSQL);
			select();
		}
	}
}

void ModeloPaquetes::procesarArchivoDescargaClasico(const QString &ruta) {
	QSqlQuery solicitudSQL;
	QString instruccionSQL;
	bool primeraFila = true;
/*
#ifdef Q_OS_ANDROID
	QFile lector;
	lector.open(_utiles->androidAbrirArchivo(ruta, "r"), QIODevice::ReadOnly, QFileDevice::AutoCloseHandle);
#else
*/
	QFile lector(ruta);
//#endif
	QString nombreArchivo = lector.fileName().mid(lector.fileName().lastIndexOf("/") + 1);
	qint64 idPaquete = 0;

	nombreArchivo.chop(4);
	nombreArchivo.replace(QRegularExpression("[/:*?\"'<>|()]"), "_");
	nombreArchivo.replace("\\", "_");
	primeraFila = true;
/*
#ifdef Q_OS_ANDROID
	if (lector.isOpen() == false) {
		return;
	}
#else
*/
	if (lector.open(QIODevice::ReadOnly | QIODevice::Text) == false) {
		return;
	}
//#endif

	instruccionSQL = "INSERT INTO tareas (paquete, estado, nombre, ruta, enlace, enlaceFirmado, tamano, tamanoTransferido, completado, velocidad, error) VALUES ";

	while (lector.atEnd() == false) {
		QList<QByteArray> campos = lector.readLine().split('\t');
		QString nombre, enlace;

		if (campos.size() > 0) {
			enlace = campos[0].trimmed();
			if (campos[0].startsWith("https://s3.todus.cu/") == true) {
				if (primeraFila == false) {
					instruccionSQL += ", ";
				}
				nombre = campos[1].trimmed();
				nombre.replace(QRegularExpression("[/:*?\"'<>|()]"), "_");
				nombre.replace("\\", "_");
				instruccionSQL += "(##ID_PAQUETE##, " + QString::number(Estados::Pausado) + ", '" + nombre.toUtf8().toPercentEncoding() + "', '', '" + enlace + "', '', 0, 0, 0, 0, false)";
				if (primeraFila == true) {
					primeraFila = false;
				}
			}
		}
	}

	instruccionSQL += ";";

	if (primeraFila == false) {
		solicitudSQL.exec("INSERT INTO paquetes (tipo, formato, clasificacion, categoria, estado, nombre, tamano, tamanoTransferido, completado, velocidad, error) VALUES (" + QString::number(Tipos::Descarga) + ", " + QString::number(Formatos::Clasico) + ", 0, " + QString::number(_categoria) + ", " + QString::number(Estados::Pausado) + ", '" + nombreArchivo.toUtf8().toPercentEncoding() + "', 0, 0, 0, 0, false)");
		if (solicitudSQL.lastInsertId().isValid() == true) {
			idPaquete = solicitudSQL.lastInsertId().toInt();
		}
		instruccionSQL.replace("##ID_PAQUETE##", QString::number(idPaquete));
		solicitudSQL.exec(instruccionSQL);
		select();
	}
}

void ModeloPaquetes::iniciar(int fila) {
	if (fila < 0 || fila >= rowCount()) {
		return;
	}

	qint64 idPaquete = QSqlTableModel::data(index(fila, fieldIndex("id"))).toLongLong();
	int indiceCampoEstado = fieldIndex("estado");
	int indiceCampoError = fieldIndex("error");
	int estado = QSqlTableModel::data(index(fila, indiceCampoEstado)).toInt();

	if (estado == Estados::Pausado || estado == Estados::Error) {
		agregarPaqueteAlListado(idPaquete, fila);

		QSqlTableModel::setData(index(fila, indiceCampoEstado), Estados::EnEsperaIniciar, Qt::EditRole);
		QSqlTableModel::setData(index(fila, indiceCampoError), false, Qt::EditRole);
		submitAll();

		emit dataChanged(index(fila, indiceCampoEstado), index(fila, indiceCampoError));

		if (_modelosTareas[idPaquete] != nullptr) {
			_modelosTareas[idPaquete]->select();
		}

		procesarColaEjecucion();
	}
}

void ModeloPaquetes::pausar(int fila) {
	if (fila < 0 || fila >= rowCount()) {
		return;
	}

	QSqlQuery solicitudSQL;
	qint64 idPaquete = QSqlTableModel::data(index(fila, fieldIndex("id"))).toLongLong();
	int indiceCampoEstado = fieldIndex("estado");
	int indiceCampoVelocidad = fieldIndex("velocidad");
	int estado = QSqlTableModel::data(index(fila, indiceCampoEstado)).toInt();
	QString enlace = QSqlTableModel::data(index(fila, fieldIndex("enlace"))).toString();

	if (estado == Estados::EnEsperaIniciar || estado == Estados::Iniciado) {
		eliminarPaqueteDelListado(idPaquete);

		if (_modelosTareas[idPaquete]) {
			_modelosTareas[idPaquete]->select();
		}

		QSqlTableModel::setData(index(fila, indiceCampoEstado), Estados::Pausado, Qt::EditRole);
		QSqlTableModel::setData(index(fila, indiceCampoVelocidad), 0, Qt::EditRole);
		submitAll();

		emit dataChanged(index(fila, indiceCampoEstado), index(fila, indiceCampoVelocidad));

		procesarColaEjecucion();
	}
}

void ModeloPaquetes::iniciarTodas() {
	QSqlQuery solicitudSQLPaquetes;
	QSqlQuery solicitudSQLTareas;
	QString filtroCategoria = obtenerFiltroCategoria(_categoria);
	QString filtrosEstados = "(estado == " + QString::number(Estados::Pausado) + " OR estado == " + QString::number(Estados::Error) + ")";

	solicitudSQLPaquetes.exec("UPDATE paquetes SET estado = " + QString::number(Estados::EnEsperaIniciar) + ", error = false WHERE (" + filtroCategoria + " AND " + filtrosEstados + ")");
	solicitudSQLPaquetes.exec("SELECT * FROM paquetes WHERE (" + filtroCategoria + " AND estado = " + QString::number(Estados::EnEsperaIniciar) + ")");
	while (solicitudSQLPaquetes.next() == true) {
		agregarPaqueteAlListado(solicitudSQLPaquetes.value("id").toLongLong(), 0);
	}

	select();

	procesarColaEjecucion();
}

void ModeloPaquetes::pausarTodas() {
	QSqlQuery solicitudSQL;
	QString filtroCategoria = obtenerFiltroCategoria(_categoria);
	QList<qint64> _idPaquetes;
	QString filtroPaquetes;
	qint64 idPaquete = 0;

	solicitudSQL.exec("SELECT id FROM paquetes WHERE (" + filtroCategoria + " AND (estado = " + QString::number(Estados::EnEsperaIniciar) + " OR estado = " + QString::number(Estados::Iniciado) + "))");
	while (solicitudSQL.next() == true) {
		idPaquete = solicitudSQL.value("id").toLongLong();
		eliminarPaqueteDelListado(idPaquete);

		if (_modelosTareas[idPaquete]) {
			_modelosTareas[idPaquete]->select();
		}
	}

	select();

	procesarColaEjecucion();
}

void ModeloPaquetes::iniciarTodosPaquetes() {
	QSqlQuery solicitudSQLPaquetes;
	QSqlQuery solicitudSQLTareas;
	QString filtrosEstados = "(estado == " + QString::number(Estados::Pausado) + " OR estado == " + QString::number(Estados::Error) + ")";

	solicitudSQLPaquetes.exec("UPDATE paquetes SET estado = " + QString::number(Estados::EnEsperaIniciar) + ", error = false WHERE (" + filtrosEstados + ")");
	solicitudSQLPaquetes.exec("SELECT * FROM paquetes WHERE (estado = " + QString::number(Estados::EnEsperaIniciar) + ")");
	while (solicitudSQLPaquetes.next() == true) {
		agregarPaqueteAlListado(solicitudSQLPaquetes.value("id").toLongLong(), 0);
	}

	select();

	procesarColaEjecucion();
}

void ModeloPaquetes::pausarTodosPaquetes() {
	QSqlQuery solicitudSQL;
	QList<qint64> _idPaquetes;
	QString filtroPaquetes;
	qint64 idPaquete = 0;

	solicitudSQL.exec("SELECT id FROM paquetes WHERE (estado = " + QString::number(Estados::EnEsperaIniciar) + " OR estado = " + QString::number(Estados::Iniciado) + ")");
	while (solicitudSQL.next() == true) {
		idPaquete = solicitudSQL.value("id").toLongLong();
		eliminarPaqueteDelListado(idPaquete);

		if (_modelosTareas[idPaquete]) {
			_modelosTareas[idPaquete]->select();
		}
	}

	select();

	procesarColaEjecucion();
}

void ModeloPaquetes::confirmarCodigo(const QString &codigo) {
	_toDus.solicitarFichaSolicitud(codigo);
}

void ModeloPaquetes::iniciarDescarga(qint64 paquete, qint64 id, const QString &enlace) {
	QVariantMap registroPaquete = obtenerDesdeId(paquete);
	QVariantMap registroTarea = obtenerTareaDesdeId(id);
	ModeloCategorias modeloCategoria(this);
	QVariantMap registroCategoria = modeloCategoria.obtenerDesdeId(registroPaquete["categoria"].toInt());
	QString servidorS3 = _configuraciones.value("avanzadas/servidorS3", "s3.todus.cu").toString();
	int puertoServidorS3 = _configuraciones.value("avanzadas/servidorS3Puerto", 443).toInt();
	QString rutaHTTP = enlace;
	QByteArray autorizacion = "Bearer " + _configuraciones.value("todus/fichaAcceso").toByteArray();
	QRegularExpression re;
	QRegularExpressionMatch rem;
	QDir directorioDescarga;
	QString rutaArchivo;
	QFileInfo informacionArchivo;
	QSqlQuery solicitudSQL;

	_tareasDescargas.insert(id, new Tarea());
	_tareasDescargas[id]->paquete = paquete;
	_tareasDescargas[id]->fila = 0;
	_tareasDescargas[id]->nombre = QByteArray::fromPercentEncoding(registroTarea["nombre"].toByteArray());
	_tareasDescargas[id]->enlace = "";
	_tareasDescargas[id]->ultimoTiempoTransferencia = std::time(nullptr);

	if (corregirFila(_paquetes[paquete]->fila, paquete) == true) {
		actualizarCampo(_paquetes[paquete]->fila, "estado", Estados::Iniciado);
	} else {
		solicitudSQL.exec("UPDATE paquetes SET estado = " + QString::number(Estados::Iniciado) + " WHERE (id = " + QString::number(paquete) + ")");
	}

	_tareasDescargas[id]->tamano = registroTarea["tamano"].toLongLong();
	_tareasDescargas[id]->tamanoTransferido = registroTarea["tamanoTransferido"].toLongLong();
	_tareasDescargas[id]->completado = registroTarea["completado"].toLongLong();

	if (_modelosTareas[paquete]) {
		if (_modelosTareas[paquete]->corregirFila(_tareasDescargas[id]->fila, id) == true) {
			_modelosTareas[paquete]->actualizarCampo(_tareasDescargas[id]->fila, "estado", Estados::Iniciado);
		}
	} else {
		solicitudSQL.exec("UPDATE tareas SET estado = " + QString::number(Estados::Iniciado) + " WHERE (id = " + QString::number(id) + ")");
	}

	rutaHTTP.remove(0, 9 + servidorS3.size());

	rutaArchivo = QString("%1/%2/%3").arg(_utiles->rutaDesdeURI(_configuraciones.value("descargas/ruta", _rutaDescargaPredeterminada).toString())).arg(registroCategoria["titulo"].toString()).arg(QByteArray::fromPercentEncoding(registroPaquete["nombre"].toByteArray()).constData());
	if (directorioDescarga.exists(rutaArchivo) == false) {
		directorioDescarga.mkpath(rutaArchivo);
	}
	rutaArchivo += "/" + _tareasDescargas[id]->nombre;

	informacionArchivo.setFile(rutaArchivo);
	if (informacionArchivo.exists() == true) {
		QVariantMap campos;

		_tareasDescargas[id]->tamanoTransferido = informacionArchivo.size();
		if (_tareasDescargas[id]->tamano > 0) {
			_tareasDescargas[id]->completado = (_tareasDescargas[id]->tamanoTransferido * 100) / _tareasDescargas[id]->tamano;
			if (_tareasDescargas[id]->completado < 0) {
				_tareasDescargas[id]->completado = 0;
			}
			if (_tareasDescargas[id]->completado > 100) {
				_tareasDescargas[id]->completado = 100;
			}
		} else {
			_tareasDescargas[id]->completado = 0;
		}

		campos["tamanoTransferido"] = _tareasDescargas[id]->tamanoTransferido;
		campos["completado"] = _tareasDescargas[id]->completado;
		campos["error"] = false;

		if (_modelosTareas[paquete]) {
			if (_modelosTareas[paquete]->corregirFila(_tareasDescargas[id]->fila, id) == true) {
				_modelosTareas[paquete]->actualizarCampos(_tareasDescargas[id]->fila, campos);
			}
		} else {
			solicitudSQL.exec("UPDATE tareas SET tamanoTransferido = " + QString::number(_tareasDescargas[id]->tamanoTransferido) + ", completado = " + QString::number(_tareasDescargas[id]->completado) + ", error = false WHERE (id = " + QString::number(id) + ")");
		}
	} else {
		QVariantMap campos;
		_tareasDescargas[id]->tamanoTransferido = 0;
		_tareasDescargas[id]->completado = 0;

		campos["tamanoTransferido"] = _tareasDescargas[id]->tamanoTransferido;
		campos["completado"] = _tareasDescargas[id]->completado;
		campos["error"] = false;

		if (_modelosTareas[paquete]) {
			if (_modelosTareas[paquete]->corregirFila(_tareasDescargas[id]->fila, id) == true) {
				_modelosTareas[paquete]->actualizarCampos(_tareasDescargas[id]->fila, campos);
			}
		} else {
			solicitudSQL.exec("UPDATE tareas SET tamanoTransferido = 0, completado = 0, error = false WHERE (id = " + QString::number(id) + ")");
		}
	}

	_tareasDescargas[id]->http.establecerMetodo(HTTP::Metodo::OBTENER);
	_tareasDescargas[id]->http.establecerAnfitrion(servidorS3);
	_tareasDescargas[id]->http.establecerPuerto(puertoServidorS3);
	_tareasDescargas[id]->http.establecerRuta(rutaHTTP);

	_tareasDescargas[id]->archivo.setFileName(rutaArchivo);
	_tareasDescargas[id]->archivo.setPermissions(QFileDevice::ReadUser | QFileDevice::WriteUser);
	_tareasDescargas[id]->archivo.open(QIODevice::Append);
	_tareasDescargas[id]->http.establecerDestinoDescarga(&_tareasDescargas[id]->archivo);

	_tareasDescargas[id]->http.agregarCabeceraSolicitud("Accept", "application/octet-stream");
	_tareasDescargas[id]->http.agregarCabeceraSolicitud("Authorization", autorizacion);
	_tareasDescargas[id]->http.agregarCabeceraSolicitud("Host", servidorS3);
	_tareasDescargas[id]->http.agregarCabeceraSolicitud("User-Agent", _configuraciones.value("avanzadas/agenteUsuario", _agenteUsuarioTodus).toString() + " HTTP-Download");
	if (_tareasDescargas[id]->tamanoTransferido > 0) {
		_tareasDescargas[id]->http.establecerBytesDescargados(_tareasDescargas[id]->tamanoTransferido);
		_tareasDescargas[id]->http.agregarCabeceraSolicitud("Range", "bytes=" + QString::number(_tareasDescargas[id]->tamanoTransferido).toLocal8Bit() + "-");
	}

	connect(&_tareasDescargas[id]->http, &HTTP::cabecerasRecibidas, this, &ModeloPaquetes::eventoTareaIniciada);
	connect(&_tareasDescargas[id]->http, &HTTP::bytesDescargaTransferidos, this, &ModeloPaquetes::eventoTareaTotalBytesTransferidos);
	connect(&_tareasDescargas[id]->http, &HTTP::detenido, this, &ModeloPaquetes::eventoTareaDetenida);
	connect(&_tareasDescargas[id]->http, &HTTP::detenidoParaReiniciar, this, &ModeloPaquetes::eventoTareaDetenidaParaReiniciar);
	connect(&_tareasDescargas[id]->http, &HTTP::finalizado, this, &ModeloPaquetes::eventoTareaFinalizada);

	_tareasDescargas[id]->http.establecerId(id);
	_tareasDescargas[id]->http.ejecutar();
}

void ModeloPaquetes::iniciarPublicacion(qint64 paquete, qint64 id, const QString &enlace, const QString &enlaceFirmado) {
	QVariantMap registroPaquete = obtenerDesdeId(paquete);
	QVariantMap registroTarea = obtenerTareaDesdeId(id);
	ModeloCategorias modeloCategoria(this);
	QVariantMap registroCategoria = modeloCategoria.obtenerDesdeId(registroPaquete["categoria"].toInt());
	QString servidorS3 = _configuraciones.value("avanzadas/servidorS3", "s3.todus.cu").toString();
	int puertoServidorS3 = _configuraciones.value("avanzadas/servidorS3Puerto", 443).toInt();
	QString rutaHTTP = enlaceFirmado;
	QByteArray autorizacion = "Bearer " + _configuraciones.value("todus/fichaAcceso").toByteArray();
	QRegularExpression re;
	QRegularExpressionMatch rem;
	QFileInfo informacionArchivo;
	QSqlQuery solicitudSQL;

	_tareasPublicaciones.insert(id, new Tarea());
	_tareasPublicaciones[id]->paquete = paquete;
	_tareasPublicaciones[id]->fila = 0;
	_tareasPublicaciones[id]->nombre = QByteArray::fromPercentEncoding(registroTarea["nombre"].toByteArray());
	_tareasPublicaciones[id]->ruta = QByteArray::fromPercentEncoding(registroTarea["ruta"].toByteArray());
	_tareasPublicaciones[id]->enlace = enlace;
	_tareasPublicaciones[id]->tamanoTransferido = 0;
	_tareasPublicaciones[id]->ultimoTiempoTransferencia = std::time(nullptr);

	if (corregirFila(_paquetes[paquete]->fila, paquete) == true) {
		actualizarCampo(_paquetes[paquete]->fila, "estado", Estados::Iniciado);
	} else {
		solicitudSQL.exec("UPDATE paquetes SET estado = " + QString::number(Estados::Iniciado) + " WHERE (id = " + QString::number(paquete) + ")");
	}

	_tareasPublicaciones[id]->tamano = registroTarea["tamano"].toLongLong();

	if (_modelosTareas[paquete]) {
		if (_modelosTareas[paquete]->corregirFila(_tareasPublicaciones[id]->fila, id) == true) {
			QVariantMap campos;

			campos["estado"] = Estados::Iniciado;
			campos["enlace"] = enlace;
			_modelosTareas[paquete]->actualizarCampos(_tareasPublicaciones[id]->fila, campos);
		}
	} else {
		solicitudSQL.exec("UPDATE tareas SET estado = " + QString::number(Estados::Iniciado) + ", enlace = '" + enlace + "' WHERE (id = " + QString::number(id) + ")");
	}

	rutaHTTP.remove(0, 9 + servidorS3.size());

	informacionArchivo.setFile(_tareasPublicaciones[id]->ruta);
	if (informacionArchivo.exists() == true) {
		if (_tareasPublicaciones[id]->tamano != informacionArchivo.size()) {
			if (_modelosTareas[paquete]) {
				registroTarea.clear();
				registroTarea["estado"] = Estados::Error;
				registroTarea["error"] = true;
				if (_modelosTareas[paquete]->corregirFila(_tareasPublicaciones[id]->fila, id) == true) {
					_modelosTareas[paquete]->actualizarCampos(_tareasPublicaciones[id]->fila, registroTarea);
				}
			} else {
				solicitudSQL.exec("UPDATE tareas SET estado = " + QString::number(Estados::Error) + ", error = true WHERE (id = " + QString::number(id) + ")");
			}
			if (corregirFila(_paquetes[paquete]->fila, paquete) == true) {
				actualizarCampo(_paquetes[paquete]->fila, "error", true);
			} else {
				solicitudSQL.exec("UPDATE paquetes SET error = true WHERE (id = " + QString::number(paquete) + ")");
			}

			_tareasPublicaciones.remove(id);
			procesarColaEjecucion();

			return;
		}
	} else {
		if (_modelosTareas[paquete]) {
			registroTarea.clear();
			registroTarea["estado"] = Estados::Error;
			registroTarea["error"] = true;
			if (_modelosTareas[paquete]->corregirFila(_tareasPublicaciones[id]->fila, id) == true) {
				_modelosTareas[paquete]->actualizarCampos(_tareasPublicaciones[id]->fila, registroTarea);
			}
		} else {
			solicitudSQL.exec("UPDATE tareas SET estado = " + QString::number(Estados::Error) + ", error = true WHERE (id = " + QString::number(id) + ")");
		}
		if (corregirFila(_paquetes[paquete]->fila, paquete) == true) {
			actualizarCampo(_paquetes[paquete]->fila, "error", true);
		} else {
			solicitudSQL.exec("UPDATE paquetes SET error = true WHERE (id = " + QString::number(paquete) + ")");
		}

		_tareasPublicaciones.remove(id);
		procesarColaEjecucion();

		return;
	}

	_tareasPublicaciones[id]->http.establecerMetodo(HTTP::Metodo::PONER);
	_tareasPublicaciones[id]->http.establecerAnfitrion(servidorS3);
	_tareasPublicaciones[id]->http.establecerPuerto(puertoServidorS3);
	_tareasPublicaciones[id]->http.establecerRuta(rutaHTTP);

	_tareasPublicaciones[id]->archivo.setFileName(_tareasPublicaciones[id]->ruta);
	_tareasPublicaciones[id]->archivo.setPermissions(QFileDevice::ReadUser);
	_tareasPublicaciones[id]->archivo.open(QIODevice::ReadOnly);
	_tareasPublicaciones[id]->http.establecerOrigenCarga(&_tareasPublicaciones[id]->archivo);

	_tareasPublicaciones[id]->http.agregarCabeceraSolicitud("Authorization", autorizacion);
	_tareasPublicaciones[id]->http.agregarCabeceraSolicitud("Content-Type", "application/octet-stream");
	_tareasPublicaciones[id]->http.agregarCabeceraSolicitud("Content-Length", QString::number(_tareasPublicaciones[id]->tamano));
	_tareasPublicaciones[id]->http.agregarCabeceraSolicitud("Host", servidorS3);
	_tareasPublicaciones[id]->http.agregarCabeceraSolicitud("User-Agent", _configuraciones.value("avanzadas/agenteUsuario", _agenteUsuarioTodus).toString() + " HTTP-Upload");

	connect(&_tareasPublicaciones[id]->http, &HTTP::cabecerasRecibidas, this, &ModeloPaquetes::eventoTareaIniciada);
	connect(&_tareasPublicaciones[id]->http, &HTTP::bytesCargaTransferidos, this, &ModeloPaquetes::eventoTareaTotalBytesTransferidos);
	connect(&_tareasPublicaciones[id]->http, &HTTP::detenido, this, &ModeloPaquetes::eventoTareaDetenida);
	connect(&_tareasPublicaciones[id]->http, &HTTP::detenidoParaReiniciar, this, &ModeloPaquetes::eventoTareaDetenidaParaReiniciar);
	connect(&_tareasPublicaciones[id]->http, &HTTP::finalizado, this, &ModeloPaquetes::eventoTareaFinalizada);

	_tareasPublicaciones[id]->http.setTransferTimeout(125000);
	_tareasPublicaciones[id]->http.establecerId(id);
	_tareasPublicaciones[id]->http.ejecutar();
}

void ModeloPaquetes::procesarColaEjecucion() {
	int totalPublicacionesParalelas = _configuraciones.value("publicaciones/paralelas", 1).toInt() - _tareasPublicaciones.size();
	int totalDescargasParalelas = _configuraciones.value("descargas/paralelas", 1).toInt() - _tareasDescargas.size();
	QSqlQuery solicitudSQL;
	qint64 idPaquete = 0;
	int tipo = Tipos::Publicacion;
	int formato = Formatos::S3;
	int clasificacion = Clasificaciones::Archivo;

	if (totalPublicacionesParalelas > 0) {
		solicitudSQL.exec("SELECT paquetes.id, paquetes.tipo, paquetes.formato, paquetes.clasificacion, paquetes.categoria, paquetes.tamano, paquetes.tamanoTransferido, paquetes.completado, tareas.id, tareas.ruta, tareas.tamano FROM paquetes LEFT JOIN tareas ON (tareas.paquete = paquetes.id) WHERE (paquetes.tipo = " + QString::number(Tipos::Publicacion) + " AND tareas.estado = " + QString::number(Estados::EnEsperaIniciar) + ") ORDER BY tareas.id LIMIT " + QString::number(totalPublicacionesParalelas));
		while (solicitudSQL.next() == true) {
			idPaquete = solicitudSQL.value("paquetes.id").toLongLong();
			tipo = solicitudSQL.value("paquetes.tipo").toInt();
			formato = solicitudSQL.value("paquetes.formato").toInt();
			clasificacion = solicitudSQL.value("paquetes.clasificacion").toInt();

			if (_paquetes.find(idPaquete) == _paquetes.end()) {
				if (tipo == Tipos::Publicacion) {
					emitirRegistro(TiposRegistro::Informacion, "PAQUETES") << "[" << idPaquete << "] Paquete iniciado. Tipo: Publicacion. Formato: " << (formato == Formatos::S3 ? "S3" : "Clasico") << std::endl;
				} else {
					emitirRegistro(TiposRegistro::Informacion, "PAQUETES") << "[" << idPaquete << "] Paquete iniciado. Tipo: Descarga. Formato: " << (formato == Formatos::S3 ? "S3" : "Clasico") << std::endl;
				}

				_paquetes.insert(idPaquete, new Paquete());
				_paquetes[idPaquete]->id = idPaquete;
				_paquetes[idPaquete]->fila = 0;
				_paquetes[idPaquete]->tipo = tipo;
				_paquetes[idPaquete]->formato = formato;
				_paquetes[idPaquete]->clasificacion = clasificacion;
				_paquetes[idPaquete]->categoria = solicitudSQL.value("paquetes.categoria").toInt();
				_paquetes[idPaquete]->tamano = solicitudSQL.value("paquetes.tamano").toLongLong();
				_paquetes[idPaquete]->tamanoTransferido = solicitudSQL.value("paquetes.tamanoTransferido").toLongLong();
				_paquetes[idPaquete]->completado = solicitudSQL.value("paquetes.completado").toInt();
				_paquetes[idPaquete]->velocidad = 0;
				_paquetes[idPaquete]->temporizadorActualizacionCampos.setInterval(1000);
				connect(&_paquetes[idPaquete]->temporizadorActualizacionCampos, &QTimer::timeout, [this, idPaquete]() { eventoPaqueteActualizarCampos(idPaquete); });
				_paquetes[idPaquete]->temporizadorActualizacionCampos.start();
			}

			_toDus.obtenerEnlaceFirmado(Tipos::Publicacion, clasificacion, idPaquete, solicitudSQL.value("tareas.id").toLongLong(), "", solicitudSQL.value("tareas.tamano").toLongLong());
		}
	}

	if (totalDescargasParalelas > 0) {
		solicitudSQL.exec("SELECT paquetes.id, paquetes.tipo, paquetes.formato, paquetes.categoria, paquetes.tamano, paquetes.tamanoTransferido, paquetes.completado, tareas.id, tareas.enlace FROM paquetes LEFT JOIN tareas ON (tareas.paquete = paquetes.id) WHERE (paquetes.tipo = " + QString::number(Tipos::Descarga) + " AND tareas.estado = " + QString::number(Estados::EnEsperaIniciar) + ") ORDER BY tareas.id LIMIT " + QString::number(totalDescargasParalelas));
		while (solicitudSQL.next() == true) {
			idPaquete = solicitudSQL.value("paquetes.id").toLongLong();
			tipo = solicitudSQL.value("paquetes.tipo").toInt();
			formato = solicitudSQL.value("paquetes.formato").toInt();

			if (_paquetes.find(idPaquete) == _paquetes.end()) {
				if (tipo == Tipos::Publicacion) {
					emitirRegistro(TiposRegistro::Informacion, "PAQUETES") << "[" << idPaquete << "] Paquete iniciado. Tipo: Publicacion. Formato: " << (formato == Formatos::S3 ? "S3" : "Clasico") << std::endl;
				} else {
					emitirRegistro(TiposRegistro::Informacion, "PAQUETES") << "[" << idPaquete << "] Paquete iniciado. Tipo: Descarga. Formato: " << (formato == Formatos::S3 ? "S3" : "Clasico") << std::endl;
				}

				_paquetes.insert(idPaquete, new Paquete());
				_paquetes[idPaquete]->id = idPaquete;
				_paquetes[idPaquete]->fila = 0;
				_paquetes[idPaquete]->tipo = tipo;
				_paquetes[idPaquete]->formato = formato;
				_paquetes[idPaquete]->clasificacion = Clasificaciones::Archivo;
				_paquetes[idPaquete]->categoria = solicitudSQL.value("paquetes.categoria").toInt();
				_paquetes[idPaquete]->tamano = solicitudSQL.value("paquetes.tamano").toLongLong();
				_paquetes[idPaquete]->tamanoTransferido = solicitudSQL.value("paquetes.tamanoTransferido").toLongLong();
				_paquetes[idPaquete]->completado = solicitudSQL.value("paquetes.completado").toInt();
				_paquetes[idPaquete]->velocidad = 0;
				_paquetes[idPaquete]->temporizadorActualizacionCampos.setInterval(1000);
				connect(&_paquetes[idPaquete]->temporizadorActualizacionCampos, &QTimer::timeout, [this, idPaquete]() { eventoPaqueteActualizarCampos(idPaquete); });
				_paquetes[idPaquete]->temporizadorActualizacionCampos.start();
			}

			_toDus.obtenerEnlaceFirmado(Tipos::Descarga, Clasificaciones::Archivo, idPaquete, solicitudSQL.value("tareas.id").toInt(), solicitudSQL.value("tareas.enlace").toString());
		}
	}
}

void ModeloPaquetes::eventoPaqueteActualizarCampos(qint64 idPaquete) {
	if (_paquetes.find(idPaquete) != _paquetes.end()) {
		QSqlQuery solicitudSQL;
		QVariantMap campos;
		int totalTareas = 0;
		bool error = false;

		solicitudSQL.exec("SELECT COUNT(id) AS totalTareas, SUM(tamano) AS tamano, SUM(tamanoTransferido) AS tamanoTransferido, SUM(completado) AS completado, SUM(velocidad) AS velocidad FROM tareas WHERE (paquete = " + QString::number(idPaquete) + ")");
		if (solicitudSQL.next() == true) {
			totalTareas = solicitudSQL.value("totalTareas").toInt();
			if (_paquetes[idPaquete]->tipo == Tipos::Descarga) {
				_paquetes[idPaquete]->tamano = solicitudSQL.value("tamano").toLongLong();
				campos["tamano"] = _paquetes[idPaquete]->tamano;
			}
			_paquetes[idPaquete]->tamanoTransferido = solicitudSQL.value("tamanoTransferido").toLongLong();
			campos["tamanoTransferido"] = _paquetes[idPaquete]->tamanoTransferido;
			if (totalTareas > 0) {
				_paquetes[idPaquete]->completado = solicitudSQL.value("completado").toInt() / totalTareas;
				campos["completado"] = _paquetes[idPaquete]->completado;
				if (_paquetes[idPaquete]->completado == 100) {
					campos["estado"] = Estados::Finalizado;
				}
			}
			_paquetes[idPaquete]->velocidad = solicitudSQL.value("velocidad").toInt();
			campos["velocidad"] = _paquetes[idPaquete]->velocidad;

			if (corregirFila(_paquetes[idPaquete]->fila, idPaquete) == true) {
				actualizarCampos(_paquetes[idPaquete]->fila, campos);
			} else {
				if (_paquetes[idPaquete]->completado == 100) {
					solicitudSQL.exec("UPDATE paquetes SET estado = " + QString::number(Estados::Finalizado) + ", tamano = " + QString::number(_paquetes[idPaquete]->tamano) + ", tamanoTransferido = " + QString::number(_paquetes[idPaquete]->tamanoTransferido) + ", completado = " + QString::number(_paquetes[idPaquete]->completado) + ", velocidad = " + QString::number(_paquetes[idPaquete]->velocidad) + " WHERE (paquete = " + QString::number(idPaquete) + ")");
				} else {
					solicitudSQL.exec("UPDATE paquetes SET tamano = " + QString::number(_paquetes[idPaquete]->tamano) + ", tamanoTransferido = " + QString::number(_paquetes[idPaquete]->tamanoTransferido) + ", completado = " + QString::number(_paquetes[idPaquete]->completado) + ", velocidad = " + QString::number(_paquetes[idPaquete]->velocidad) + " WHERE (paquete = " + QString::number(idPaquete) + ")");
				}
			}
		}

		solicitudSQL.exec(QString("SELECT COUNT(id) AS totalTareas FROM tareas WHERE (paquete = %1 AND estado = %2)").arg(idPaquete).arg(Estados::Error));
		if (solicitudSQL.next() == true) {
			if (solicitudSQL.value("totalTareas").toInt() == totalTareas) {
				if (corregirFila(_paquetes[idPaquete]->fila, idPaquete) == true) {
					actualizarCampo(_paquetes[idPaquete]->fila, "estado", Estados::Error);
				} else {
					solicitudSQL.exec(QString("UPDATE paquetes SET estado = %2 WHERE (paquete = %1)").arg(idPaquete).arg(Estados::Error));
				}
				error = true;
			}
		}

		solicitudSQL.exec(QString("SELECT COUNT(id) AS totalTareas FROM tareas WHERE (paquete = %1 AND (estado = %2 OR estado = %3))").arg(idPaquete).arg(Estados::EnEsperaIniciar).arg(Estados::Iniciado));
		if (solicitudSQL.next() == true) {
			if (solicitudSQL.value("totalTareas").toInt() == 0) {
				if (_paquetes[idPaquete]->completado != 100 && error == false) {
					if (corregirFila(_paquetes[idPaquete]->fila, idPaquete) == true) {
						actualizarCampo(_paquetes[idPaquete]->fila, "estado", Estados::Pausado);
					} else {
						solicitudSQL.exec(QString("UPDATE paquetes SET estado = %2 WHERE (paquete = %1)").arg(idPaquete).arg(Estados::Pausado));
					}

					emitirRegistro(TiposRegistro::Informacion, "PAQUETES") << "[" << idPaquete << "] Paquete pausado" << std::endl;
				}

				if (_paquetes[idPaquete]->completado == 100 && error == false) {
					emitirRegistro(TiposRegistro::Informacion, "PAQUETES") << "[" << idPaquete << "] Paquete finalizado. Transferido: " << _utiles->representarTamano(_paquetes[idPaquete]->tamano).toStdString() << std::endl;

					if (_paquetes[idPaquete]->tipo == Tipos::Publicacion) {
						if (_configuraciones.value("publicaciones/generarArchivosDescargaAlFinalizar", true).toBool() == true) {
							generarArchivoDescarga(idPaquete);
						}
					} else {
						if (_configuraciones.value("descargas/eliminarDelListadoAlFinalizar", false).toBool() == true) {
							solicitudSQL.exec(QString("DELETE FROM tareas WHERE (paquete = %1)").arg(idPaquete));
							solicitudSQL.exec(QString("DELETE FROM paquetes WHERE (id = %1)").arg(idPaquete));

							select();
						}
					}

					if (_paquetes[idPaquete]->tipo == Tipos::Publicacion) {
						_utiles->notificar("FinalizacionExitosaPaquete", false, "Paquete de publicación finalizado exitosamente", "Título: " + _paquetes[idPaquete]->nombre + "\nTotal transferido: " + _utiles->representarTamano(_paquetes[idPaquete]->tamanoTransferido), "finalizacion-exitosa-paquete.mp3");
					} else {
						_utiles->notificar("FinalizacionExitosaPaquete", false, "Paquete de descarga finalizado exitosamente", "Título: " + _paquetes[idPaquete]->nombre + "\nTotal transferido: " + _utiles->representarTamano(_paquetes[idPaquete]->tamanoTransferido), "finalizacion-exitosa-paquete.mp3");
					}
				}

				if (error == true) {
					if (_paquetes[idPaquete]->tipo == Tipos::Publicacion) {
						_utiles->notificar("FinalizacionErroneaPaquete", true, "Paquete de publicación detenido con error", "Título: " + _paquetes[idPaquete]->nombre, "finalizacion-erronea-paquete.mp3");
					} else {
						_utiles->notificar("FinalizacionErroneaPaquete", true, "Paquete de descarga detenido con error", "Título: " + _paquetes[idPaquete]->nombre, "finalizacion-erronea-paquete.mp3");
					}
				}

				_paquetes[idPaquete]->temporizadorActualizacionCampos.stop();
				_paquetes.remove(idPaquete);
			}
		}
	}
}

void ModeloPaquetes::eventoTareaIniciada(qint64 id) {
	QPointer<Tarea> tarea;
	bool publicacion = false;

	if (_tareasPublicaciones.find(id) != _tareasPublicaciones.end()) {
		tarea = _tareasPublicaciones[id];
		publicacion = true;
	} else {
		tarea = _tareasDescargas[id];
	}

	if (tarea) {
		if (tarea->http.error() == false) {
			if (publicacion == false) {
				if (tarea->tamano == 0) {
					if (tarea->tamanoTransferido > 0) {
						tarea->tamano = tarea->tamanoTransferido + tarea->http.cabeceraRespuesta("content-length").toLongLong();
					} else {
						tarea->tamano = tarea->http.cabeceraRespuesta("content-length").toLongLong();
					}
				}

				if (_modelosTareas[tarea->paquete]) {
					if (_modelosTareas[tarea->paquete]->corregirFila(tarea->fila, id) == true) {
						_modelosTareas[tarea->paquete]->actualizarCampo(tarea->fila, "tamano", tarea->tamano);
					}
				} else {
					QSqlQuery solicitudSQL;

					solicitudSQL.exec(QString("UPDATE tareas SET tamano = %2 WHERE (id = %1)").arg(id).arg(tarea->tamano));
				}
			}

			if (_paquetes[tarea->paquete]->tipo == Tipos::Publicacion) {
				emitirRegistro(TiposRegistro::Informacion, "TAREAS") << "[" << id << "] Tarea iniciada. Tipo: Publicacion. Tamano: " << _utiles->representarTamano(tarea->tamano).toStdString() << std::endl;
			}
			if (_paquetes[tarea->paquete]->tipo == Tipos::Descarga) {
				emitirRegistro(TiposRegistro::Informacion, "TAREAS") << "[" << id << "] Tarea iniciada. Tipo: Descarga. Tamano: " << _utiles->representarTamano(tarea->tamano).toStdString() << std::endl;
			}

			QMetaObject::invokeMethod(_qmlRaiz, "visualizarTareaIniciada", Qt::QueuedConnection, Q_ARG(QVariant, _paquetes[tarea->paquete]->categoria), Q_ARG(QVariant, tarea->paquete), Q_ARG(QVariant, _paquetes[tarea->paquete]->fila), Q_ARG(QVariant, tarea->fila));

			if (publicacion == false) {
				_utiles->notificar("InicializacionTarea", false, "Tarea de publicación iniciada", "Nombre: " + tarea->nombre + "\nRuta: " + tarea->ruta + "\nTamaño: " +  _utiles->representarTamano(tarea->tamano), "incializacion-tarea.mp3");
			} else {
				_utiles->notificar("InicializacionTarea", false, "Tarea descarga iniciada", "Nombre: " + tarea->nombre + "\nTamaño: " +  _utiles->representarTamano(tarea->tamano), "incializacion-tarea.mp3");
			}
		}
	}
}

void ModeloPaquetes::eventoTareaTotalBytesTransferidos(qint64 id, qint64 transferido) {
	QPointer<Tarea> tarea;

	if (_tareasPublicaciones.find(id) != _tareasPublicaciones.end()) {
		tarea = _tareasPublicaciones[id];
	} else {
		tarea = _tareasDescargas[id];
	}

	if (tarea) {
		QSqlQuery solicitudSQL;
		QVariantMap campos;

		if (tarea->tamano > 0) {
			tarea->completado = (int)((transferido * 100) / tarea->tamano);
			if (tarea->completado < 0) {
				tarea->completado = 0;
			}
			if (tarea->completado > 100) {
				tarea->completado = 100;
			}
		}
		tarea->velocidad = (transferido - tarea->tamanoTransferido);
		if (tarea->velocidad < 0) {
			tarea->velocidad = 0;
		}
		tarea->tamanoTransferido = transferido;
		tarea->ultimoTiempoTransferencia = std::time(nullptr);

		if (_modelosTareas[tarea->paquete]) {
			campos["tamanoTransferido"] = tarea->tamanoTransferido;
			campos["completado"] = tarea->completado;
			campos["velocidad"] = tarea->velocidad;
			if (_modelosTareas[tarea->paquete]->corregirFila(tarea->fila, id) == true) {
				_modelosTareas[tarea->paquete]->actualizarCampos(tarea->fila, campos);
			}
		} else {
			solicitudSQL.exec(QString("UPDATE tareas SET tamanoTransferido = %2, completado = %3, velocidad %4 WHERE (id = %1)").arg(id).arg(tarea->tamanoTransferido).arg(tarea->completado).arg(tarea->velocidad));
		}
	}
}

void ModeloPaquetes::eventoTareaDetenida(qint64 id) {
	QPointer<Tarea> tarea;
	bool publicacion = false;

	if (_tareasPublicaciones.find(id) != _tareasPublicaciones.end()) {
		tarea = _tareasPublicaciones[id];
		publicacion = true;
	} else {
		tarea = _tareasDescargas[id];
	}

	if (tarea) {
		QVariantMap campos;

		tarea->archivo.close();
		tarea->tamanoTransferido = tarea->http.bytesDescargados();
		if (tarea->tamano > 0) {
			tarea->completado = (int)((tarea->tamanoTransferido * 100) / tarea->tamano);
			if (tarea->completado < 0) {
				tarea->completado = 0;
			}
			if (tarea->completado > 100) {
				tarea->completado = 100;
			}
		}
		tarea->velocidad = 0;
		campos["estado"] = Estados::Pausado;
		campos["tamanoTransferido"] = tarea->tamanoTransferido;
		campos["completado"] = tarea->completado;
		campos["velocidad"] = 0;
		campos["error"] = false;

		if (tarea->http.error() == false) {
			if (_modelosTareas[tarea->paquete]) {
				if (_modelosTareas[tarea->paquete]->corregirFila(tarea->fila, id) == true) {
					_modelosTareas[tarea->paquete]->actualizarCampos(tarea->fila, campos);
				}
			} else {
				QSqlQuery solicitudSQL;

				solicitudSQL.exec("UPDATE tareas SET estado = " + QString::number(Estados::Pausado) + ", tamanoTransferido = " + QString::number(tarea->tamanoTransferido) + ", completado = " + QString::number(tarea->completado) + ", veloidad = 0, error = false WHERE (id = " + QString::number(id) + ")");
			}

			emitirRegistro(TiposRegistro::Informacion, "TAREAS") << "[" << id << "] Tarea pausada. Transferido: " << _utiles->representarTamano(tarea->tamanoTransferido).toStdString() << ". Completado: " << tarea->completado << "%" << std::endl;
		} else {
			if (_configuraciones.value("publicaciones/reintentarTareasErroneas", false) == false || tarea->http.codigoHTTP() == 404) {
				if (_modelosTareas[tarea->paquete]) {
					campos["estado"] = Estados::Error;
					campos["error"] = true;
					if (_modelosTareas[tarea->paquete]->corregirFila(tarea->fila, id) == true) {
						_modelosTareas[tarea->paquete]->actualizarCampos(tarea->fila, campos);
					}
				} else {
					QSqlQuery solicitudSQL;

					solicitudSQL.exec("UPDATE tareas SET estado = " + QString::number(Estados::Error) + ", velocidad = 0, error = true WHERE (id = " + QString::number(id) + ")");
				}

				if (corregirFila(_paquetes[tarea->paquete]->fila, tarea->paquete) == true) {
					actualizarCampo(_paquetes[tarea->paquete]->fila, "error", true);
				} else {
					QSqlQuery solicitudSQL;

					solicitudSQL.exec("UPDATE paquetes SET error = true WHERE (id = " + QString::number(tarea->paquete) + ")");
				}

				if (publicacion == true) {
					emitirRegistro(TiposRegistro::Informacion, "TAREAS") << "[" << id << "] Tarea de publicación detenida con error" << std::endl;
					_utiles->notificar("FinalizacionErroneaTarea", true, "Tarea de publicación detenida con error", "Nombre: " + tarea->nombre + "\nRuta: " + tarea->ruta, "finalizacion-erronea-tarea.mp3");
				} else {
					emitirRegistro(TiposRegistro::Informacion, "TAREAS") << "[" << id << "] Tarea de descarga detenida con error" << std::endl;
					_utiles->notificar("FinalizacionErroneaTarea", true, "Tarea de descarga detenida con error", "Nombre: " + tarea->nombre, "finalizacion-erronea-tarea.mp3");
				}
			} else {
				if (_modelosTareas[tarea->paquete]) {
					campos["estado"] = Estados::EnEsperaIniciar;
					campos["tamanoTransferido"] = 0;
					campos["completado"] = 0;
					campos["velocidad"] = 0;
					campos["error"] = false;
					if (_modelosTareas[tarea->paquete]->corregirFila(tarea->fila, id) == true) {
						_modelosTareas[tarea->paquete]->actualizarCampos(tarea->fila, campos);
					}
				} else {
					QSqlQuery solicitudSQL;

					solicitudSQL.exec("UPDATE tareas SET estado = " + QString::number(Estados::EnEsperaIniciar) + ", tamanoTransferido = 0, completado = 0, velocidad = 0, error = true WHERE (id = " + QString::number(id) + ")");
				}
			}
		}

		eventoPaqueteActualizarCampos(tarea->paquete);
	}

	if (publicacion == true) {
		_tareasPublicaciones.remove(id);
	} else {
		_tareasDescargas.remove(id);
	}

	procesarColaEjecucion();

}

void ModeloPaquetes::eventoTareaDetenidaParaReiniciar(qint64 id) {
	Tarea *tarea;
	bool publicacion = false;

	if (_tareasPublicaciones.find(id) != _tareasPublicaciones.end()) {
		tarea = _tareasPublicaciones[id];
		publicacion = true;
	} else {
		tarea = _tareasDescargas[id];
	}

	if (tarea) {
		tarea->archivo.close();

		if (publicacion == true) {
			tarea->tamanoTransferido = 0;
		} else {
			tarea->tamanoTransferido = tarea->http.bytesDescargados();
		}

		if (tarea->tamano > 0) {
			tarea->completado = (int)((tarea->tamanoTransferido * 100) / tarea->tamano);
			if (tarea->completado < 0) {
				tarea->completado = 0;
			}
			if (tarea->completado > 100) {
				tarea->completado = 100;
			}
		}
		tarea->velocidad = 0;

		if (_modelosTareas[tarea->paquete]) {
			QVariantMap campos;

			campos["estado"] = Estados::EnEsperaIniciar;
			campos["tamanoTransferido"] = tarea->tamanoTransferido;
			campos["completado"] = tarea->completado;
			campos["velocidad"] = 0;
			if (_modelosTareas[tarea->paquete]->corregirFila(tarea->fila, id) == true) {
				_modelosTareas[tarea->paquete]->actualizarCampos(tarea->fila, campos);
			}
		} else {
			QSqlQuery solicitudSQL;

			solicitudSQL.exec(QString("UPDATE tareas SET estado = %2, tamanoTransferido = %3, completado = %4, velocidad = 0 WHERE (id = %1)").arg(id).arg(Estados::EnEsperaIniciar).arg(tarea->tamanoTransferido).arg(tarea->completado));
		}

		emitirRegistro(TiposRegistro::Informacion, "TAREAS") << "[" << id << "] Error detectado en la transferencia. Reanudando" << std::endl;

		if (publicacion == true) {
			_tareasPublicaciones.remove(id);
		} else {
			_tareasDescargas.remove(id);
		}

		procesarColaEjecucion();
	}
}

void ModeloPaquetes::eventoTareaFinalizada(qint64 id) {
	QPointer<Tarea> tarea;
	bool publicacion = false;

	if (_tareasPublicaciones.find(id) != _tareasPublicaciones.end()) {
		tarea = _tareasPublicaciones[id];
		publicacion = true;
	} else {
		tarea = _tareasDescargas[id];
	}

	if (tarea) {
		tarea->archivo.close();

		if (tarea->http.error() == false) {
			emitirRegistro(TiposRegistro::Informacion, "TAREAS") << "[" << id << "] Tarea finalizada. Transferido: " << _utiles->representarTamano(tarea->tamano).toStdString() << std::endl;

			tarea->tamanoTransferido = tarea->tamano;
			tarea->completado = 100;
			tarea->velocidad = 0;

			if (_modelosTareas[tarea->paquete]) {
				QVariantMap campos;

				campos["estado"] = Estados::Finalizado;
				campos["tamanoTransferido"] = tarea->tamanoTransferido;
				campos["completado"] = 100;
				campos["velocidad"] = 0;
				if (_modelosTareas[tarea->paquete]->corregirFila(tarea->fila, id) == true) {
					_modelosTareas[tarea->paquete]->actualizarCampos(tarea->fila, campos);
				}
			} else {
				QSqlQuery solicitudSQL;

				solicitudSQL.exec(QString("UPDATE tareas SET estado = %2, tamanoTransferido = %3, completado = %4, velocidad = %5 WHERE (id = %1)").arg(id).arg(Estados::Finalizado).arg(tarea->tamanoTransferido).arg(tarea->completado).arg(tarea->velocidad));
			}

			if (_paquetes[tarea->paquete]->tipo == Tipos::Publicacion) {
				generarArchivoDescarga(tarea->paquete);
			}

			if (publicacion == true) {
				_utiles->notificar("FinalizacionExitosaTarea", false, "Tarea de publicación finalizada exitosamente", "Nombre: " + tarea->nombre + "\nRuta: " + tarea->ruta + "\nTotal transferido: " + _utiles->representarTamano(tarea->tamanoTransferido), "finalizacion-exitosa-tarea.mp3");
			} else {
				_utiles->notificar("FinalizacionExitosaTarea", false, "Tarea de descarga finalizada exitosamente", "Nombre: " + tarea->nombre + "\nTotal transferido: " + _utiles->representarTamano(tarea->tamanoTransferido), "finalizacion-exitosa-tarea.mp3");
			}

			eventoPaqueteActualizarCampos(tarea->paquete);
		} else {
			emitirRegistro(TiposRegistro::Informacion, "TAREAS") << "[" << id << "] Tarea pausada con error" << std::endl;

			if (_modelosTareas[tarea->paquete]) {
				QVariantMap campos;

				campos["estado"] = Estados::Error;
				campos["velocidad"] = 0;
				campos["error"] = true;
				if (_modelosTareas[tarea->paquete]->corregirFila(tarea->fila, id) == true) {
					_modelosTareas[tarea->paquete]->actualizarCampos(tarea->fila, campos);
				}
			} else {
				QSqlQuery solicitudSQL;

				solicitudSQL.exec(QString("UPDATE tareas SET estado = %2, velocidad = 0, error = true WHERE (id = %1)").arg(id).arg(Estados::Error));
			}

			if (corregirFila(_paquetes[tarea->paquete]->fila, tarea->paquete) == true) {
				actualizarCampo(_paquetes[tarea->paquete]->fila, "error", true);
			} else {
				QSqlQuery solicitudSQL;

				solicitudSQL.exec(QString("UPDATE paquetes SET error = true WHERE (id = %1)").arg(tarea->paquete));
			}

			if (publicacion == true) {
				_utiles->notificar("FinalizacionErroneaTarea", true, "Tarea de publicación detenida con error", "Nombre: " + tarea->nombre + "\nRuta: " + tarea->ruta, "finalizacion-erronea-tarea.mp3");
			} else {
				_utiles->notificar("FinalizacionErroneaTarea", true, "Tarea des descarga detenida con error", "Nombre: " + tarea->nombre, "finalizacion-erronea-tarea.mp3");
			}

			eventoPaqueteActualizarCampos(tarea->paquete);
		}
	}

	if (publicacion == true) {
		_tareasPublicaciones.remove(id);
	} else {
		_tareasDescargas.remove(id);
	}

	procesarColaEjecucion();
}

void ModeloPaquetes::generarArchivoDescarga(qint64 paquete) {
	QVariantMap registroPaquete = obtenerDesdeId(paquete);

	switch (registroPaquete["formato"].toInt()) {
		case Formatos::S3:
			generarArchivoDescargaS3(registroPaquete);
			break;
		case Formatos::Clasico:
			generarArchivoDescargaClasico(registroPaquete);
			break;
	}
}

void ModeloPaquetes::generarArchivoDescargaS3(const QVariantMap &registroPaquete) {
	Q_UNUSED(registroPaquete);
	emitirRegistro(TiposRegistro::Informacion, "PAQUETES") << "[" << registroPaquete["id"].toLongLong() << "] Generando archivo de descarga. Formato: S3" << std::endl;

}

void ModeloPaquetes::generarArchivoDescargaClasico(const QVariantMap &registroPaquete) {
	QSqlQuery solicitudSQL;
	QString rutaArchivo;
	QDir directorioDescarga;
	QFile archivo;
	QByteArray datos;

	emitirRegistro(TiposRegistro::Informacion, "PAQUETES") << "[" << registroPaquete["id"].toLongLong() << "] Generando archivo de descarga. Formato: Clasico" << std::endl;

	solicitudSQL.exec(QString("SELECT nombre, enlace FROM tareas WHERE (paquete = %1 AND estado = %2)").arg(registroPaquete["id"].toLongLong()).arg(Estados::Finalizado));
	while (solicitudSQL.next() == true) {
		datos += solicitudSQL.value("enlace").toByteArray() + "\t" + QByteArray::fromPercentEncoding(solicitudSQL.value("nombre").toByteArray()) + "\n";
	}

	rutaArchivo = QString("%1/Publicaciones").arg(QUrl(_configuraciones.value("descargas/ruta", _rutaDescargaPredeterminada).toString()).toLocalFile());
	if (directorioDescarga.exists(rutaArchivo) == false) {
		directorioDescarga.mkpath(rutaArchivo);
	}
	rutaArchivo += "/" + QByteArray::fromPercentEncoding(registroPaquete["nombre"].toByteArray()) + ".txt";
	archivo.setFileName(rutaArchivo);
	if (archivo.open(QIODevice::WriteOnly) == true) {
		archivo.write(datos);
		archivo.close();
	}
}

void ModeloPaquetes::iniciarSesionToDus() {
	QTimer::singleShot(1000, &_toDus, &toDus::iniciarSesion);
}

QString ModeloPaquetes::obtenerFiltroCategoria(int categoria) {
	QString filtros = "categoria = " + QString::number(categoria);

	_categoria = categoria;

	if (_categoria < 4) {
		switch (_categoria) {
			case 1:
				filtros = "tipo = " + QString::number(Tipos::Descarga);
				break;
			case 2:
				filtros = "tipo = " + QString::number(Tipos::Publicacion);
				break;
			case 3:
				filtros = "estado = " + QString::number(Estados::Finalizado);
				break;
		}
	}

	return filtros;
}

void ModeloPaquetes::agregarPaqueteAlListado(qint64 id, int fila) {
	if (_paquetes.find(id) == _paquetes.end()) {
		QVariantMap registroPaquete = obtenerDesdeId(id);
		int tipo = registroPaquete["tipo"].toInt();
		int categoria = registroPaquete["categoria"].toInt();
		int formato = registroPaquete["formato"].toInt();
		qint64 tamano = registroPaquete["tamano"].toLongLong();
		qint64 tamanoTransferido = registroPaquete["tamanoTransferido"].toLongLong();
		int completado = registroPaquete["completado"].toInt();
		QSqlQuery solicitudSQL;

		solicitudSQL.exec("UPDATE tareas SET estado = " + QString::number(Estados::EnEsperaIniciar) + ", error = false WHERE (paquete = " + QString::number(id) + " AND (estado = " + QString::number(Estados::Pausado) + " OR estado = " + QString::number(Estados::Error) + "))");

		_paquetes.insert(id, new Paquete());
		_paquetes[id]->id = id;
		_paquetes[id]->fila = fila;
		_paquetes[id]->tipo = tipo;
		_paquetes[id]->categoria = categoria;
		_paquetes[id]->formato = formato;
		_paquetes[id]->nombre = QByteArray::fromPercentEncoding(registroPaquete["nombre"].toByteArray());
		_paquetes[id]->tamano = tamano;
		_paquetes[id]->tamanoTransferido = tamanoTransferido;
		_paquetes[id]->completado = completado;
		_paquetes[id]->velocidad = 0;
		_paquetes[id]->temporizadorActualizacionCampos.setInterval(1000);
		connect(&_paquetes[id]->temporizadorActualizacionCampos, &QTimer::timeout, [this, id]() { eventoPaqueteActualizarCampos(id); });
		_paquetes[id]->temporizadorActualizacionCampos.start();

		if (tipo == Tipos::Publicacion) {
			emitirRegistro(TiposRegistro::Informacion, "PAQUETES") << "[" << id << "] Paquete iniciado. Tipo: Publicacion. Formato: " << (formato == Formatos::S3 ? "S3" : "Clasico") << ". Tamano: " << tamano << std::endl;
			_utiles->notificar("InicializacionPaquete", false, "Paquete de publicación iniciado", "Título: " + _paquetes[id]->nombre + "\nTamaño: " + QString::number(_paquetes[id]->tamano) + " B", "incializacion-paquete.mp3");
		} else {
			emitirRegistro(TiposRegistro::Informacion, "PAQUETES") << "[" << id << "] Paquete iniciado. Tipo: Descarga. Formato: " << (formato == Formatos::S3 ? "S3" : "Clasico") << std::endl;
			_utiles->notificar("InicializacionPaquete", false, "Paquete de descarga iniciado", "Título: " + _paquetes[id]->nombre + "\nTamaño: " + QString::number(_paquetes[id]->tamano) + " B", "incializacion-paquete.mp3");
		}
	}
}

void ModeloPaquetes::eliminarPaqueteDelListado(qint64 id) {
	QVariantMap registroPaquete = obtenerDesdeId(id);
	int tipo = registroPaquete["tipo"].toInt();
	QSqlQuery solicitudSQL;

	solicitudSQL.exec("UPDATE tareas SET estado = " + QString::number(Estados::Pausado) + " WHERE (paquete = " + QString::number(id) + " AND (estado = " + QString::number(Estados::EnEsperaIniciar) + " OR estado = " + QString::number(Estados::Iniciado) + "))");

	_toDus.eliminarTodasSolicitudesEnlaceFirmado(id);

	if (tipo == Tipos::Publicacion) {
		for (qint64 idTarea : _tareasPublicaciones.keys()) {
			if (_tareasPublicaciones[idTarea]) {
				if (_tareasPublicaciones[idTarea]->paquete == id) {
					_tareasPublicaciones[idTarea]->http.detener();
				}
			}
		}
	} else {
		for (qint64 idTarea : _tareasDescargas.keys()) {
			if (_tareasDescargas[idTarea]) {
				if (_tareasDescargas[idTarea]->paquete == id) {
					_tareasDescargas[idTarea]->http.detener();
				}
			}
		}
	}
}
/*
void ModeloPaquetes::agregarTareaAlListado(qint64 id, int fila) {}
void ModeloPaquetes::eliminarTareaDelListado(qint64 id) {}
*/
