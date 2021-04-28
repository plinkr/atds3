#include "main.hpp"
#include "ventanaprincipal.hpp"
#include <QApplication>
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
 * @brief Crea los directorios para alojar los archivos descargados
 */
void crearDirectoriosDescargas();


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	_aplicacionTitulo = "Descargador de Archivos de la Red Todus (S3)";
	_aplicacionNombreCorto = "DARTS3";
	_aplicacionVersion = "0.1";

	app.setApplicationName(_aplicacionTitulo);
	app.setApplicationName(_aplicacionNombreCorto);
	app.setApplicationVersion(_aplicacionVersion);

	crearDirectoriosDescargas();

	VentanaPrincipal vp;
	vp.show();

	return app.exec();
}

/**
 * @brief Crea los directorios para alojar los archivos descargados
 */
void crearDirectoriosDescargas() {
	QString rutaDescarga = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
	QDir directorioDescarga;

	if (rutaDescarga.size() == 0) {
		rutaDescarga = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	}

	directorioDescarga.cd(rutaDescarga);
	_rutaDescargas = rutaDescarga + "/" + _aplicacionNombreCorto.toLower();

	if (directorioDescarga.exists(_aplicacionNombreCorto.toLower()) == true) {
		return;
	}

	directorioDescarga.mkdir(_aplicacionNombreCorto.toLower());
	directorioDescarga.cd(_aplicacionNombreCorto.toLower());
	directorioDescarga.mkdir("programas");
	directorioDescarga.mkdir("musica");
	directorioDescarga.mkdir("videos");
	directorioDescarga.mkdir("otros");
}
