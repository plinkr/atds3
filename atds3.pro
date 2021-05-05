QT       += core gui sql network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

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
	cabeceras/ventanaagregardescargasdesdearchivos.hpp \
	cabeceras/ventanaconfiguracion.hpp \
	cabeceras/ventanaprincipal.hpp \
	cabeceras/main.hpp

RESOURCES += \
	recursos/iconos.qrc

DISTFILES += README.md LICENSE.txt generar_todus_pb.sh

INCLUDEPATH += \
	/usr/local/include \
	cabeceras

LIBS += -lssl -lcrypto -lprotobuf

PHONY_DEPS = .
GenerarTodusPB.input = PHONY_DEPS
GenerarTodusPB.output = todus.pb.o
unix: GenerarTodusPB.commands = $$PWD/generar_todus_pb.sh "$$PWD" "$$OUT_PWD" "$$QMAKE_CXX"
windows: GenerarTodusPB.commands = $$PWD/generar_todus_pb.bat "$$PWD" "$$OUT_PWD" "$$QMAKE_CXX"
GenerarTodusPB.SOURCES += fuentes/todus.pb.cc
GenerarTodusPB.HEADERS += cabeceras/todus.pb.h
GenerarTodusPB.CONFIG += target_predeps
QMAKE_EXTRA_COMPILERS += GenerarTodusPB

# Default rules for deployment.
unix: target.path = /usr/local/bin
unix:linux: target.path = /usr/bin
!isEmpty(target.path): INSTALLS += target
