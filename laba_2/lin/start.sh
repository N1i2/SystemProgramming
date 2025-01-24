if [ -d "build" ]; then
    rm -rf build
fi

cmake -B build
cmake --build ./build

cd build
for f in *.exe; do
    ./"$f" &
done
