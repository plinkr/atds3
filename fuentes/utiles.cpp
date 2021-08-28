#include "utiles.hpp"
#include "modelopaquetes.hpp"
#include "main.hpp"
#include <QApplication>
#include <QMenu>
#include <QSqlDatabase>
#include <QDir>
#include <QStandardPaths>
#include <QTime>
#include <QThread>
#include <QProcess>
#if (defined(Q_OS_UNIX) || defined(Q_OS_LINUX)) && !defined(Q_OS_ANDROID) && !defined(Q_OS_MACOS)
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/extensions/dpms.h>
#endif // (defined(Q_OS_UNIX) || defined(Q_OS_LINUX)) && !defined(Q_OS_ANDROID) && !defined(Q_OS_MACOS)
#if defined(Q_OS_UNIX) && !defined(Q_OS_LINUX) && !defined(Q_OS_MACOS)
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <dev/acpica/acpiio.h>
#endif // defined(Q_OS_UNIX) && !defined(Q_OS_LINUX) && !defined(Q_OS_MACOS)
#ifdef Q_OS_MACOS
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/pwr_mgt/IOPMLib.h>
#endif // Q_OS_MACOS
#ifdef Q_OS_WINDOWS
#include <windows.h>
#include <winuser.h>
#include <powrprof.h>
#include <reason.h>
#endif
#ifdef Q_OS_ANDROID
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>

Utiles *_instanciaUtiles;


static void _notificarSeleccionRuta(JNIEnv *env, jobject, jstring ruta) {
	_instanciaUtiles->reportarSeleccionRutaDescarga(QString::fromUtf8(env->GetStringUTFChars(ruta, nullptr)));
}

static void _notificarSeleccionArchivoDescarga(JNIEnv, jobject, jint descriptor) {
	_instanciaUtiles->reportarSeleccionArchivoDescarga((int)descriptor);
}

static void _notificarArchivoCompartido(JNIEnv *env, jobject, jstring ruta) {
	_instanciaUtiles->reportarArchivoCompartido(QString::fromUtf8(env->GetStringUTFChars(ruta, nullptr)));
}
#endif // Q_OS_ANDROID


Utiles::Utiles(QObject *padre)
	: QObject(padre) {
#ifdef Q_OS_ANDROID
	_instanciaUtiles = this;

	JNINativeMethod metodosNativosJava[] {
		{"notificarSeleccionRutaDescarga", "(Ljava/lang/String;)V", reinterpret_cast<void *>(_notificarSeleccionRuta)},
		{"notificarSeleccionArchivoDescarga", "(I)V", reinterpret_cast<void *>(_notificarSeleccionArchivoDescarga)},
		{"notificarArchivoCompartido", "(Ljava/lang/String;)V", reinterpret_cast<void *>(_notificarArchivoCompartido)}
	};
	QAndroidJniEnvironment env;
	jclass objetoClase = env->GetObjectClass(QtAndroid::androidActivity().object<jobject>());
	env->RegisterNatives(objetoClase, metodosNativosJava, sizeof(metodosNativosJava) / sizeof(metodosNativosJava[0]));
	env->DeleteLocalRef(objetoClase);
#endif // Q_OS_ANDROID
}

void Utiles::establecerObjetoModeloPaquetes(ModeloPaquetes *objeto) {
	_modeloPaquetes = objeto;
}

