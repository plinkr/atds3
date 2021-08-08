#include "utiles.hpp"
#include "main.hpp"
#include <QApplication>
#include <QMenu>
#include <QSqlDatabase>
#include <QDir>
#include <QStandardPaths>
#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QAndroidJniObject>
#endif // Q_OS_ANDROID


Utiles::Utiles(QObject *padre)
	: QObject(padre) {}

void Utiles::iniciarMonitorizacionConexionTodus() {
	connect(&_socaloTCPDisponibilidadTodus, &QTcpSocket::connected, this, &Utiles::eventoConexionTodus);
	connect(&_socaloTCPDisponibilidadTodus, &QTcpSocket::errorOccurred, this, &Utiles::eventoErrorConexionTodus);
	_temporizadorVerificadorDisponibilidadTodus.setInterval(30000);
	connect(&_temporizadorVerificadorDisponibilidadTodus, &QTimer::timeout, this, &Utiles::verificarDisponibilidadTodus);
	_temporizadorVerificadorDisponibilidadTodus.start();
	_estadoDisponibilidadTodus = EstadosTodus::Desconocido;
	QTimer::singleShot(5000, this, &Utiles::verificarDisponibilidadTodus);
}

void Utiles::crearBandejaIcono() {
#ifndef Q_OS_ANDROID
	_bandejaIcono.setIcon(QIcon(":/png/atds3.png"));
	_bandejaIcono.setToolTip(_aplicacionTitulo);

	QMenu *menuBandeIcono = new QMenu();
	menuBandeIcono->addAction(QIcon(":/svg/window-close.svg"), "Terminar", this, &QApplication::quit);

	_bandejaIcono.setContextMenu(menuBandeIcono);
	_bandejaIcono.show();
	connect(&_bandejaIcono, &QSystemTrayIcon::activated, this, &Utiles::mostrarOcultarVentana);
#endif // Q_OS_ANDROID
}
void Utiles::notificar(const QString &llave, bool valorPredeterminado, const QString &titulo, const QString &mensaje, const QString &sonido) {
	QString llaveVisual = "notificaciones/visual" + llave;
	QString llaveAudible = "notificaciones/audible" + llave;

	if (_configuraciones.value(llaveVisual, valorPredeterminado).toBool() == true) {
		_bandejaIcono.showMessage(titulo, mensaje, QIcon(":/svg/atds3.svg"));
	}
	if (_configuraciones.value(llaveAudible, valorPredeterminado).toBool() == true) {
		QMetaObject::invokeMethod(_qmlRaiz, "reproducirNotificacion", Qt::QueuedConnection, Q_ARG(QVariant, sonido));
	}
}

void Utiles::mostrarOcultarVentana(QSystemTrayIcon::ActivationReason razon) {
	if (razon == QSystemTrayIcon::Trigger || razon == QSystemTrayIcon::MiddleClick) {
		QMetaObject::invokeMethod(_qmlRaiz, "mostrarOcultarVentana", Qt::QueuedConnection);
	}
}

void Utiles::verificarDisponibilidadTodus() {
	QString servidorMensajeria = _configuraciones.value("avanzadas/servidorMensajeria", "im.todus.cu").toString();
	int servidorMensajeriaPuerto = _configuraciones.value("avanzadas/servidorMensajeriaPuerto", 1756).toInt();

	if (_socaloTCPDisponibilidadTodus.state() != QAbstractSocket::UnconnectedState) {
		_socaloTCPDisponibilidadTodus.disconnectFromHost();
		_socaloTCPDisponibilidadTodus.waitForDisconnected(5000);
	}
	_socaloTCPDisponibilidadTodus.connectToHost(servidorMensajeria, servidorMensajeriaPuerto);
}

void Utiles::eventoConexionTodus() {
	QMetaObject::invokeMethod(_qmlRaiz, "actualizarEstadoDisponibilidadTodus", Qt::QueuedConnection, Q_ARG(QVariant, "disponible"));
	if (_estadoDisponibilidadTodus != EstadosTodus::Disponible) {
		notificar("ConexionDisponibleTodus", false, "Disponibilidad de toDus", "Se ha detectado disponibilidad de acceso a la red toDus.", "conexion-disponible-todus.mp3");
	}
	_socaloTCPDisponibilidadTodus.disconnectFromHost();
	_estadoDisponibilidadTodus = EstadosTodus::Disponible;
}

void Utiles::eventoErrorConexionTodus(QAbstractSocket::SocketError errorSocalo) {
	Q_UNUSED(errorSocalo);

	QMetaObject::invokeMethod(_qmlRaiz, "actualizarEstadoDisponibilidadTodus", Qt::QueuedConnection, Q_ARG(QVariant, "perdido"));
	if (_estadoDisponibilidadTodus != EstadosTodus::Perdido) {
		notificar("ConexionPerdidaTodus", true, "Pérdida de la disponibilidad de toDus", "Se ha detectado la pérdida de la disponibilidad de acceso a la red toDus.", "conexion-perdida-todus.mp3");
	}
	_estadoDisponibilidadTodus = EstadosTodus::Perdido;
}

void Utiles::restablecerDatosFabrica() {
	QSqlDatabase bd = QSqlDatabase::database();
	QDir directorio;

	bd.close();
	directorio.remove(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" + _aplicacionNombreCorto + ".db");

	_configuraciones.remove("");

	QApplication::quit();
}

QString Utiles::rutaDesdeURI(const QString &uri) {
	QString ruta = QByteArray::fromPercentEncoding(uri.toUtf8());

#ifdef Q_OS_ANDROID
	QAndroidJniObject jniPath = QAndroidJniObject::fromString(uri);
	QAndroidJniObject jniUri = QAndroidJniObject::callStaticObjectMethod("android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;", jniPath.object());

	ruta = QAndroidJniObject::callStaticObjectMethod(
			  "org/ekkescorner/utils/QSharePathResolver", "getRealPathFromURI",
			  "(Landroid/content/Context;Landroid/net/Uri;)Ljava/lang/String;",
			  QtAndroid::androidContext().object(), jniUri.object<jobject>()
	).toString();
#else
#ifdef Q_OS_WINDOWS
	ruta.replace("file:///", "");
#else
	ruta.replace("file://", "");
#endif // Q_OS_WINDOWS

#endif // Q_OS_ANDROID

	return ruta;
}
