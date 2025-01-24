cd ..

set name=build

if not exist %name% (
	call bat/build.bat
)

cd %name%

cmake --build . --config Release

cd Release

for %%f in (*.exe) do (
	start "" "%%f"
)

pause