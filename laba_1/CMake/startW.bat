@echo off

mkdir build
cd build

cmake .. -G "Ninja"

cmake --build .

pause

for %%f in (*.exe) do (
	start %%f
)