windows: QMAKE_CXXFLAGS_DEBUG += /std:c++17

QT += svg sql network widgets

windows: CONFIG += embed_manifest_exe

TEMPLATE = app

TARGET = atds3

VERSION = 0.2.0
windows: RC_ICONS = recursos/iconos/atds3.ico
windows: QMAKE_TARGET_COMPANY = ATDS3
windows: QMAKE_TARGET_DESCRIPTION = Administrador de Transferencias para toDus (S3)
windows: QMAKE_TARGET_COPYRIGHT = Todos los derechos reservados.
windows: QMAKE_TARGET_PRODUCT = ATDS3.EXE
windows: RC_LANG = es_ES

unix: SOURCES += fuentes/todus_unix.pb.cc
windows: SOURCES += fuentes/todus_windows.pb.cc
SOURCES += \
	fuentes/todus.cpp \
	fuentes/modelocategorias.cpp \
	fuentes/modeloentradas.cpp \
	fuentes/descarga.cpp \
	fuentes/gestordescargas.cpp \
	fuentes/delegacioniconoestado.cpp \
	fuentes/delegacionbarraprogreso.cpp \
	fuentes/delegacionvelocidad.cpp \
	fuentes/ventanaagregardescarga.cpp \
	fuentes/ventanaagregardescargasdesdearchivo.cpp \
	fuentes/ventanaconfiguracion.cpp \
	fuentes/ventanaprincipal.cpp \
	fuentes/main.cpp

unix: HEADERS += cabeceras/todus_unix.pb.h
windows: HEADERS += cabeceras/todus_windows.pb.h
HEADERS += \
	cabeceras/todus.hpp \
	cabeceras/modelocategorias.hpp \
	cabeceras/modeloentradas.hpp \
	cabeceras/descarga.hpp \
	cabeceras/gestordescargas.hpp \
	cabeceras/delegacioniconoestado.hpp \
	cabeceras/delegacionbarraprogreso.hpp \
	cabeceras/delegacionvelocidad.hpp \
	cabeceras/ventanaagregardescarga.hpp \
	cabeceras/ventanaagregardescargasdesdearchivo.hpp \
	cabeceras/ventanaconfiguracion.hpp \
	cabeceras/ventanaprincipal.hpp \
	cabeceras/main.hpp

windows: DEFINES += PROTOBUF_USE_DLLS

RESOURCES += recursos/iconos.qrc

DISTFILES +=	README.md \
				LICENSE \
				atds3.desktop

unix: INCLUDEPATH += /usr/local/include
windows: INCLUDEPATH += C:\\Qt\\vcpkg\\packages\\openssl_x64-windows\\include \
						C:\\Qt\\vcpkg\\packages\\protobuf_x64-windows\\include
INCLUDEPATH += cabeceras

unix: LIBS += -lssl -lcrypto -lprotobuf
windows:isEqual(QMAKE_HOST.arch, x86): {
	contains(CONFIG, debug): {
		LIBS +=	-LC:\\Qt\\vcpkg\\packages\\openssl_x86-windows\\debug\\lib \
				-LC:\\Qt\\vcpkg\\packages\\protobuf_x86-windows\\debug\\lib \
				-llibprotobufd
	}
	contains(CONFIG, release): {
		LIBS +=	-LC:\\Qt\\vcpkg\\packages\\openssl_x86-windows\\lib \
				-LC:\\Qt\\vcpkg\\packages\\protobuf_x86-windows\\lib \
				-llibprotobuf
	}
}
windows:isEqual(QMAKE_HOST.arch, x86_64): {
	contains(CONFIG, debug): {
		LIBS +=	-LC:\\Qt\\vcpkg\\packages\\openssl_x64-windows\\debug\\lib \
				-LC:\\Qt\\vcpkg\\packages\\protobuf_x64-windows\\debug\\lib \
				-llibprotobufd
	}
	contains(CONFIG, release): {
		LIBS +=	-LC:\\Qt\\vcpkg\\packages\\openssl_x64-windows\\lib \
				-LC:\\Qt\\vcpkg\\packages\\protobuf_x64-windows\\lib \
				-llibprotobuf
	}
}
windows: LIBS += -llibssl -llibcrypto

unix: unix_desktop_icon.path = /usr/local/share/pixmaps
unix:linux: unix_desktop_icon.path = /usr/share/pixmaps
unix_desktop_icon.files = recursos/iconos/atds3.svg
unix: unix_desktop.path = /usr/local/share/applications
unix:linux: unix_desktop.path = /usr/share/applications
unix_desktop.files = atds3.desktop

# Default rules for deployment.
unix: target.path = /usr/local/bin
unix:linux: target.path = /usr/bin
windows: target.path = %ProgramFiles%\ATDS3
!isEmpty(target.path): {
	INSTALLS += target
	unix: INSTALLS += unix_desktop_icon unix_desktop
}
