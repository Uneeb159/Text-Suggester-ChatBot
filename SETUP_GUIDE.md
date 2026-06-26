# Visual Studio Setup Guide

This guide will help you set up the AutoComplete Engine project in Visual Studio with all dependencies.

## Prerequisites

1. **Visual Studio 2019 or later** with the following components:
   - C++ CMake tools for Visual Studio
   - MSVC v143 compiler toolset
   - Windows 10/11 SDK

2. **Git** (for cloning vcpkg)

## Step 1: Install vcpkg (Package Manager)

1. Open Command Prompt or PowerShell as Administrator
2. Navigate to your development directory (e.g., `C:\dev\`)
3. Clone vcpkg:
   ```bash
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   ```
4. Bootstrap vcpkg:
   ```bash
   .\bootstrap-vcpkg.bat
   ```
5. Integrate with Visual Studio:
   ```bash
   .\vcpkg integrate install
   ```

## Step 2: Install Dependencies

Install the required packages using vcpkg:

```bash
# Essential for GUI
.\vcpkg install sfml:x64-windows

# Essential for testing
.\vcpkg install gtest:x64-windows

# For property-based testing (optional but recommended)
.\vcpkg install rapidcheck:x64-windows
```

**Note**: This may take 10-30 minutes depending on your internet connection and system.

## Step 3: Open Project in Visual Studio

1. Launch Visual Studio
2. Select **"Open a local folder"**
3. Navigate to and select the AutoComplete Engine project folder
4. Visual Studio will automatically detect `CMakeLists.txt` and configure the project

## Step 4: Configure CMake (if needed)

If Visual Studio doesn't automatically use vcpkg:

1. Go to **Project → CMake Settings**
2. Under **CMake variables**, add:
   - Name: `CMAKE_TOOLCHAIN_FILE`
   - Value: `C:\vcpkg\scripts\buildsystems\vcpkg.cmake`
   - Type: `FILEPATH`

## Step 5: Build the Project

1. Select build configuration (Debug or Release) from the dropdown
2. Press **Ctrl+Shift+B** or go to **Build → Build All**
3. Wait for compilation to complete

## Step 6: Run the Applications

### Console Application
1. In Solution Explorer, right-click on `autocomplete_console.exe`
2. Select **"Set as Startup Item"**
3. Press **F5** or **Ctrl+F5** to run

### GUI Application (if SFML is installed)
1. In Solution Explorer, right-click on `autocomplete_gui.exe`
2. Select **"Set as Startup Item"**
3. Press **F5** or **Ctrl+F5** to run

### Tests (if Google Test is installed)
1. Go to **Test → Test Explorer**
2. Click **"Run All Tests"** or run individual test suites

## Troubleshooting

### CMake Configuration Issues

**Problem**: CMake can't find dependencies
**Solution**: 
- Ensure vcpkg integration is complete: `.\vcpkg integrate install`
- Verify CMake toolchain file path in CMake settings
- Try deleting the `build` folder and reconfiguring

### SFML Not Found

**Problem**: GUI application doesn't build
**Solution**:
- Reinstall SFML: `.\vcpkg install sfml:x64-windows`
- Check that you're building for the correct architecture (x64)

### Tests Not Building

**Problem**: Test executables are missing
**Solution**:
- Install Google Test: `.\vcpkg install gtest:x64-windows`
- Install RapidCheck: `.\vcpkg install rapidcheck:x64-windows`
- Rebuild the project

### Build Errors

**Problem**: Compilation errors
**Solution**:
- Ensure you have C++17 support enabled
- Check that all required Windows SDK components are installed
- Try building in Release mode if Debug mode fails

### Performance Issues

**Problem**: Slow build times
**Solution**:
- Use Release configuration for faster execution
- Close other applications during build
- Consider using an SSD for faster I/O

## Alternative Build Methods

### Command Line Build
```bash
# Using the provided batch file
.\build.bat

# Or manually
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

### PowerShell Build
```powershell
# Using the provided PowerShell script
.\build.ps1
```

## Project Structure After Build

```
AutoCompleteEngine/
├── build/
│   └── bin/
│       └── Release/
│           ├── autocomplete_console.exe
│           ├── autocomplete_gui.exe      (if SFML available)
│           ├── unit_tests.exe           (if GTest available)
│           └── property_tests.exe       (if RapidCheck available)
├── Source files...
└── Configuration files...
```

## Next Steps

Once the project builds successfully:

1. **Test the console application** - Run basic functionality tests
2. **Explore the GUI** - If SFML is available, test the graphical interface
3. **Run the test suite** - Verify all components work correctly
4. **Review the code** - Understand the architecture and implementation
5. **Implement additional features** - Follow the task list in `tasks.md`

## Getting Help

If you encounter issues:

1. Check the **Output** window in Visual Studio for detailed error messages
2. Review the **Error List** for compilation errors
3. Consult the main `README.md` for additional troubleshooting tips
4. Ensure all prerequisites are properly installed

## Performance Notes

- **Debug builds** are slower but provide better debugging information
- **Release builds** are optimized for performance and should be used for testing response times
- The system is designed to handle 10,000+ words with sub-50ms response times in Release mode