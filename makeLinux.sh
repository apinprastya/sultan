#!/bin/sh

LINUX_DEPLOY_LINUX=/home/apin/Downloads/linuxdeploy-x86_64.AppImage
APPIMAGE_BUILDER=/home/apin/Downloads/appimage-builder-x86_64.AppImage

chmod +x $LINUX_DEPLOY_LINUX
chmod +x $APPIMAGE_BUILDER

mkdir buildLinux
cd buildLinux

qmake CONFIG+=release CONFIG+=SINGLEBIN ../sultan.pro
make -j8
make install INSTALL_ROOT=${PWD}/AppDir

if [ $? -ne 0 ]; then
    echo "error building, exiting"
    exit 255
fi

mkdir -p ${PWD}/AppDir/usr/share/applications/
mkdir -p ${PWD}/AppDir/usr/share/icons/

cp ../linux/sultan.desktop ${PWD}/AppDir/usr/share/applications/
cp ../linux/sultan.png ${PWD}/AppDir/usr/share/icons/

$LINUX_DEPLOY_LINUX --appdir ${PWD}/AppDir/ --plugin qt

if [ $? -ne 0 ]; then
    echo "error building, exiting"
    exit 255
fi

echo "success setting up linux deploy"

rm ${PWD}/AppDir/sultan.png

cp -R ${PWD}/AppDir/usr/plugins/* ${PWD}/AppDir/usr/bin/

$APPIMAGE_BUILDER --appdir ${PWD}/AppDir --recipe ../linux/AppImageBuilder_x86_64.yml 
