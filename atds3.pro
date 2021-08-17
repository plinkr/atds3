QT += quick quickcontrols2 svg sql network websockets multimedia

CONFIG += c++17

unix: !android {
	QT += widgets
}
unix: !android: !macx {
	QT += x11extras
}
android {
	QT += androidextras
}
windows: CONFIG += windows embed_manifest_exe

TEMPLATE = app

TARGET = atds3

VERSION = 1.5.0
windows {
	RC_ICONS = recursos/ico/atds3.ico
	QMAKE_TARGET_COMPANY = ATDS3
	QMAKE_TARGET_DESCRIPTION = Administrador de Transferencias para toDus (S3)
	QMAKE_TARGET_COPYRIGHT = Todos los derechos reservados.
	QMAKE_TARGET_PRODUCT = ATDS3.EXE
	RC_LANG = 0x040A
	RC_CODEPAGE = 0x04B0
}

HEADERS += \
	cabeceras/configuraciones.hpp \
	cabeceras/utiles.hpp \
	cabeceras/http.hpp \
	cabeceras/todus.pb.h \
	cabeceras/todus.hpp \
	cabeceras/modeloiconocategorias.hpp \
	cabeceras/modelocategorias.hpp \
	cabeceras/modelotareas.hpp \
	cabeceras/modelopaquetes.hpp \
	cabeceras/main.hpp

SOURCES += \
	fuentes/configuraciones.cpp \
	fuentes/utiles.cpp \
	fuentes/http.cpp \
	fuentes/todus.pb.cc \
	fuentes/todus.cpp \
	fuentes/modeloiconocategorias.cpp \
	fuentes/modelocategorias.cpp \
	fuentes/modelotareas.cpp \
	fuentes/modelopaquetes.cpp \
	fuentes/main.cpp

RESOURCES += recursos/recursos.qrc qml.qrc

DISTFILES +=	README.md \
				LICENSE

android {
	DISTFILES +=	android-sources/AndroidManifest.xml \
					android-sources/src/org/ekkescorner/utils/QSharePathResolver.java \
					android-sources/src/cu/atds3/android/ActividadExtendida.java \
					android-sources/src/cu/atds3/android/Archivos.java
}

INCLUDEPATH += cabeceras

unix: !android: QML_IMPORT_PATH += qml
android: QML_IMPORT_PATH += c:/proyectos/atds3-1.5.0/qml
windows: QML_IMPORT_PATH += c:/proyectos/atds3-1.5.0/qml

unix: !android: !macx {
	INCLUDEPATH += /usr/include /usr/local/include
	LIBS += -lX11 -lXext -lssl -lcrypto

	CONFIG(protobuf): LIBS += -lprotobuf
	!CONFIG(protobuf) {
		contains(QMAKE_HOST.arch, x86_64) {
			LIBPROTOBUF =	/usr/lib64/libprotobuf.a \
							/usr/lib/x86_64-linux-gnu/libprotobuf.a \
							/usr/local/lib64/libprotobuf.a
		} else {
			LIBPROTOBUF =	/usr/lib/libprotobuf.a \
							/usr/lib/i386-linux-gnu/libprotobuf.a \
							/usr/local/lib/libprotobuf.a
		}
		for(ruta, LIBPROTOBUF): exists($${ruta}): LIBS += $${ruta}
	}

	unix_desktop_icon.files = recursos/svg/atds3.svg
	unix_desktop_icon.path = $${QMAKE_INSTALL_PREFIX}/share/pixmaps

	unix_desktop.files = atds3.desktop
	unix_desktop.path = $${QMAKE_INSTALL_PREFIX}/share/applications
}

macx {
	ICON = recursos/icns/atds3.icns
	INCLUDEPATH += /usr/local/Cellar/openssl@1.1/1.1.1k/include /usr/local/include
	LIBS +=	-L/usr/local/Cellar/openssl@1.1/1.1.1k/lib -L/usr/local/lib \
			-lssl -lcrypto /usr/local/lib/libprotobuf.a
}

android {
	include(C:/android/sdk/android_openssl/openssl.pri)

	JAVA_HOME = C:/Program Files/Java/jdk1.8.0_291
#	ANDROID_ABIS = arm64-v8a armeabi-v7a
	ANDROID_MIN_SDK_VERSION = 21
	ANDROID_TARGET_SDK_VERSION = 29
	ANDROID_PERMISSIONS = android.permission.ACCESS_NETWORK_STATE android.permission.ACCESS_WIFI_STATE android.permission.INTERNET android.permission.MANAGE_DOCUMENTS android.permission.READ_EXTERNAL_STORAGE android.permission.READ_USER_DICTIONARY android.permission.WRITE_EXTERNAL_STORAGE
	ANDROID_VERSION_CODE = 0x1400
	ANDROID_VERSION_NAME = 1.4.0
	ANDROID_BUNDLED_JAR_DEPENDENCIES += jar/QtAndroid.jar jar/QtAndroidBearer.jar jar/QtAndroidExtras.jar jar/QtAndroidNetwork.jar jar/QtMultimedia.jar
	ANDROID_PACKAGE_SOURCE_DIR = $${PWD}/android-sources

	INCLUDEPATH +=	$${SSL_PATH}/static/include \
					C:/proyectos/protobuf-3.16.0/_construccion/android/$${ANDROID_TARGET_ARCH}/include
	LIBS +=	-L$${SSL_PATH}/latest/$${ANDROID_TARGET_ARCH} -lssl_1_1 -lcrypto_1_1 \
			C:/proyectos/protobuf-3.16.0/_construccion/android/$${ANDROID_TARGET_ARCH}/lib/libprotobuf.a
}

windows {
	contains(QMAKE_HOST.arch, x86_64) {
		INCLUDEPATH +=	C:/msys64/mingw64/include
		LIBS += -LC:/msys64/mingw64/lib C:/msys64/mingw64/lib/libprotobuf.a
	} else {
		DEFINES += PROTOBUF_USE_DLLS
		INCLUDEPATH +=	C:/msys64/mingw32/include
		LIBS += -LC:/msys64/mingw32/lib -lprotobuf
	}

	LIBS += -lpowrprof -lssl -lcrypto
}

target.path = $${QMAKE_INSTALL_PREFIX}/bin
INSTALLS += target
unix: !android: !macx: INSTALLS += unix_desktop_icon unix_desktop
android: include(C:/android/sdk/android_openssl/openssl.pri)
