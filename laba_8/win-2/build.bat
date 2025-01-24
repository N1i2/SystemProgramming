rmdir /s /q build
mkdir build
cd build
cmake -G "Ninja" -DCMAKE_C_COMPILER=clang ..
cmake --build .