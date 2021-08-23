#!/bin/sh
export QMAKE=/opt/Qt/5.15.2/gcc_64/bin/qmake
export QML_SOURCES_PATHS=/root/atds3-1.5.0/qml
export QML_MODULES_PATHS=/opt/Qt/5.15.2/gcc_64/qml
cd documentos/protobuf/; protoc --cpp_out=. todus.proto
mv todus.pb.h ../../cabeceras/
mv todus.pb.cc ../../fuentes/
cd ../../
mkdir construccion
cd construccion
${QMAKE} ../atds3.pro QMAKE_INSTALL_PREFIX=/root/AppDir/usr CONFIG-=debug CONFIG-=qml_debug CONFIG+=qtquickcompiler CONFIG+=release
make install
cd ../..
# Para la generacion de la imagen AppImage por 1ra vez
# ./linuxdeploy-x86_64.AppImage --appdir AppDir --plugin qt --output appimage
# Para la generacion de la imagen en construcciones en donde ya se ha generado previamente la estructura AppDir
./appimagetool AppDir
