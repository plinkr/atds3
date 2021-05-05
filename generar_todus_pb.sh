#!/bin/sh
DIR="${1}"
DEST="${2}"
COMPILADOR="${3}"
protoc --cpp_out . --proto_path ${DIR}/documentos/protobuf todus.proto
mv todus.pb.h ${DIR}/cabeceras/
mv todus.pb.cc ${DIR}/fuentes/
${COMPILADOR} -c -pipe -pthread -fPIC -I${DIR}/cabeceras -I/usr/include -I/usr/local/include -o ${DEST}/todus.pb.o ${DIR}/fuentes/todus.pb.cc
exit 0
