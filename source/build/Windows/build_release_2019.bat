"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

cd D:\Development\OmegaBuild
rmdir /s /q *

git clone https://github.com/bonezuk/blackomega.git
cd blackomega
git checkout master

cd ..

git clone https://github.com/bonezuk/blackomega_utils.git
cd blackomega_utils
git checkout master

cd ..\blackomega
mkdir build_cmake_win64

cmake "-DTIGER_DEBUG_BUILD:BOOL=OFF" "-DQT_HOME=C:\Qt\6.7.2\msvc2019_64" -G "Ninja" -B .\build_cmake_win64 -S .
cd build_cmake_win64
nmake
