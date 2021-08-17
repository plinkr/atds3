#include "main.hpp"
#include "configuraciones.hpp"
#include "utiles.hpp"
#include "modelocategorias.hpp"
#include "modeloiconocategorias.hpp"
#include "modelotareas.hpp"
#include "modelopaquetes.hpp"
#include <QApplication>
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
#ifdef Q_OS_ANDROID
#include <QtAndroid>
#endif


QString _organizacionNombre;
QString _organizacionDominio;
QString _aplicacionNombreCorto;
QString _aplicacionTitulo;
QString _aplicacionVersion;
QString _agenteUsuarioTodus;
QString _numeroVersionTodus;
QString _rutaDescargaPredeterminada;
QObject *_qmlRaiz;


QNetworkProxy _obtenerProxy();
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

	_organizacionNombre = "ATDS3";
	_organizacionDominio = "atds3.cu";
	_aplicacionNombreCorto = "atds3";
	_aplicacionTitulo = "Administrador de Transferencias para toDus (S3)";
	_aplicacionVersion ="1.5.0";
	_agenteUsuarioTodus = "ToDus 0.40.19";
	_numeroVersionTodus = "21823";
#ifdef Q_OS_WIN
	_rutaDescargaPredeterminada = QString("file:///%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DownloadLocation)).arg(_aplicacionNombreCorto);
#else
	_rutaDescargaPredeterminada = QString("file://%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DownloadLocation)).arg(_aplicacionNombreCorto);
#endif

	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication::setOrganizationName(_organizacionNombre);
	QApplication::setOrganizationDomain(_organizacionDominio);
	QApplication::setApplicationName(_aplicacionNombreCorto);
	QApplication::setApplicationVersion(_aplicacionVersion);

#ifdef Q_OS_WINDOWS
	QSharedMemory memoriaCompartida(_organizacionNombre);
	if (memoriaCompartida.create(1) == false) {
		return codigoSalida;
	}
#endif

	QApplication app(argc, argv);
	app.setApplicationDisplayName(_aplicacionTitulo);
	app.setWindowIcon(QIcon(":/svg/atds3.svg"));

	{
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

	QQuickStyle::setStyle("Material");

	qmlRegisterType<Configuraciones>("cu.atds3.librerias", 1, 0, "Configuraciones");
	qmlRegisterType<Utiles>("cu.atds3.librerias", 1, 0, "Utiles");
	qmlRegisterType<ModeloCategorias>("cu.atds3.librerias", 1, 0, "ModeloCategorias");
	qmlRegisterType<ModeloIconoCategorias>("cu.atds3.librerias", 1, 0, "ModeloIconoCategorias");
	qmlRegisterType<ModeloTareas>("cu.atds3.librerias", 1, 0, "ModeloTareas");
	qmlRegisterType<ModeloPaquetes>("cu.atds3.librerias", 1, 0, "ModeloPaquetes");

	NetworkAccessManagerFactory namf;
	QQmlApplicationEngine maquinaQML;
	const QUrl url(QStringLiteral("qrc:/qml/atds3.qml"));
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
			_qmlRaiz = obj;
#ifdef Q_OS_ANDROID
			QList<QString> listadoPermisos {
				"android.permission.ACCESS_NETWORK_STATE",
				"android.permission.ACCESS_WIFI_STATE",
				"android.permission.INTERNET",
				"android.permission.MANAGE_DOCUMENTS",
				"android.permission.READ_EXTERNAL_STORAGE",
				"android.permission.READ_USER_DICTIONARY",
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
#endif
		}
	}, Qt::QueuedConnection);
	maquinaQML.load(url);

	codigoSalida = app.exec();

	return codigoSalida;
}

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