void Utiles::iniciarMonitorizacionConexionTodus() {
	connect(&_socaloTCPDisponibilidadTodus, &QTcpSocket::connected, this, &Utiles::eventoConexionTodus);
	connect(&_socaloTCPDisponibilidadTodus, &QTcpSocket::errorOccurred, this, &Utiles::eventoErrorConexionTodus);

	_temporizadorVerificadorDisponibilidadTodus.setInterval(30000);
	connect(&_temporizadorVerificadorDisponibilidadTodus, &QTimer::timeout, this, &Utiles::verificarDisponibilidadTodus);
	_temporizadorVerificadorDisponibilidadTodus.start();
	_estadoDisponibilidadTodus = EstadosTodus::Desconocido;
	QTimer::singleShot(5000, this, &Utiles::verificarDisponibilidadTodus);

	connect(&_temporizadorProgramacionInicioColaTransferencias, &QTimer::timeout, this, &Utiles::verificarProgramacionInicioColaTransferencias);
	_temporizadorProgramacionInicioColaTransferencias.setInterval((60 - QTime::currentTime().second()) * 1000);
	if (_configuraciones.value("programacion/colaTransferenciasInicio", false).toBool() == true) {
		_temporizadorProgramacionInicioColaTransferencias.start();
	}
	connect(&_temporizadorProgramacionFinalizarColaTransferencias, &QTimer::timeout, this, &Utiles::verificarProgramacionFinalizarColaTransferencias);
	_temporizadorProgramacionFinalizarColaTransferencias.setInterval((60 - QTime::currentTime().second()) * 1000);
	if (_configuraciones.value("programacion/colaTransferenciasFinalizar", false).toBool() == true) {
		_temporizadorProgramacionFinalizarColaTransferencias.start();
	}
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

void Utiles::verificarProgramacionInicioColaTransferencias() {
	QString horaProgramada = _configuraciones.value("programacion/colaTransferenciasInicioHora", "23:00").toString();

	if (QTime::currentTime().toString("hh:mm") == horaProgramada) {
		emitirRegistro(TiposRegistro::Informacion, "UTIL") << "Activada la inicialización programada de la cola de las transferencias." << std::endl;
		_modeloPaquetes->iniciarTodosPaquetes();
		switch (_configuraciones.value("programacion/colaTransferenciasAccionInicio", 0).toInt()) {
			case Utiles::ProgramacionAcciones::ApagarPantalla:
				ejecutarAccionApagarPantalla();
				break;
			default:
				break;
		}
	}

	if (_temporizadorProgramacionInicioColaTransferencias.interval() != 60000) {
		_temporizadorProgramacionInicioColaTransferencias.setInterval(60000);
	}
}

void Utiles::verificarProgramacionFinalizarColaTransferencias() {
	QString horaProgramada = _configuraciones.value("programacion/colaTransferenciasFinalizarHora", "08:00").toString();

	if (QTime::currentTime().toString("hh:mm") == horaProgramada) {
		emitirRegistro(TiposRegistro::Informacion, "UTIL") << "Activada la finalización programada de la cola de las transferencias." << std::endl;
		_modeloPaquetes->pausarTodosPaquetes();
		switch (_configuraciones.value("programacion/colaTransferenciasAccionFinalizar", 0).toInt()) {
			case Utiles::ProgramacionAcciones::Suspender:
				ejecutarAccionSuspender();
				break;
			case Utiles::ProgramacionAcciones::Hibernar:
				ejecutarAccionHibernar();
				break;
			case Utiles::ProgramacionAcciones::Apagar:
				ejecutarAccionApagar();
				break;
			default:
				break;
		}
	}

	if (_temporizadorProgramacionFinalizarColaTransferencias.interval() != 60000) {
		_temporizadorProgramacionFinalizarColaTransferencias.setInterval(60000);
	}
}

void Utiles::ejecutarAccionApagarPantalla() {
#if defined(Q_OS_UNIX) || defined(Q_OS_LINUX) || defined(Q_OS_MACOS) || defined(Q_OS_WINDOWS)
	emitirRegistro(TiposRegistro::Informacion, "UTIL") << "Apangando la pantalla..." << std::endl;
#endif
#if (defined(Q_OS_UNIX) || defined(Q_OS_LINUX)) && !defined(Q_OS_ANDROID) && !defined(Q_OS_MACOS)
	Display *pantalla = QX11Info::display();

	DPMSEnable(pantalla);
	QThread::currentThread()->msleep(100);
	DPMSForceLevel(pantalla, DPMSModeOff);
	XSync(pantalla, False);
#endif
#ifdef Q_OS_MACOS
	io_registry_entry_t disp_wrangler = IO_OBJECT_NULL;
	kern_return_t kr;

	disp_wrangler = IORegistryEntryFromPath(kIOMasterPortDefault, kIOServicePlane ":/IOResources/IODisplayWrangler");
	if(disp_wrangler == IO_OBJECT_NULL) {
		return;
	}
	kr = IORegistryEntrySetCFProperty( disp_wrangler, CFSTR("IORequestIdle"), kCFBooleanTrue);
	IOObjectRelease(disp_wrangler);
#endif
#ifdef Q_OS_WINDOWS
	SendMessageA(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
#endif
}

void Utiles::ejecutarAccionSuspender() {
#if defined(Q_OS_UNIX) || defined(Q_OS_LINUX) || defined(Q_OS_MACOS) || defined(Q_OS_WINDOWS)
	emitirRegistro(TiposRegistro::Informacion, "UTIL") << "Suspendiendo hacia la RAM..." << std::endl;
#endif
#if defined(Q_OS_UNIX) && !defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID) && !defined(Q_OS_MACOS)
	BSDSolicitarEstadoACPI(EstadosACPI::SuspenderHaciaRAM);
#endif
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
//	LinuxSolicitarEstadoACPI("mem");
	QProcess::execute("systemctl", QStringList { "suspend" });
#endif
#ifdef Q_OS_MACOS
	io_connect_t fb = IOPMFindPowerManagement(MACH_PORT_NULL);

	if (fb != MACH_PORT_NULL)
	{
		IOPMSleepSystem(fb);
		IOServiceClose(fb);
	}
#endif
#ifdef Q_OS_WINDOWS
	if (WindowsActivarPrivilegios("SeShutdownPrivilege") == true) {
		SetSuspendState(FALSE, TRUE, FALSE);
	}
#endif
}

void Utiles::ejecutarAccionHibernar() {
#if defined(Q_OS_UNIX) || defined(Q_OS_LINUX) || defined(Q_OS_MACOS) || defined(Q_OS_WINDOWS)
	emitirRegistro(TiposRegistro::Informacion, "UTIL") << "Suspendiendo hacia el disco duro..." << std::endl;
#endif
#if defined(Q_OS_UNIX) && !defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID) && !defined(Q_OS_MACOS)
	BSDSolicitarEstadoACPI(EstadosACPI::SuspenderHaciaDiscoDuro);
#endif
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
//	LinuxSolicitarEstadoACPI("disk");
	QProcess::execute("systemctl", QStringList { "hibernate" });
#endif
#ifdef Q_OS_MACOS
	io_connect_t fb = IOPMFindPowerManagement(MACH_PORT_NULL);

	if (fb != MACH_PORT_NULL)
	{
		IOPMSleepSystem(fb);
		IOServiceClose(fb);
	}
#endif
#ifdef Q_OS_WINDOWS
	if (WindowsActivarPrivilegios("SeShutdownPrivilege") == true) {
		SetSuspendState(TRUE, TRUE, FALSE);
	}
#endif
}

void Utiles::ejecutarAccionApagar() {
#if defined(Q_OS_UNIX) || defined(Q_OS_LINUX) || defined(Q_OS_MACOS) || defined(Q_OS_WINDOWS)
	emitirRegistro(TiposRegistro::Informacion, "UTIL") << "Apagando el sistema..." << std::endl;
#endif
#if (defined(Q_OS_UNIX) || defined(Q_OS_LINUX)) && !defined(Q_OS_ANDROID) && !defined(Q_OS_MACOS)
	QProcess::execute("shutdown", QStringList { "-h", "now" });
#endif
#ifdef Q_OS_MACOS
	QProcess::execute("shutdown", QStringList { "-h", "now" });
#endif
#ifdef Q_OS_WINDOWS
	if (WindowsActivarPrivilegios("SeShutdownPrivilege") == true) {
		ExitWindowsEx(EWX_SHUTDOWN | EWX_POWEROFF | EWX_FORCE, SHTDN_REASON_MAJOR_APPLICATION | SHTDN_REASON_MINOR_OTHER | SHTDN_REASON_FLAG_PLANNED);
	}
#endif
}

#if defined(Q_OS_UNIX) && !defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID) && !defined(Q_OS_MACOS)
void Utiles::BSDSolicitarEstadoACPI(int estadoACPI) {
	int daACPI = open("/dev/acpi", O_RDWR);

	if (daACPI == -1) {
		daACPI = open("/dev/acpi", O_RDONLY);
	}
	if (daACPI != -1) {
		int valorError = 0;

		ioctl(daACPI, ACPIIO_REQSLPSTATE, &estadoACPI);
		ioctl(daACPI, ACPIIO_ACKSLPSTATE, &valorError);
		close(daACPI);
	}
}
#endif

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
void Utiles::LinuxSolicitarEstadoACPI(const std::string &estadoACPI) {
/*
	int daACPI = open("/sys/power/state", O_RDWR);

	if (daACPI != -1) {
		write(daACPI, estadoACPI.c_str(), estadoACPI.size());
		close(daACPI);
	}
*/
}
#endif

#ifdef Q_OS_WINDOWS
bool Utiles::WindowsActivarPrivilegios(const std::string privilegio) {
	HANDLE hFicha;
	TOKEN_PRIVILEGES fichaPrivilegio;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hFicha)) {
		LookupPrivilegeValueA(NULL, privilegio.c_str(), &fichaPrivilegio.Privileges[0].Luid);

		fichaPrivilegio.PrivilegeCount = 1;
		fichaPrivilegio.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges(hFicha, FALSE, &fichaPrivilegio, 0, (PTOKEN_PRIVILEGES)NULL, 0);

		if (GetLastError() == ERROR_SUCCESS) {
			return true;
		}
	}

	return false;
}
#endif

