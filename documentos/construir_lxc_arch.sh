#!/bin/sh

VERSION="${1}"

export PATH=/bin:/sbin:/usr/bin:/usr/sbin

cd /home/arch/atds3-${VERSION}
makepkg

exit $?
