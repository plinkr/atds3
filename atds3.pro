QT += svg sql network widgets

windows: CONFIG += embed_manifest_exe

TEMPLATE = app

TARGET = atds3

VERSION = 0.8.0
windows {
	RC_ICONS = recursos/iconos/atds3.ico
	QMAKE_TARGET_COMPANY = ATDS3
	QMAKE_TARGET_DESCRIPTION = Administrador de Transferencias para toDus (S3)
	QMAKE_TARGET_COPYRIGHT = Todos los derechos reservados.
	QMAKE_TARGET_PRODUCT = ATDS3.EXE
	RC_LANG = 0x040A
	RC_CODEPAGE = 0x04B0
}

SOURCES += \
	fuentes/todus.pb.cc \
	fuentes/http1.cpp \
	fuentes/todus.cpp \
	fuentes/modelocategorias.cpp \
	fuentes/modeloentradas.cpp \
	fuentes/descarga.cpp \
	fuentes/gestordescargas.cpp \
	fuentes/delegacioniconoestado.cpp \
	fuentes/delegacionbarraprogreso.cpp \
	fuentes/delegaciontamano.cpp \
	fuentes/ventanaagregardescarga.cpp \
	fuentes/ventanaagregardescargasdesdearchivo.cpp \
	fuentes/ventanaconfiguracion.cpp \
	fuentes/ventanaprincipal.cpp \
	fuentes/main.cpp

HEADERS += \
	cabeceras/todus.pb.h \
	cabeceras/http1.hpp \
	cabeceras/todus.hpp \
	cabeceras/modelocategorias.hpp \
	cabeceras/modeloentradas.hpp \
	cabeceras/descarga.hpp \
	cabeceras/gestordescargas.hpp \
	cabeceras/delegacioniconoestado.hpp \
	cabeceras/delegacionbarraprogreso.hpp \
	cabeceras/delegaciontamano.hpp \
	cabeceras/ventanaagregardescarga.hpp \
	cabeceras/ventanaagregardescargasdesdearchivo.hpp \
	cabeceras/ventanaconfiguracion.hpp \
	cabeceras/ventanaprincipal.hpp \
	cabeceras/main.hpp

RESOURCES += recursos/iconos.qrc

DISTFILES +=	README.md \
				LICENSE \
				atds3.desktop \
				documentos/* \
				documentos/protobuf/* \
				recursos/iconos/* \
				paquetes/freebsd/* \
				paquetes/deb/* \
				paquetes/rpm/* \
				paquetes/arch/*

INCLUDEPATH += cabeceras

unix {
	LIBS += -lssl -lcrypto

	CONFIG(protobuf): LIBS += -lprotobuf
	!CONFIG(protobuf) {
		LIBPROTOBUF =	/usr/local/lib/libprotobuf.a \
						/usr/lib/libprotobuf.a \
						/usr/lib64/libprotobuf.a \
						/usr/lib/x86_64-linux-gnu/libprotobuf.a
		for(ruta, LIBPROTOBUF): exists($${ruta}): LIBS += $${ruta}
	}

	unix_desktop_icon.files = recursos/iconos/atds3.svg
	unix_desktop_icon.path = $${QMAKE_PREFIX}/share/pixmaps
	
	unix_desktop.files = atds3.desktop
	unix_desktop.path = $${QMAKE_PREFIX}/share/applications
}

windows {
	INCLUDEPATH +=	C:/msys64/mingw64/include
	
	LIBS += -LC:/msys64/mingw64/lib \
			-lssl -lcrypto C:/msys64/mingw64/lib/libprotobuf.a
}

target.path = $${QMAKE_PREFIX}/bin
INSTALLS += target
unix: INSTALLS += unix_desktop_icon unix_desktop
