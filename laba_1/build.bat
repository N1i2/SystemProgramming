set name="build"

if exist %name% (
	del /q %name%
)

mkdir build
cd build
cmake ..