#include "main.hpp"
#include "ventanaconfiguracion.hpp"
#include "ventanaprincipal.hpp"
#include <sqlite3.h>
#include <openssl/evp.h>
#include <QApplication>
#include <QSettings>
#include <QColor>
#include <QPalette>
#include <QToolTip>
#include <QLineEdit>
#include <QStandardPaths>
#include <QDir>
#include <QNetworkProxy>


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

QString _rutaBaseDatos;

/**
 * @brief Ruta en donde almacenar las descargas
 */
QString _rutaDescargas;

/**
 * @brief Agente de usuario de toDus
 */
QString _agenteUsuarioTodus;

/**
 * @brief Base de datos
 */
sqlite3 *_baseDatos;

/**
 * @brief Sesión toDus
 */
QSharedPointer<toDus> _toDus;

bool _temaClaro = true;

sqlite3 *iniciarConexionBaseDatos();
void cerrarConexionBaseDatos(sqlite3 *baseDatos);
void baseDatosEjecutar(sqlite3 *baseDatos, const QString instruccion);
sqlite3_stmt *baseDatosPreparar(sqlite3 *baseDatos, const QString instruccion);
void baseDatosFinalizar(sqlite3_stmt *resultados);

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

/**
 * @brief Obtiene la ruta base para los iconos basado en el tema de escritorio activo
 * @return Ruta del icono
 */
QString obtenerRutaIcono();

/**
 * @brief Cifra un texto
 */
QByteArray cifrarTexto(const QString &datos, const QString &contrasena);

/**
 * @brief Descifra un texto
 */
QString descifrarTexto(const QByteArray &datos, const QString &contrasena);

/**
 * @brief Actualiza la configuración del proxy a nivel de aplicación
 */
QNetworkProxy obtenerConfiguracionProxy();

#ifdef Q_OS_WIN
QPalette _obtenerPaletaColoresOscuros();
#endif


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	int codigoSalida = 0;

	_aplicacionTitulo = "Administrador de Transferencias para toDus (S3)";
	_aplicacionNombreCorto = "atds3";
	_aplicacionVersion = "0.4.0";
	_agenteUsuarioTodus = "ToDus 0.38.35";

	app.setOrganizationName(_aplicacionNombreCorto);
	app.setApplicationDisplayName(_aplicacionTitulo);
	app.setApplicationName(_aplicacionNombreCorto);
	app.setApplicationVersion(_aplicacionVersion);
/*
	if (iniciarConexionBaseDatos() == false) {
		return EXIT_FAILURE;
	}
*/
	crearConfiguracionesDefecto();

	crearDirectoriosDescargas();

#ifdef Q_OS_WIN
	{
		QSettings windowsRegistro("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);

		app.setStyle("Fusion");
		if (windowsRegistro.value("AppsUseLightTheme").toInt() == 0) {
			QToolTip::setPalette(_obtenerPaletaColoresOscuros());
			app.setPalette(_obtenerPaletaColoresOscuros());
			_temaClaro = false;
		}
	}
#else
	QPalette *p = new QPalette();
	if (p->window().color().lightness() >= 150) {
		_temaClaro = true;
	} else {
		_temaClaro = false;
	}
	delete p;
#endif

	VentanaPrincipal ventanaPrincipal;
	ventanaPrincipal.show();

	codigoSalida = app.exec();

//	cerrarConexionBaseDatos();

	return codigoSalida;
}

sqlite3 *iniciarConexionBaseDatos() {
	sqlite3 *baseDatos;
	_rutaBaseDatos = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/" + _aplicacionNombreCorto.toLower() + "/" + _aplicacionNombreCorto + ".db";

	sqlite3_open_v2(_rutaBaseDatos.toStdString().c_str(), &baseDatos, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, NULL);

	return baseDatos;
}

void cerrarConexionBaseDatos(sqlite3 *baseDatos) {
	sqlite3_close_v2(baseDatos);
}

void baseDatosEjecutar(sqlite3 *baseDatos, const QString instruccion) {
	sqlite3_stmt *resultados;

	if (baseDatos == NULL) {
		baseDatos = iniciarConexionBaseDatos();
	}

	sqlite3_prepare_v2(baseDatos, instruccion.toStdString().c_str(), instruccion.size(), &resultados, NULL);
	sqlite3_step(resultados);
	sqlite3_finalize(resultados);
}

sqlite3_stmt *baseDatosPreparar(sqlite3 *baseDatos, const QString instruccion) {
	sqlite3_stmt *resultados;

	sqlite3_prepare_v2(baseDatos, instruccion.toStdString().c_str(), instruccion.size(), &resultados, NULL);

	return resultados;
}

