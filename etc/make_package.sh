#!/bin/bash

VERSION=1.0.0
DEB_NAME="ydisk-package_$VERSION.deb"

ROOT=$(pwd)
DEB_ROOT=$ROOT/debian

DEB_DEBIAN=$DEB_ROOT/DEBIAN
DEB_CONTROL=$DEB_DEBIAN/control
DEB_POSTINST=$DEB_DEBIAN/postinst

DEB_USR=$DEB_ROOT/usr
DEB_YDISK=$DEB_USR/bin/ydisk
DEB_SHARE=$DEB_USR/share/ydisk
DEB_RC=$DEB_SHARE/rc
DEB_LOG=$DEB_ROOT/var/log/ydisk
DEB_NAUTILUS=$DEB_USR/lib/nautilus/extensions-3.0

SRC_LIB=../nautilus/libydisk.so
SRC_APPLET=../applet
SRC_RC=$SRC_APPLET/rc

if [ -d $DEB_ROOT ] 
then
	rm -rf $DEB_ROOT
fi

mkdir -p $DEB_DEBIAN
mkdir -p $DEB_YDISK
mkdir -p $DEB_NAUTILUS
mkdir -p $DEB_RC
mkdir -p $DEB_LOG

echo "Package: ydisk
Version: 1.0.0
Section: utils
Priority: extra
Architecture: all
Depends: libc6, libglib2.0-0, libdbus-1-3, libdbus-glib-1-2, libnautilus-extension1 | libnautilus-extension1a, yandex-disk
Maintainer: Novokreshchenov Konstantin <novokrest013@gmail.com>
Description: GUI for YandexDisk" > $DEB_CONTROL

echo "#!/bin/bash
chmod +x /usr/bin/ydisk.sh

ln -s /usr/bin/ydisk.sh /usr/bin/ydisk
echo \"[Desktop Entry]
Name=Ydisk
Comment=GUI for YandexDisk
GenericName=Ydisk
Keywords=Ydisk;ydisk;Yandex;yandexdisk;
Exec=/usr/bin/ydisk.sh
Terminal=false
Type=Application
Icon=/usr/share/ydisk/rc/Ydisk_UFO_main.png
Categories=
Path=/usr/bin/\" > home/$USER/.local/share/applications/ydisk.desktop" > $DEB_POSTINST
chmod 775 $DEB_POSTINST

echo "#!/bin/bash
python /usr/bin/ydisk/main.py" > $DEB_USR/bin/ydisk.sh

touch $DEB_LOG/log.lg

cp $SRC_LIB $DEB_NAUTILUS
cp $SRC_RC/* $DEB_RC
cp $SRC_APPLET/* $DEB_YDISK

dpkg-deb --build $DEB_ROOT ../$DEB_NAME