void Utiles::restablecerDatosFabrica() {
	QSqlDatabase bd = QSqlDatabase::database();
	QDir directorio;

	bd.close();
	directorio.remove(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" + _aplicacionNombreCorto + ".db");

	_configuraciones.remove("");

	QApplication::quit();
}

QVariant Utiles::obtenerRutaSistema(int indice) {
#ifdef Q_OS_ANDROID
	return QVariant(QtAndroid::androidActivity().callObjectMethod("obtenerRutaSistema", "(I)Ljava/lang/String;", indice).toString());
#else
	Q_UNUSED(indice)

	return QVariant();
#endif // Q_OS_ANDROID
}

void Utiles::seleccionarRutaExterno() {
#ifdef Q_OS_ANDROID
	QtAndroid::androidActivity().callMethod<void>("seleccionarRutaExterno");
#endif // Q_OS_ANDROID
}

void Utiles::reportarSeleccionRutaDescarga(const QString &ruta) {
	emit notificarSeleccionRuta(ruta);
}

void Utiles::reportarSeleccionArchivoDescarga(int descriptor) {
	emit notificarSeleccionArchivoDescarga(descriptor);
}

void Utiles::reportarArchivoCompartido(const QString &ruta) {
	emit notificarArchivoCompartido(ruta);
}

void Utiles::seleccionarArchivoDescarga() {
#ifdef Q_OS_ANDROID
	QtAndroid::androidActivity().callMethod<void>("seleccionarArchivoDescarga");
#endif
}

void Utiles::crearDirectorio(const QString &ubicacion) {
/*#ifdef Q_OS_ANDROID
	QAndroidJniObject jniRuta = QAndroidJniObject::fromString(ubicacion);

	QtAndroid::androidActivity().callMethod<void>("crearDirectorio", "(Ljava/lang/String;)V", jniRuta.object<jstring>());
#else*/
	QDir directorio;
	QString ruta = rutaDesdeURI(ubicacion);

	if (directorio.exists(ruta) == false) {
		directorio.mkpath(ruta);
	}
//#endif // Q_OS_ANDROID
}

QString Utiles::rutaDesdeURI(const QString &uri) {
	QString ruta = QByteArray::fromPercentEncoding(uri.toUtf8());

#ifdef Q_OS_ANDROID
	if (ruta.startsWith("file://") == false) {
		QAndroidJniObject jniPath = QAndroidJniObject::fromString(uri);
		QAndroidJniObject jniUri = QAndroidJniObject::callStaticObjectMethod("android/net/Uri", "parse", "(Ljava/lang/String;)Landroid/net/Uri;", jniPath.object());

		ruta = QAndroidJniObject::callStaticObjectMethod(
				  "org/ekkescorner/utils/QSharePathResolver", "getRealPathFromURI",
				  "(Landroid/content/Context;Landroid/net/Uri;)Ljava/lang/String;",
				  QtAndroid::androidContext().object(), jniUri.object()
		).toString();
	}

	ruta.replace("file://", "");
#else
#ifdef Q_OS_WINDOWS
	ruta.replace("file:///", "");
#else
	ruta.replace("file://", "");
#endif // Q_OS_WINDOWS
#endif // Q_OS_ANDROID

	return ruta;
}

int Utiles::androidAbrirArchivo(const QString &uri, const QString &modo) {
#ifdef Q_OS_ANDROID
	QAndroidJniObject jniUri = QAndroidJniObject::fromString(uri);
	QAndroidJniObject jniModo = QAndroidJniObject::fromString(modo);
	int fd = QAndroidJniObject::callStaticObjectMethod(
			  "cu/atds3/android/Archivos", "abrirArchivo",
			  "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
			  QtAndroid::androidContext().object(), jniUri.object(), jniModo.object()
	).toString().toInt();

	return fd;
#else
	Q_UNUSED(uri);
	Q_UNUSED(modo);

	return -1;
#endif // Q_OS_ANDROID
}

void Utiles::activarProgramacionInicioColaTransferencias(bool activar) {
	if (activar == true) {
		if (_temporizadorProgramacionInicioColaTransferencias.isActive() == false) {
			_temporizadorProgramacionInicioColaTransferencias.start();
		}
	} else {
		if (_temporizadorProgramacionInicioColaTransferencias.isActive() == true) {
			_temporizadorProgramacionInicioColaTransferencias.stop();
		}
	}
}

void Utiles::activarProgramacionFinalizarColaTransferencias(bool activar) {
	if (activar == true) {
		if (_temporizadorProgramacionFinalizarColaTransferencias.isActive() == false) {
			_temporizadorProgramacionFinalizarColaTransferencias.start();
		}
	} else {
		if (_temporizadorProgramacionFinalizarColaTransferencias.isActive() == true) {
			_temporizadorProgramacionFinalizarColaTransferencias.stop();
		}
	}
}

QString Utiles::representarTamano(qint64 tamano) {
	QStringList listadoUnidadesTamano { "B", "KiB", "MiB", "GiB", "TiB" };
	qint64 unidadTamano = 0;
	double tamanoAProcesar = tamano;
	QString representacion = "0 B";

	try {
		while (tamanoAProcesar > 1024) {
			tamanoAProcesar /= 1024;
			unidadTamano++;
		}

		if (unidadTamano < 2) {
			representacion = QString::number((qint64)tamanoAProcesar) + " " + listadoUnidadesTamano[unidadTamano];
		} else {
			if (tamanoAProcesar - (qint64)tamanoAProcesar > 0) {
				representacion = QString::number(tamanoAProcesar, 'g', QString::number((qint64)tamanoAProcesar).size() + 2) + " " + listadoUnidadesTamano[unidadTamano];
			} else {
				representacion = QString::number((qint64)tamanoAProcesar) + " " + listadoUnidadesTamano[unidadTamano];
			}
		}
	} catch (std::exception &) {}

	return representacion;
}
