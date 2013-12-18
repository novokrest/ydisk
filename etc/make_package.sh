#!/bin/bash

VERSION=1.0.0
DEB_NAME="ydisk-package_$VERSION.deb"

ROOT=$(pwd)
DEB_ROOT=$ROOT/debian

DEB_DEBIAN=$DEB_ROOT/DEBIAN
DEB_CONTROL=$DEB_DEBIAN/control
DEB_POSTINST=$DEB_DEBIAN/postinst
DEB_POSTRM=$DEB_DEBIAN/postrm

DEB_USR=$DEB_ROOT/usr
DEB_YDISK=$DEB_USR/bin/ydisk
DEB_SHARE=$DEB_USR/share/ydisk
DEB_RC=$DEB_SHARE/rc
DEB_NAUTILUS=$DEB_USR/lib/nautilus/extensions-3.0

SRC_LIB=../nautilus/libydisk.so
SRC_APPLET=../applet
SRC_RC=../rc

VAR_DIR=/var/log/ydisk

if [ -d $DEB_ROOT ] 
then
	sudo rm -rf $DEB_ROOT
fi

mkdir -p $DEB_DEBIAN
mkdir -p $DEB_YDISK
mkdir -p $DEB_NAUTILUS
mkdir -p $DEB_RC

echo "Package: ydisk
Version: 1.0.0
Section: utils
Priority: extra
Architecture: amd64
Depends: libc6, libglib2.0-0, libdbus-1-3, libdbus-glib-1-2, libnautilus-extension1 | libnautilus-extension1a, yandex-disk, python-gtk2, python-appindicator, python-dbus, python-loggingx
Maintainer: Novokreshchenov Konstantin <novokrest013@gmail.com>
Description: GUI for YandexDisk
Installed-size: 425" > $DEB_CONTROL

echo "#!/bin/bash
chmod +x /usr/bin/ydisk.sh
chmod 755 /usr/bin/ydisk/*
chmod 664 /usr/share/ydisk/rc/*

if [ ! -d $VAR_DIR ]; then
	mkdir -p $VAR_DIR
fi
chmod 777 -R $VAR_DIR

nautilus -q
nautilus -n

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
Path=/usr/bin/\" > home/$USER/.local/share/applications/ydisk.desktop " > $DEB_POSTINST
chmod 755 $DEB_POSTINST

echo "#!/bin/bash
python /usr/bin/ydisk/main.py" > $DEB_USR/bin/ydisk.sh

cp $SRC_LIB $DEB_NAUTILUS
cp -R $SRC_RC/* $DEB_RC
cp -R $SRC_APPLET/* $DEB_YDISK

sudo chown -R root:root $DEB_ROOT
sudo dpkg-deb --build $DEB_ROOT ../$DEB_NAME
sudo rm -rf $DEB_ROOT
