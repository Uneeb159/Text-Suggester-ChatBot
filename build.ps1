# PowerShell build script for AutoComplete Engine

Write-Host "Building AutoComplete Engine..." -ForegroundColor Green

# Create build directory if it doesn't exist
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Path "build"
}

# Configure CMake with vcpkg toolchain
Write-Host "Configuring CMake..." -ForegroundColor Yellow
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    exit 1
}

# Build the project
Write-Host "Building project..." -ForegroundColor Yellow
cmake --build build --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Build complete! Executables are in build/bin/Release/" -ForegroundColor Green
Write-Host ""
Write-Host "Available executables:" -ForegroundColor Cyan

if (Test-Path "build/bin/Release/autocomplete_console.exe") {
    Write-Host "- autocomplete_console.exe (Console version)" -ForegroundColor White
}

if (Test-Path "build/bin/Release/autocomplete_gui.exe") {
    Write-Host "- autocomplete_gui.exe (GUI version)" -ForegroundColor White
}

if (Test-Path "build/bin/Release/unit_tests.exe") {
    Write-Host "- unit_tests.exe (Unit tests)" -ForegroundColor White
}

if (Test-Path "build/bin/Release/property_tests.exe") {
    Write-Host "- property_tests.exe (Property tests)" -ForegroundColor White
}

Write-Host ""
Write-Host "To run the console version: ./build/bin/Release/autocomplete_console.exe" -ForegroundColor Yellow