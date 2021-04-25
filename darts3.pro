QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

SOURCES += \
	fuentes/main.cpp \
	fuentes/ventanaprincipal.cpp

HEADERS += \
	cabeceras/main.hpp \
	cabeceras/ventanaprincipal.hpp

INCLUDEPATH += cabeceras

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
