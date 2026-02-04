#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.15.163
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-
SYSROOT="$(${CROSS_COMPILE}gcc -print-sysroot)"

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p "${OUTDIR}"

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    # Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e "${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image" ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # Kernel build steps
    sudo apt-get update && sudo apt-get install -y --no-install-recommends \
      bc u-boot-tools kmod cpio flex bison libssl-dev psmisc && \
      sudo apt-get install -y qemu-system-arm

    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} mrproper
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig
    make -j4 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} all
    make -j4 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} modules
fi

echo "Adding the Image in outdir"
cp "${OUTDIR}/linux-stable/arch/arm64/boot/Image" "${OUTDIR}/Image"

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm -rf "${OUTDIR}/rootfs"
fi

# Create necessary base directories
mkdir -p rootfs
cd rootfs
mkdir -p bin dev etc home lib lib64 proc sbin sys tmp usr var
mkdir -p usr/bin usr/lib usr/sbin
mkdir -p var/log

cd "${OUTDIR}"
if [ ! -d "${OUTDIR}/busybox" ]
then
    git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
else
    cd busybox
fi

# TODO: Make and install busybox
make distclean
make defconfig
make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
make CONFIG_PREFIX=${OUTDIR}/rootfs ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install

echo "Library dependencies"
cd "${OUTDIR}/rootfs"
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

# Add library dependencies to rootfs
interpreter=$(${CROSS_COMPILE}readelf -a bin/busybox | sed -n 's/.*program interpreter: \(.*\)]/\1/p')
interpreter_base="$(basename "${interpreter}")"
file=$(find "${SYSROOT}" -name "${interpreter_base}" | head -n 1 || true)

if [ -z "${file}" ]; then

    echo "ERROR: Could not find ${interpreter} in toolchain"
    exit 1
fi

cp -a "${file}" "${OUTDIR}/rootfs/lib"

for lib in $(${CROSS_COMPILE}readelf -a bin/busybox | sed -n 's/.*Shared library: \[\(.*\)]/\1/p'); do
    file=$(find "${SYSROOT}" -name "${lib}" | head -n 1 || true)
    if [ -z "${file}" ]; then

        echo "ERROR: Could not find ${lib} in toolchain"
        exit 1
    fi

    cp -a "${file}" "${OUTDIR}/rootfs/lib64"

done

# Make device nodes
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 666 dev/console c 5 1

# Clean and build the writer utility
cd "${FINDER_APP_DIR}"
make clean
make CROSS_COMPILE=${CROSS_COMPILE}

# Copy the finder related scripts and executables to the /home directory
# on the target rootfs
cp finder.sh "${OUTDIR}/rootfs/home"
cp finder-test.sh "${OUTDIR}/rootfs/home"
cp writer "${OUTDIR}/rootfs/home"
mkdir -p "${OUTDIR}/rootfs/home/conf"
cp conf/* "${OUTDIR}/rootfs/home/conf"
cp autorun-qemu.sh "${OUTDIR}/rootfs/home"

# Chown the root directory
sudo chown -R root:root "${OUTDIR}/rootfs"

# Create initramfs.cpio.gz
cd "${OUTDIR}/rootfs"
find . | cpio -H newc -ov --owner root:root > "${OUTDIR}/initramfs.cpio"
gzip -f "${OUTDIR}/initramfs.cpio"
