/*!
 * \file main.cpp
 * \brief Archivo en donde reside la función de entrada de la aplicación y funciones auxiliares de alcance global
 * \ingroup cu.atds3
 * \author Leinier Cruz Salfrán
 * Contact: leiniercs@gmail.com
 */

#include "main.hpp"
#include "configuraciones.hpp"
#include "utiles.hpp"
#include "modelocategorias.hpp"
#include "modeloiconocategorias.hpp"
#include "modelotareas.hpp"
#include "modelopaquetes.hpp"
#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QGuiApplication>
#else
#include <QApplication>
#endif
#include <QIcon>
#include <QSettings>
#include <QDir>
#include <QStandardPaths>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QNetworkAccessManager>
#include <QDateTime>
#include <QSharedMemory>
#include <QCryptographicHash>


/*!
 * \brief Nombre de la organización
 */
QString _organizacionNombre;

/*!
 * \brief Nombre del dominio
 */
QString _organizacionDominio;

/*!
 * \brief Nombre corto de la aplicación
 */
QString _aplicacionNombreCorto;

/*!
 * \brief Título descriptivo de la aplicación
 */
QString _aplicacionTitulo;

/*!
 * \brief Versión de la aplicación
 */
QString _aplicacionVersion;

/*!
 * \brief Agente de usuario a utilizar en las conexiones al servidor HTTP de la red toDus
 */
QString _agenteUsuarioTodus;

/*!
 * \brief Número de la versión a utilizar en las conexiones al servidor HTTP de la red toDus
 */
QString _numeroVersionTodus;

/*!
 * \brief Ruta del directorio predeterminada en donde se almacenarán las descargas y archivos de publicación
 */
QString _rutaDescargaPredeterminada;

/*!
 * \brief Referencia al objeto raíz QML
 */
QObject *_qmlRaiz;


/*!
 * \brief Obtiene las configuraciones del proxy definidas por el usuario
 * \return Información del proxy a utilizar en  las clases de acceso a la red
 */
QNetworkProxy _obtenerProxy();

/*!
 * \brief Emite un registro
 * \param tipo Tipo de registro a emitir
 * \param modulo Nombre del módulo que emite el registro
 * \return Referencia al flujo de salida
 */
std::ostream &emitirRegistro(TiposRegistro tipo, const QString &modulo);


QNetworkAccessManager *NetworkAccessManagerFactory::create(QObject *padre)
{
	QNetworkAccessManager *nam = new QNetworkAccessManager(padre);

	nam->setProxy(_obtenerProxy());

	return nam;
}


