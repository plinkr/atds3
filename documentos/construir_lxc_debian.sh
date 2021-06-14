#!/bin/sh

VERSION="${1}"

cd /root/atds3-${VERSION}
debuild -b

exit $?
