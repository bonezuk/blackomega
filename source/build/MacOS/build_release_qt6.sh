#!/bin/bash

export BUILD_NUMBER=670
export QT_HOME=~/Qt/6.7.0/macos

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
make

cd ..
./build/Omega/bin/versioner ./source/player/Resources/buildInfo.xml $BUILD_NUMBER ./source/player/Info.plist ./source/player/Info.plist ./source/player/player.rc ./source/installer/Version.nsh "./source/help/apple/Black Omega.help/Contents/Info.plist" "./source/help/apple/Black Omega.help/Contents/Info.plist"
./build/Omega/bin/versioner ./source/player/Resources/buildInfo.xml $BUILD_NUMBER ./source/player/appstore/Info.plist ./source/player/appstore/Info.plist ./source/player/player.rc ./source/installer/Version.nsh "./source/help/appleStore/Black Omega.help/Contents/Info.plist" "./source/help/appleStore/Black Omega.help/Contents/Info.plist"