void baseDatosFinalizar(sqlite3_stmt *resultados) {
	sqlite3_finalize(resultados);
}

/**
 * @brief Crea las configuraciones de la aplicación por defecto si no existen
 */
void crearConfiguracionesDefecto() {
	QSettings configuracion;
	QSslConfiguration configuracionSSL = QSslConfiguration::defaultConfiguration();

	configuracionSSL.setPeerVerifyMode(QSslSocket::PeerVerifyMode::VerifyNone);
	configuracionSSL.setProtocol(QSsl::SslProtocol::TlsV1_2);
	QSslConfiguration::setDefaultConfiguration(configuracionSSL);

	if (configuracion.contains("descargas/ruta") == false) {
		configuracion.setValue("descargas/ruta", obtenerRutaDescargas());
	}
	if (configuracion.contains("descargas/descargasParalelas") == false) {
		configuracion.setValue("descargas/descargasParalelas", 3);
	}
	if (configuracion.contains("descargas/descomprimirAlFinalizar") == false) {
		configuracion.setValue("descargas/descomprimirAlFinalizar", true);
	}
	if (configuracion.contains("descargas/eliminarAlFinalizar") == false) {
		configuracion.setValue("descargas/eliminarAlFinalizar", true);
	}
	if (configuracion.contains("subidas/subidasParalelas") == false) {
		configuracion.setValue("subidas/subidasParalelas", 2);
	}
	if (configuracion.contains("subidas/comprimirArchivosAntesSubir") == false) {
		configuracion.setValue("subidas/comprimirArchivosAntesSubir", true);
	}
	if (configuracion.contains("subidas/dividirArchivosAntesSubir") == false) {
		configuracion.setValue("subidas/dividirArchivosAntesSubir", true);
	}
	if (configuracion.contains("subidas/tamanoDivisionArchivos") == false) {
		configuracion.setValue("subidas/tamanoDivisionArchivos", 50);
	}
	if (configuracion.contains("subidas/eliminarAlFinalizar") == false) {
		configuracion.setValue("subidas/eliminarAlFinalizar", true);
	}
	if (configuracion.contains("herramientas/ruta7Zip") == false) {
		configuracion.setValue("herramientas/ruta7Zip", "7z");
	}
	if (configuracion.contains("proxy/tipo") == false) {
		configuracion.setValue("proxy/tipo", 0);
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
	QVector<QString> directorios {"programas", "musica", "videos", "otros"};

	directorioDescarga.cd(rutaDescarga);
	for (const QString &directorio : directorios) {
		if (directorioDescarga.exists(directorio) == false) {
			directorioDescarga.mkdir(directorio);
		}
	}
}

/**
 * @brief Obtiene la ruta base para los iconos basado en el tema de escritorio activo
 * @return Ruta del icono
 */
QString obtenerRutaIcono() {
	if (_temaClaro == true) {
		return ":/iconos/claro/";
	} else {
		return ":/iconos/oscuro/";
	}
}

/**
 * @brief Cifra un texto
 */
QByteArray cifrarTexto(const QString &datos, const QString &contrasena) {
	std::string datosDescifrados = datos.toStdString();
	int longitudDatosDescifrados1 = 0;
	int longitudDatosDescifrados2 = 0;
	std::string vectorInicializacion = "8211123230432304";
	std::string contrasenaStd = contrasena.toStdString();
	EVP_CIPHER_CTX *evpContextoCifrado;
	std::string resultados;

	evpContextoCifrado = EVP_CIPHER_CTX_new();
	if (EVP_EncryptInit_ex(evpContextoCifrado, EVP_aes_256_cbc(), nullptr, (const unsigned char *)&contrasenaStd[0], (const unsigned char *)&vectorInicializacion[0]) != -1) {
		resultados.resize(512);

		if (EVP_EncryptUpdate(evpContextoCifrado, (unsigned char *)&resultados[0], &longitudDatosDescifrados1, (const unsigned char *)&datosDescifrados[0], static_cast<int>(datosDescifrados.size())) != -1) {
			if (EVP_EncryptFinal_ex(evpContextoCifrado, (unsigned char *)&resultados[0] + longitudDatosDescifrados1, &longitudDatosDescifrados2) != -1) {
				resultados.resize(longitudDatosDescifrados1 + longitudDatosDescifrados2);
			}
		}
	}

	EVP_CIPHER_CTX_free(evpContextoCifrado);

	return QByteArray::fromStdString(resultados);
}

/**
 * @brief Descifra un texto
 */
QString descifrarTexto(const QByteArray &datos, const QString &contrasena) {
	std::string datosCifrados = datos.toStdString();
	int longitudDatosCifrados1 = 0;
	int longitudDatosCifrados2 = 0;
	std::string vectorInicializacion = "8211123230432304";
	std::string contrasenaStd = contrasena.toStdString();
	EVP_CIPHER_CTX *evpContextoCifrado;
	std::string resultados;

	evpContextoCifrado = EVP_CIPHER_CTX_new();
	if (EVP_DecryptInit_ex(evpContextoCifrado, EVP_aes_256_cbc(), nullptr, (const unsigned char *)&contrasenaStd[0], (const unsigned char *)&vectorInicializacion[0]) != -1) {
		resultados.resize(512);

		if (EVP_DecryptUpdate(evpContextoCifrado, (unsigned char *)&resultados[0], &longitudDatosCifrados1, (const unsigned char *)&datosCifrados[0], static_cast<int>(datosCifrados.size())) != -1) {
			if (EVP_DecryptFinal_ex(evpContextoCifrado, (unsigned char *)&resultados[0] + longitudDatosCifrados1, &longitudDatosCifrados2) != -1) {
				resultados.resize(longitudDatosCifrados1 + longitudDatosCifrados2);
			}
		}
	}

	EVP_CIPHER_CTX_free(evpContextoCifrado);

	return QString::fromStdString(resultados);
}

/**
 * @brief Actualiza la configuración del proxy a nivel de aplicación
 */
QNetworkProxy obtenerConfiguracionProxy() {
	QSettings configuracion;
	QNetworkProxy proxy;

	proxy.setHostName(configuracion.value("proxy/servidor").toString());
	proxy.setPort(configuracion.value("proxy/puerto").toInt());
	if (configuracion.contains("proxy/usuario") == true) {
		proxy.setUser(configuracion.value("proxy/usuario").toString());
	}
	if (configuracion.contains("proxy/contrasena") == true) {
		proxy.setPassword(descifrarTexto(QByteArray::fromBase64(configuracion.value("proxy/contrasena").toByteArray()), proxy.user() + "@" + proxy.hostName() + ":" + QString::number(proxy.port())));
	}

	switch (configuracion.value("proxy/tipo", 0).toInt()) {
		case 0: // Sin proxy
		{
			QNetworkProxy noProxy;

			noProxy.setType(QNetworkProxy::NoProxy);

			return noProxy;
		}
		case 1: // HTTP(S)
			proxy.setType(QNetworkProxy::HttpProxy);

			break;
		case 2: // SOCKS 4/5
			proxy.setType(QNetworkProxy::Socks5Proxy);

			break;
	}

	return proxy;
}

#ifdef Q_OS_WIN
QPalette _obtenerPaletaColoresOscuros() {
	QPalette paletaColores = qApp->palette();

	paletaColores.setColor(QPalette::Window, QColor(53, 53, 53));
	paletaColores.setColor(QPalette::WindowText, Qt::white);
	paletaColores.setColor(QPalette::Base, QColor(25, 25, 25));
	paletaColores.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
	paletaColores.setColor(QPalette::ToolTipBase, QColor(53, 53, 53));
	paletaColores.setColor(QPalette::ToolTipText, Qt::white);
	paletaColores.setColor(QPalette::Text, Qt::white);
	paletaColores.setColor(QPalette::PlaceholderText, Qt::gray);
	paletaColores.setColor(QPalette::Button, QColor(53, 53, 53));
	paletaColores.setColor(QPalette::ButtonText, Qt::white);
	paletaColores.setColor(QPalette::BrightText, Qt::red);
	paletaColores.setColor(QPalette::Link, QColor(42, 130, 218));
	paletaColores.setColor(QPalette::Highlight, QColor(42, 130, 218));
	paletaColores.setColor(QPalette::HighlightedText, Qt::black);
	paletaColores.setColor(QPalette::Disabled, QPalette::Text, QColor(164, 166, 168));
	paletaColores.setColor(QPalette::Disabled, QPalette::WindowText, QColor(164, 166, 168));
	paletaColores.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(164, 166, 168));
	paletaColores.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(164, 166, 168));
	paletaColores.setColor(QPalette::Disabled, QPalette::Base, QColor(68, 68, 68));
	paletaColores.setColor(QPalette::Disabled, QPalette::Window, QColor(68, 68, 68));
	paletaColores.setColor(QPalette::Disabled, QPalette::Highlight, QColor(68, 68, 68));

	return paletaColores;
}
#endif
