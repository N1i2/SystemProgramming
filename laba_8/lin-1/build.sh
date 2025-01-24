cd build
rm -rf ./*
cmake -DCMAKE_C_COMPILER=clang ..
cmake --build .
