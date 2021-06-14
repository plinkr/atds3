#include "basedatos.hpp"
#include "main.hpp"
#include <QStandardPaths>
#include <QSqlDatabase>


BaseDatos::BaseDatos(QObject *padre) : QThread(padre) {}

void BaseDatos::run() {
	//inicializarBaseDatos();

	while (isInterruptionRequested() == false) {
		sleep(2);
	}
}

void BaseDatos::ejecutarInstruccion(const QString &instruccion, QSqlQuery *resultados) {
	QSqlQuery *solicitud = new QSqlQuery(instruccion);

	resultados = solicitud;
}

/**
 * @brief Inicializa la base de datos
 */
void BaseDatos::inicializarBaseDatos() {
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	QString archivoBaseDatos = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/" + _aplicacionNombreCorto.toLower() + "/" + _aplicacionNombreCorto + ".db";

	db.setDatabaseName(archivoBaseDatos);
	db.open();

	/**
	 * Crea las estructuras de las tablas
	 */
	QSqlQuery solicitudSQL;
	solicitudSQL.exec("CREATE TABLE IF NOT EXISTS entradas (id INTEGER PRIMARY KEY AUTOINCREMENT, estado INTEGER NOT NULL, nombre TEXT NOT NULL, completado INTEGER DEFAULT 0, velocidad INTEGER DEFAULT 0, ruta TEXT NOT NULL, categoria INTEGER NOT NULL, enlace TEXT NOT NULL, totalADescargar INTEGER DEFAULT 0, totalDescargado INTEGER DEFAULT 0)");
}
