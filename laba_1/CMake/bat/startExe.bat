cd ..

if not exist build (
	cd bat 
	call build.bat
	cd ..
	cd bat 
	call createExe.bat
) else (
	cd build
)

if not exist Release (
	cd ..
	cd bat 
	call createExe.bat
) else (
	cd Release
)

cls
for %%f in (*.exe) do (
	start "" "%%f"
)