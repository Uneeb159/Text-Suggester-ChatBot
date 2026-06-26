@echo off
echo Building AutoComplete Engine...

REM Create build directory if it doesn't exist
if not exist build mkdir build

REM Configure CMake with vcpkg toolchain
echo Configuring CMake...
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

REM Build the project
echo Building project...
cmake --build build --config Release

echo.
echo Build complete! Executables are in build/bin/Release/
echo.
echo Available executables:
echo - autocomplete_console.exe (Console version)
if exist "build\bin\Release\autocomplete_gui.exe" echo - autocomplete_gui.exe (GUI version)
if exist "build\bin\Release\unit_tests.exe" echo - unit_tests.exe (Unit tests)
if exist "build\bin\Release\property_tests.exe" echo - property_tests.exe (Property tests)

pause