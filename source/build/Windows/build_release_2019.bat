#"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

set BUILD_NUMBER=672

cd D:\Development
rmdir /S /Q OmegaBuild
mkdir OmegaBuild
cd OmegaBuild

git clone https://github.com/bonezuk/blackomega.git
cd blackomega
git checkout master

cd ..

git clone https://github.com/bonezuk/blackomega_utils.git
cd blackomega_utils
git checkout master

cd ..\blackomega

mkdir build_versioner
cmake "-DTIGER_DEBUG_BUILD:BOOL=OFF" "-DQT_HOME=C:\Qt\6.7.2\msvc2019_64" -G "Ninja" -B .\build_versioner -S .\source\versioner
cd build_versioner
ninja

cd ..
.\Build\Omega\bin\versioner.exe .\source\player\Resources\buildInfo.xml %BUILD_NUMBER% .\source\player\Info.plist .\source\player\Info.plist .\source\player\player.rc .\source\installer\Version.nsh

mkdir build_cmake_win64
cmake "-DTIGER_DEBUG_BUILD:BOOL=OFF" "-DQT_HOME=C:\Qt\6.7.2\msvc2019_64" -G "Ninja" -B .\build_cmake_win64 -S .
cd build_cmake_win64
ninja

cd ..\Build\Omega\bin
move "Omega.exe" "Black Omega.exe"

OmegaUnitTests.exe --gtest_output=xml:..\unit_test_results.xml

signtool sign /n "Open Source Developer, Stuart Andrew MacLean" /t http://time.certum.pl/ /fd SHA256 /v audioio.dll blackomega.dll blueomega.dll common.dll concrt140.dll cyanomega.dll engine.dll ftp.dll greenomega.dll http.dll libiconv.dll libxml2.dll mime.dll msvcp140.dll msvcp140_1.dll msvcp140_2.dll network_omega.dll playercommon.dll Qt6Core.dll Qt6Core5Compat.dll Qt6Gui.dll Qt6Widgets.dll Qt6Xml.dll redomega.dll remote.dll rtp.dll rtp_silveromega.dll silveromega.dll toneomega.dll trackdb.dll trackinfo.dll trackmodel.dll vccorlib140.dll vcruntime140.dll violetomega.dll wavpackdll.dll wavpackomega.dll whiteomega.dll widget.dll "Black Omega.exe" ..\plugins\platforms\qwindows.dll ..\plugins\imageformats\qgif.dll ..\plugins\imageformats\qjpeg.dll ..\plugins\sqldrivers\qsqlite.dll

cd ..\..\..\source\installer
"C:\Program Files (x86)\NSIS\makensis.exe" .\BlackOmega_2019.nsi

cd ..\..\Build\Omega
signtool sign /n "Open Source Developer, Stuart Andrew MacLean" /t http://time.certum.pl/ /fd SHA256 /v BlackOmega-%BUILD_NUMBER%-Installer.exe
