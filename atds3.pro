windows: QMAKE_CXXFLAGS_DEBUG += /std:c++17

QT += core gui sql network widgets

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
#windows: RC_ICONS += recursos/iconos/atds3.svg
#windows:RC_FILE = recursos/atds3.rc

DISTFILES +=	README.md \
				LICENSE \
				atds3.desktop

unix: INCLUDEPATH += /usr/local/include
windows: INCLUDEPATH += C:\\Qt\\vcpkg\\packages\\openssl_x64-windows\\include \
						C:\\Qt\\vcpkg\\packages\\protobuf_x64-windows\\include
INCLUDEPATH += cabeceras

unix: LIBS += -lssl -lcrypto -lprotobuf
windows: LIBS +=	-LC:\\Qt\\vcpkg\\packages\\openssl_x64-windows\\lib \
					-LC:\\Qt\\vcpkg\\packages\\protobuf_x64-windows\\lib \
#windows: LIBS +=	-LC:\\Qt\\vcpkg\\packages\\openssl_x64-windows\\debug\\lib \
#					-LC:\\Qt\\vcpkg\\packages\\protobuf_x64-windows\\debug\\lib \
					-llibssl -llibcrypto -llibprotobuf

#windows: INCLUDEPATH += C:\Qt\Tools\OpenSSL\Win_x64\include C:\Qt\libraries\protobuf-3.17.0\install\x64\include
#windows: LIBS += -LC:\Qt\Tools\OpenSSL\Win_x64\lib -LC:\Qt\libraries\protobuf-3.17.0\install\x64\lib -llibssl -llibcrypto -llibprotobuf

#PHONY_DEPS = .
#GenerarTodusPB.input = PHONY_DEPS
#GenerarTodusPB.output = todus.pb.o
#unix: GenerarTodusPB.commands = $$PWD/generar_todus_pb.sh "$$PWD" "$$OUT_PWD" "$$QMAKE_CXX"
#windows: GenerarTodusPB.commands = $$PWD/generar_todus_pb.bat "$$PWD" "$$OUT_PWD" "$$QMAKE_CXX" "C:\Qt\libraries\openssl-1.1.1k-x64\build\OpenSSL" "C:\Qt\libraries\protobuf-3.17.0\install\x64"
#GenerarTodusPB.CONFIG += target_predeps
#QMAKE_EXTRA_COMPILERS += GenerarTodusPB

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
!isEmpty(target.path): INSTALLS += target
unix: INSTALLS += unix_desktop_icon unix_desktop
