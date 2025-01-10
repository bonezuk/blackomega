#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <0 or 1 is APP Store build>"
    exit 1
fi

export BUILD_NUMBER=670
export QT_HOME=~/Qt/6.7.0/macos
export IS_APP_STORE=$1

cd ~/Development
if [ -d "./OmegaBuild" ]; then
	rm -rf OmegaBuild
fi
mkdir OmegaBuild
cd OmegaBuild

git clone https://github.com/bonezuk/blackomega.git
cd blackomega
git checkout master

cd ..
git clone https://github.com/bonezuk/blackomega_utils.git
cd blackomega_utils
git checkout master

cd ../blackomega

mkdir build_versioner
cmake "-DTIGER_DEBUG_BUILD:BOOL=OFF" "-DQT_HOME=$QT_HOME" -G "Unix Makefiles" -B ./build_versioner -S ./source/versioner
cd build_versioner
make -j 12

cd ..
mkdir build_cmake_macos

if [ $IS_APP_STORE -eq 0 ]; then
	echo "Build Website version"
	./build/Omega/bin/versioner ./source/player/Resources/buildInfo.xml $BUILD_NUMBER ./source/player/Info.plist ./source/player/Info.plist ./source/player/player.rc ./source/installer/Version.nsh
	cmake "-DTIGER_DEBUG_BUILD:BOOL=OFF" "-DQT_HOME=$QT_HOME" "-DTIGER_MAC_STORE:BOOL=OFF" -G "Unix Makefiles" -B ./build_cmake_macos -S .
else
	echo "Build APP Store version"
	./build/Omega/bin/versioner ./source/player/Resources/buildInfo.xml $BUILD_NUMBER ./source/player/appstore/Info.plist ./source/player/appstore/Info.plist ./source/player/player.rc ./source/installer/Version.nsh
	cmake "-DTIGER_DEBUG_BUILD:BOOL=OFF" "-DQT_HOME=$QT_HOME" "-DTIGER_MAC_STORE:BOOL=ON" -G "Unix Makefiles" -B ./build_cmake_macos -S .
fi

cd build_cmake_macos
make -j 12

cd ../source/build/MacOS
python3 ./build_macosx_app_qt6_bundle.py $IS_APP_STORE
