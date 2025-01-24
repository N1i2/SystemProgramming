@echo off

cd ..
set name=build

if exist %name% (
	rmdir -s -q %name%
)

mkdir %name%
cd %name%

cmake ..
cmake --build .

pause