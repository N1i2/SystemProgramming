set name="build"

if not exist %name% (
	exit /b 0
)

cd build

cmake --build . --config Release