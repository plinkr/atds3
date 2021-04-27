QT       += core gui sql network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

SOURCES += \
	fuentes/modeloentradas.cpp \
	fuentes/delegacioniconoestado.cpp \
	fuentes/delegacionbarraprogreso.cpp \
	fuentes/delegacionvelocidad.cpp \
	fuentes/ventanaagregardescarga.cpp \
	fuentes/ventanaprincipal.cpp \
	fuentes/main.cpp

HEADERS += \
	cabeceras/modeloentradas.hpp \
	cabeceras/delegacioniconoestado.hpp \
	cabeceras/delegacionbarraprogreso.hpp \
	cabeceras/delegacionvelocidad.hpp \
	cabeceras/ventanaagregardescarga.hpp \
	cabeceras/ventanaprincipal.hpp \
	cabeceras/main.hpp

RESOURCES += \
	recursos/iconos.qrc

DISTFILES += README.md LICENSE.txt

INCLUDEPATH += cabeceras

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