int main(int argc, char *argv[])
{
	int codigoSalida = -1;

	/*
	 * Inicialización de las variables
	 */

	_organizacionNombre = "ATDS3";
	_organizacionDominio = "atds3.cu";
	_aplicacionNombreCorto = "atds3";
	_aplicacionTitulo = "Administrador de Transferencias para toDus (S3)";
	_aplicacionVersion ="1.6.0";
	_agenteUsuarioTodus = "ToDus 0.40.30";
	_numeroVersionTodus = "21834";
#ifdef Q_OS_WIN
	_rutaDescargaPredeterminada = QString("file:///%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DownloadLocation)).arg(_aplicacionNombreCorto);
#else
	_rutaDescargaPredeterminada = QString("file://%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DownloadLocation)).arg(_aplicacionNombreCorto);
#endif

	/*
	 * Inicializacion de la variables y atributos de la aplicacion
	 */

#ifdef Q_OS_ANDROID
	QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QGuiApplication::setOrganizationName(_organizacionNombre);
	QGuiApplication::setOrganizationDomain(_organizacionDominio);
	QGuiApplication::setApplicationName(_aplicacionNombreCorto);
	QGuiApplication::setApplicationVersion(_aplicacionVersion);
#else
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setOrganizationName(_organizacionNombre);
	QApplication::setOrganizationDomain(_organizacionDominio);
	QApplication::setApplicationName(_aplicacionNombreCorto);
	QApplication::setApplicationVersion(_aplicacionVersion);
#endif

#ifdef Q_OS_WINDOWS
	/*
	 * Mecanismo de inicio de aplicacion unica
	 */

	QSharedMemory memoriaCompartida(_organizacionNombre);
	if (memoriaCompartida.create(1) == false) {
		return codigoSalida;
	}
#endif

#ifdef Q_OS_ANDROID
	QGuiApplication app(argc, argv);
#else
	QApplication app(argc, argv);
#endif
	app.setApplicationDisplayName(_aplicacionTitulo);
	app.setWindowIcon(QIcon(":/svg/atds3.svg"));

	{
		/*
		 * Apertura y configuracion de la base de datos SQLITE
		 */

		QSqlDatabase bd = QSqlDatabase::addDatabase("QSQLITE");
		QString rutaBaseDatos = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
		QDir directorio;

		if (directorio.exists(rutaBaseDatos) == false) {
			if (directorio.mkpath(rutaBaseDatos) == false) {
				return codigoSalida;
			}
		}

		bd.setDatabaseName(rutaBaseDatos + "/" + _aplicacionNombreCorto + ".db");
		if (bd.open() == false) {
			return codigoSalida;
		}
		bd.exec("PRAGMA temp_store = MEMORY");
		bd.exec("PRAGMA synchronous = OFF");
	}

	// Configuracion del estilo visual de la aplicacion (QML)
	QQuickStyle::setStyle("Material");

	/*
	 * Registro y exposicion de las clases requeridas al subsistema QML
	 */

	qmlRegisterType<Configuraciones>("cu.atds3.librerias", 1, 6, "Configuraciones");
	qmlRegisterType<Utiles>("cu.atds3.librerias", 1, 6, "Utiles");
	qmlRegisterType<ModeloCategorias>("cu.atds3.librerias", 1, 6, "ModeloCategorias");
	qmlRegisterType<ModeloIconoCategorias>("cu.atds3.librerias", 1, 6, "ModeloIconoCategorias");
	qmlRegisterType<ModeloTareas>("cu.atds3.librerias", 1, 6, "ModeloTareas");
	qmlRegisterType<ModeloPaquetes>("cu.atds3.librerias", 1, 6, "ModeloPaquetes");

	NetworkAccessManagerFactory namf;
	QQmlApplicationEngine maquinaQML;
	const QUrl url(QStringLiteral("qrc:/qml/atds3.qml"));
//	Utiles *utiles = new Utiles(&app);
	maquinaQML.setNetworkAccessManagerFactory(&namf);
	maquinaQML.rootContext()->setContextProperty(QStringLiteral("_aplicacionTitulo"), _aplicacionTitulo);
	maquinaQML.rootContext()->setContextProperty(QStringLiteral("_aplicacionVersion"), _aplicacionVersion);
	maquinaQML.rootContext()->setContextProperty(QStringLiteral("_agenteUsuarioTodus"), _agenteUsuarioTodus);
	maquinaQML.rootContext()->setContextProperty(QStringLiteral("_numeroVersionTodus"), _numeroVersionTodus);
#ifdef Q_OS_ANDROID
	maquinaQML.rootContext()->setContextProperty(QStringLiteral("tamanoFuente"), 13);
#else
	maquinaQML.rootContext()->setContextProperty(QStringLiteral("tamanoFuente"), 11);
#endif

	QObject::connect(&maquinaQML, &QQmlApplicationEngine::objectCreated, &app, [&](QObject *obj, const QUrl &objUrl) {
		if (!obj && url == objUrl) {
			QCoreApplication::exit(-1);
		} else {
			// Guardar la referencia al objeto raiz de la interfaz de usuario creada en QML
			_qmlRaiz = obj;

#ifdef Q_OS_ANDROID
			/*
			 * Solicitud de los permisos requeridos para operar en Android
			 */

			QList<QString> listadoPermisos {
				"android.permission.ACCESS_NETWORK_STATE",
				"android.permission.ACCESS_WIFI_STATE",
				"android.permission.INTERNET",
				"android.permission.MANAGE_EXTERNAL_STORAGE",
				"android.permission.WRITE_EXTERNAL_STORAGE"
			};
			QStringList permisosASolicitar;

			for (const QString &permiso : listadoPermisos) {
				QtAndroid::PermissionResult resultadoPermiso = QtAndroid::checkPermission(permiso);

				if (resultadoPermiso == QtAndroid::PermissionResult::Denied) {
					permisosASolicitar.append(permiso);
				}
			}

			if (permisosASolicitar.size() > 0) {
				QtAndroid::requestPermissions(permisosASolicitar, [](const QtAndroid::PermissionResultMap &) {});
			}

			QtAndroid::androidActivity().callMethod<void>("solicitarAdministracionCompletaArchivos");
#endif
		}
	}, Qt::QueuedConnection);
	maquinaQML.load(url);

	codigoSalida = app.exec();

	return codigoSalida;
}

/*!
 * \brief Obtiene las configuraciones del proxy definidas por el usuario
 * \return Información del proxy a utilizar en  las clases de acceso a la red
 */
QNetworkProxy _obtenerProxy() {
	Configuraciones configuraciones;
	bool activado = configuraciones.valor("proxy/activado", false).toBool();
	bool tipoOk = true;
	uint tipo = configuraciones.valor("proxy/tipo", QNetworkProxy::HttpProxy).toUInt(&tipoOk);
	QString anfitrion = configuraciones.valor("proxy/anfitrion", "proxy.local").toString();
	bool puertoOk = true;
	uint puerto = configuraciones.valor("proxy/puerto", 3128).toUInt(&puertoOk);
	QString usuario = configuraciones.valor("proxy/usuario", "").toString();
	QString contrasena = configuraciones.valor("proxy/contrasena", "").toString();
	QNetworkProxy proxy(QNetworkProxy::NoProxy);

	if (activado == true) {
		if (tipoOk == true) {
			proxy.setType(static_cast<QNetworkProxy::ProxyType>(tipo));

			if (anfitrion.isEmpty() == false && puertoOk == true) {
				proxy.setHostName(anfitrion);
				proxy.setPort(puerto);
				if (usuario.isEmpty() == false && contrasena.isEmpty() == false) {
					proxy.setUser(usuario);
					proxy.setPassword(contrasena);
				}
			}

		}
	}

	return proxy;
}

/*!
 * \brief Emite un registro
 * \param tipo Tipo de registro a emitir
 * \param modulo Nombre del módulo que emite el registro
 * \return Referencia al flujo de salida
 */
std::ostream &emitirRegistro(TiposRegistro tipo, const QString &modulo) {
	QDateTime tiempoActual(QDateTime::currentDateTime());
	QString registro;
	QTextStream flujo(&registro);

	if (tipo > TiposRegistro::Ninguno) {
		std::cerr << tiempoActual.toString(Qt::DateFormat::ISODateWithMs).toStdString() << " ";

		switch (tipo) {
			case TiposRegistro::Critico:
				std::cerr << "[CRIT] ";
				break;
			case TiposRegistro::Error:
				std::cerr << "[ERROR] ";
				break;
			case TiposRegistro::Informacion:
				std::cerr << "[INFO] ";
				break;
			case TiposRegistro::Depuracion:
				std::cerr << "[DEPU] ";
				break;
			default:
				break;
		}

		std::cerr << "[" << modulo.toStdString() << "] ";
	}

	return std::cerr;
}
