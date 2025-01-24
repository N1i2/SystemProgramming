if exist build (
	rmdir /S /Q build
)

cmake -B build -G "Ninja" -DCMAKE_C_COMPILER=clang
cmake --build ./build
