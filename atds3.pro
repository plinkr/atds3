windows: QMAKE_CXXFLAGS_DEBUG += /std:c++17

QT += svg sql network widgets

windows: CONFIG += embed_manifest_exe

TEMPLATE = app

TARGET = atds3

VERSION = 0.4.0
windows: RC_ICONS = recursos/iconos/atds3.ico
windows: QMAKE_TARGET_COMPANY = ATDS3
windows: QMAKE_TARGET_DESCRIPTION = Administrador de Transferencias para toDus (S3)
windows: QMAKE_TARGET_COPYRIGHT = Todos los derechos reservados.
windows: QMAKE_TARGET_PRODUCT = ATDS3.EXE
windows: RC_LANG = es_ES

SOURCES += \
	fuentes/todus.pb.cc \
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

HEADERS += \
	cabeceras/todus.pb.h \
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
				atds3.desktop \
				recursos/iconos/* \
				paquetes/freebsd/* \
				paquetes/deb/* \
				paquetes/rpm/* \
				paquetes/arch/*

unix: INCLUDEPATH += /usr/include /usr/local/include
windows: INCLUDEPATH += C:\\Qt\\vcpkg\\packages\\openssl_x64-windows\\include \
						C:\\Qt\\vcpkg\\packages\\protobuf_x64-windows\\include \
						C:\\Qt\\vcpkg\\packages\\sqlite3_x64-windows\\include
INCLUDEPATH += cabeceras

unix: LIBS += -lssl -lcrypto -lprotobuf -lsqlite3
windows: LIBS += -LC:\\Qt\\vcpkg\\packages\\openssl_x64-windows\\lib \
		-LC:\\Qt\\vcpkg\\packages\\protobuf_x64-windows\\lib \
		-LC:\\Qt\\vcpkg\\packages\\sqlite3_x64-windows\\lib \
		-llibssl -llibcrypto -llibprotobuf -lsqlite3

unix: unix_desktop_icon.path = /usr/local/share/pixmaps
unix:linux: unix_desktop_icon.path = /usr/share/pixmaps
unix_desktop_icon.files = recursos/iconos/atds3.svg
unix: unix_desktop.path = /usr/local/share/applications
unix:linux: unix_desktop.path = /usr/share/applications
unix_desktop.files = atds3.desktop

# Default rules for deployment.
#unix: target.path = /usr/local/bin
#unix:linux: target.path = /usr/bin
#windows: target.path = %ProgramFiles%\ATDS3

INSTALLS += target
unix: INSTALLS += unix_desktop_icon unix_desktop
