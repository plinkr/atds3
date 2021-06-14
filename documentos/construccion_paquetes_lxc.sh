#!/bin/sh

DIRECTORIO_INICIAL="$(pwd)"
LXC_RAIZ="/var/lib/lxc"
LXC_DEBIAN="atds3-debian"
LXC_FEDORA="atds3-fedora"
LXC_ARCH="atds3-arch"
VERSION="${1}"
DIRECTORIO_PAQUETES="paquetes"

mkdir -p "${DIRECTORIO_PAQUETES}" 2&>1 > /dev/null

## Debian/Ubuntu
cp construir_lxc_debian.sh ${LXC_RAIZ}/${LXC_DEBIAN}/rootfs/root/
chmod +x ${LXC_RAIZ}/${LXC_DEBIAN}/rootfs/root/construir_lxc_debian.sh
cp atds3-${VERSION}.tar.gz ${LXC_RAIZ}/${LXC_DEBIAN}/rootfs/root/atds3_${VERSION}.orig.tar.gz
cd ${LXC_RAIZ}/${LXC_DEBIAN}/rootfs/root
tar xf atds3_${VERSION}.orig.tar.gz
cd ${DIRECTORIO_INICIAL}
cp -R deb ${LXC_RAIZ}/${LXC_DEBIAN}/rootfs/root/atds3-${VERSION}/debian
lxc-execute --name="${LXC_DEBIAN}" -- /root/construir_lxc_debian.sh ${VERSION}
cp ${LXC_RAIZ}/${LXC_DEBIAN}/rootfs/root/atds3_0.8.0-1_amd64.deb "${DIRECTORIO_PAQUETES}"/

## Fedora/CentOS/RHEL
cp construir_lxc_fedora.sh ${LXC_RAIZ}/${LXC_FEDORA}/rootfs/root/
chmod +x ${LXC_RAIZ}/${LXC_FEDORA}/rootfs/root/construir_lxc_fedora.sh
cp atds3-${VERSION}.tar.gz ${LXC_RAIZ}/${LXC_FEDORA}/rootfs/rpmbuild/SOURCES/
cp rpm/atds3.spec ${LXC_RAIZ}/${LXC_FEDORA}/rootfs/rpmbuild/SPECS/
lxc-execute --name="${LXC_FEDORA}" -- /root/construir_lxc_fedora.sh
cp ${LXC_RAIZ}/${LXC_FEDORA}/rootfs/rpmbuild/RPMS/x86_64/atds3-${VERSION}-1.fc34.x86_64.rpm "${DIRECTORIO_PAQUETES}"/

## Arch/Manjaro
cp construir_lxc_arch.sh ${LXC_RAIZ}/${LXC_ARCH}/rootfs/home/arch/
chmod +x ${LXC_RAIZ}/${LXC_ARCH}/rootfs/home/arch/construir_lxc_arch.sh
mkdir ${LXC_RAIZ}/${LXC_ARCH}/rootfs/home/arch/atds3-${VERSION}
chown -R 1000:1000 ${LXC_RAIZ}/${LXC_ARCH}/rootfs/home/arch/atds3-${VERSION}
cp atds3-${VERSION}.tar.gz ${LXC_RAIZ}/${LXC_ARCH}/rootfs/home/arch/atds3-${VERSION}/
cp arch/* ${LXC_RAIZ}/${LXC_ARCH}/rootfs/home/arch/atds3-${VERSION}/
lxc-execute --name="${LXC_ARCH}" -- su -l arch -c "/home/arch/construir_lxc_arch.sh ${VERSION}"
cp ${LXC_RAIZ}/${LXC_ARCH}/rootfs/home/arch/atds3-${VERSION}/atds3-${VERSION}-1-x86_64.pkg.tar.zst "${DIRECTORIO_PAQUETES}"/

exit 0
