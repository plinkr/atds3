#include "main.hpp"
#include "ventanaprincipal.hpp"
#include <QApplication>


/**
 * @brief Título de la aplicación
 */
QString _aplicacionTitulo;

/**
 * @brief Versión de la aplicación
 */
QString _aplicacionVersion;


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	VentanaPrincipal vp;

	_aplicacionTitulo = "Descargador de Archivos de la Red Todus (S3)";
	_aplicacionVersion = "0.1";

	app.setApplicationName(_aplicacionTitulo);
	app.setApplicationVersion(_aplicacionVersion);

	vp.show();

	return app.exec();
}
