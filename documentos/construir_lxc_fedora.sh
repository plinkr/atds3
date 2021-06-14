#!/bin/sh

export PATH=/bin:/sbin:/usr/bin:/usr/sbin

rpmbuild -bb /rpmbuild/SPECS/atds3.spec

exit $?
