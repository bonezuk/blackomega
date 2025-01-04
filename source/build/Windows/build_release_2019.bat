"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

set BUILD_NUMBER=670

cd D:\Development\OmegaBuild
D:
rmdir /s /q *

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
.\Build\Omega\bin\versioner.exe .\source\player\Resources\buildInfo.xml %BUILD_NUMBER% .\source\player\Info.plist .\source\player\Info.plist .\source\player\player.rc .\source\installer\Version.nsh ".\source\help\apple\Black Omega.help\Contents\Info.plist" ".\source\help\apple\Black Omega.help\Contents\Info.plist"
.\Build\Omega\bin\versioner.exe .\source\player\Resources\buildInfo.xml %BUILD_NUMBER% .\source\player\appstore\Info.plist .\source\player\appstore\Info.plist .\source\player\player.rc .\source\installer\Version.nsh ".\source\help\appleStore\Black Omega.help\Contents\Info.plist" ".\source\help\appleStore\Black Omega.help\Contents\Info.plist"

mkdir build_cmake_win64
cmake "-DTIGER_DEBUG_BUILD:BOOL=OFF" "-DQT_HOME=C:\Qt\6.7.2\msvc2019_64" -G "Ninja" -B .\build_cmake_win64 -S .
cd build_cmake_win64
ninja

cd ..\source\help
C:\Qt\6.7.2\msvc2019_64\bin\qhelpgenerator.exe ".\windows\help.qhcp" -o ".\windows\BlackOmega.qhc"
xcopy /Y ".\windows\BlackOmega.qch" "..\..\Build\Omega\help"
xcopy /Y ".\windows\BlackOmega.qhc" "..\..\Build\Omega\help"

cd ..\..\Build\Omega\bin
move "Omega.exe" "Black Omega.exe"

cd ..\..\..\source\installer
"C:\Program Files (x86)\NSIS\makensis.exe" .\BlackOmega_2019.nsi
