#ifndef MAIN_HPP
#define MAIN_HPP

#include <QSharedPointer>
#include <QSharedPointer>


struct sqlite3;
struct sqlite3_stmt;
class QNetworkProxy;
class QPalette;
class toDus;


/**
 * @brief Título de la aplicación
 */
extern QString _aplicacionTitulo;

/**
 * @brief Nombre corto de la aplicación
 */
extern QString _aplicacionNombreCorto;

/**
 * @brief Versión de la aplicación
 */
extern QString _aplicacionVersion;

extern QString _rutaBaseDatos;

/**
 * @brief Ruta en donde almacenar las descargas
 */
extern QString _rutaDescargas;

/**
 * @brief Agente de usuario de toDus
 */
extern QString _agenteUsuarioTodus;

/**
 * @brief Sesión toDus
 */
extern QSharedPointer<toDus> _toDus;

/**
 * @brief Listado de categorías
 */
enum _ListadoCategorias {
	Programas = 0x03,
	Musica = 0x04,
	Videos = 0x05,
	Otros = 0x06,
	Subidas = 0x07
};

/**
 * @brief Listado de estados
 */
enum _ListadoEstados {
	EnEsperaPausar = 0x01,
	Pausada = 0x02,
	EnEsperaIniciar = 0x03,
	Iniciada = 0x04,
	Finalizada = 0x05,
	Error = 0x06
};

struct _NuevaDescarga {
	QString enlace;
	QString nombre;
	int categoria;
	bool iniciar;
};

extern sqlite3 *iniciarConexionBaseDatos();
extern void cerrarConexionBaseDatos(sqlite3 *baseDatos);
extern void baseDatosEjecutar(sqlite3 *baseDatos, const QString instruccion);
extern sqlite3_stmt *baseDatosPreparar(sqlite3 *baseDatos, const QString instruccion);
extern void baseDatosFinalizar(sqlite3_stmt *resultados);

/**
 * @brief Obtiene la ruta base para los iconos basado en el tema de escritorio activo
 * @return Ruta del icono
 */
extern QString obtenerRutaIcono();

/**
 * @brief Cifra un texto
 */
extern QByteArray cifrarTexto(const QString &datos, const QString &contrasena);

/**
 * @brief Descifra un texto
 */
extern QString descifrarTexto(const QByteArray &datos, const QString &contrasena);

/**
 * @brief Actualiza la configuración del proxy a nivel de aplicación
 */
extern QNetworkProxy obtenerConfiguracionProxy();

#ifdef Q_OS_WIN
extern QPalette _obtenerPaletaColores();
#endif

#endif // MAIN_HPP

