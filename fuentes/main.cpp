#include "main.hpp"
#include "ventanaconfiguracion.hpp"
#include "ventanaprincipal.hpp"
#include <thread>
#include <QApplication>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>


/**
 * @brief Título de la aplicación
 */
QString _aplicacionTitulo;

/**
 * @brief Nombre corto de la aplicación
 */
QString _aplicacionNombreCorto;

/**
 * @brief Versión de la aplicación
 */
QString _aplicacionVersion;

/**
 * @brief Ruta en donde almacenar las descargas
 */
QString _rutaDescargas;

/**
 * @brief Sesión toDus
 */
QSharedPointer<toDus> _toDus;

/**
 * @brief Crea las configuraciones de la aplicación por defecto si no existen
 */
void crearConfiguracionesDefecto();

/**
 * @brief Obtiene la ruta en donde se almacenarán las descargas
 * @return La ruta en donde se almacenarán las descargas
 */
QString obtenerRutaDescargas();

/**
 * @brief Crea los directorios para alojar los archivos descargados
 */
void crearDirectoriosDescargas();


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	_aplicacionTitulo = "Administrador de Transferencias para toDus (S3)";
	_aplicacionNombreCorto = "atds3";
	_aplicacionVersion = "0.1";

	app.setOrganizationName(_aplicacionNombreCorto);
	app.setApplicationDisplayName(_aplicacionTitulo);
	app.setApplicationName(_aplicacionNombreCorto);
	app.setApplicationVersion(_aplicacionVersion);

	crearConfiguracionesDefecto();

	crearDirectoriosDescargas();

	VentanaPrincipal ventanaPrincipal;
	ventanaPrincipal.show();

	QSettings configuracion;

	if (configuracion.value("todus/fichaAcceso").toString().size() > 0 || configuracion.value("todus/telefono").toString().size() > 0) {
		_toDus->iniciarSesion();
	} else {
		ventanaPrincipal._ventanaConfiguracion->show();
	}

	return app.exec();
}

/**
 * @brief Crea las configuraciones de la aplicación por defecto si no existen
 */
void crearConfiguracionesDefecto() {
	QSettings configuracion;

	if (configuracion.value("descargas/ruta", "").toString().size() == 0) {
		configuracion.setValue("descargas/ruta", obtenerRutaDescargas());
	}
	if (configuracion.value("descargas/descargasParalelas", 0).toInt() == 0) {
		configuracion.setValue("descargas/descargasParalelas", 2);
	}
}

/**
 * @brief Obtiene la ruta en donde se almacenarán las descargas
 * @return La ruta en donde se almacenarán las descargas
 */
QString obtenerRutaDescargas() {
	QSettings configuracion;
	QDir directorioDescarga;
	QString rutaDescarga = configuracion.value("descargas/ruta").value<QString>();

	if (rutaDescarga.size() == 0) {
		rutaDescarga = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);

		if (rutaDescarga.size() == 0) {
			rutaDescarga = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
		}

		rutaDescarga += "/" + _aplicacionNombreCorto;
		directorioDescarga.mkdir(rutaDescarga);
	}

	return rutaDescarga;
}

/**
 * @brief Crea los directorios para alojar los archivos descargados
 */
void crearDirectoriosDescargas() {
	QDir directorioDescarga;
	QString rutaDescarga = obtenerRutaDescargas();
	QList<QString> directorios {"programas", "musica", "videos", "otros"};

	directorioDescarga.cd(rutaDescarga);
	for (const QString &directorio : directorios) {
		if (directorioDescarga.exists(directorio) == false) {
			directorioDescarga.mkdir(directorio);
		}
	}
}
