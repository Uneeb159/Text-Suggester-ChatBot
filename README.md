# AutoComplete Engine

A high-performance text suggestion engine built with C++ using advanced data structures including Trie, Heaps, and Hash Maps. Features real-time text prediction with learning capabilities and an interactive GUI built with SFML.

## Features

- **Real-time Suggestions**: Sub-50ms response time for text predictions
- **Learning System**: Adapts to user behavior and frequently used words
- **Custom Dictionary**: Add and manage custom words
- **Graphical Interface**: Interactive SFML-based GUI
- **Console Interface**: Command-line version for testing
- **Persistent Storage**: Saves dictionary and frequency data between sessions

## Prerequisites

### Required
- Visual Studio 2019 or later with C++17 support
- CMake 3.16 or higher

### Optional (for full functionality)
- **SFML 2.5+** (for GUI version)
- **Google Test** (for unit tests)
- **RapidCheck** (for property-based tests)

## Quick Setup with vcpkg (Recommended)

1. Install vcpkg if you haven't already:
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
```

2. Install dependencies:
```bash
.\vcpkg install sfml:x64-windows
.\vcpkg install gtest:x64-windows
.\vcpkg install rapidcheck:x64-windows
```

## Building

### Option 1: Visual Studio (Recommended)
1. Open Visual Studio
2. Select "Open a local folder" and choose this project directory
3. Visual Studio will automatically detect CMakeLists.txt
4. Build the project (Ctrl+Shift+B)

### Option 2: Command Line
```bash
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

## Running

### Console Version (Always Available)
```bash
./build/bin/Release/autocomplete_console.exe
```

### GUI Version (If SFML is installed)
```bash
./build/bin/Release/autocomplete_gui.exe
```

### Tests (If Google Test/RapidCheck are installed)
```bash
./build/bin/Release/unit_tests.exe
./build/bin/Release/property_tests.exe
```

## Usage

### Console Interface
- Type a prefix to get suggestions
- Use `add <word>` to add custom words
- Select suggestions by number (1-5)
- Type `quit` or `exit` to close

### GUI Interface
- Type in the input field to see real-time suggestions
- Click on suggestions to select them
- Use arrow keys to navigate suggestions
- Press Enter to select highlighted suggestion

### Adding Custom Words
The system validates words to contain only:
- Letters (a-z, A-Z)
- Apostrophes (')
- Hyphens (-)

## Architecture

The system uses a modular architecture:

```
┌─────────────────────────────────────────┐
│              GUI Layer (SFML)           │
│  ┌─────────────┐  ┌─────────────────┐   │
│  │ Input Field │  │ Suggestion List │   │
│  └─────────────┘  └─────────────────┘   │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│           Business Logic Layer          │
│  ┌─────────────────┐ ┌─────────────────┐│
│  │ AutoComplete    │ │ Suggestion      ││
│  │ Engine          │ │ Ranker          ││
│  └─────────────────┘ └─────────────────┘│
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│            Data Layer                   │
│  ┌─────────────┐ ┌─────────────────────┐│
│  │ Trie        │ │ Frequency Tracker & ││
│  │ Structure   │ │ Dictionary Manager  ││
│  └─────────────┘ └─────────────────────┘│
└─────────────────────────────────────────┘
```

### Core Components
- **Trie Structure**: Efficient prefix-based word storage and retrieval
- **Frequency Tracker**: Monitors and persists word usage patterns
- **Suggestion Ranker**: Ranks suggestions by frequency, recency, and alphabetical order
- **Dictionary Manager**: Handles persistence, loading, and word validation
- **AutoComplete Engine**: Coordinates all components and provides main API

## Testing

The project includes comprehensive testing:
- **Unit Tests**: Test individual components with specific examples
- **Property-Based Tests**: Verify correctness across random inputs
- **Performance Tests**: Validate response time requirements

### Running Tests
```bash
# Unit tests
./build/bin/Release/unit_tests.exe

# Property-based tests
./build/bin/Release/property_tests.exe
```

## Project Structure

```
AutoCompleteEngine/
├── AutoCompleteEngine.h          # Core header with all class definitions
├── AutoCompleteEngine.cpp        # Implementation of all classes
├── main_console.cpp              # Console application entry point
├── main_gui.cpp                  # GUI application entry point (SFML)
├── CMakeLists.txt               # Build system configuration
├── tests/
│   ├── unit_tests.cpp           # Unit tests (Google Test)
│   └── property_tests.cpp       # Property-based tests (RapidCheck)
├── src/                         # Additional source files (if needed)
├── build/                       # Build output directory
└── README.md                    # This file
```

## Troubleshooting

### SFML Not Found
- Install SFML via vcpkg: `vcpkg install sfml:x64-windows`
- Or download from https://www.sfml-dev.org/ and set SFML_ROOT

### Tests Not Building
- Install Google Test: `vcpkg install gtest:x64-windows`
- Install RapidCheck: `vcpkg install rapidcheck:x64-windows`

### CMake Issues
- Ensure vcpkg toolchain is specified: `-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake`
- Update CMake to version 3.16 or higher

### Visual Studio Issues
- Make sure "C++ CMake tools for Visual Studio" is installed
- Open the folder (not a .sln file) in Visual Studio
- Let Visual Studio configure CMake automatically

## Performance

- **Response Time**: < 50ms for dictionaries up to 10,000 words
- **Memory Usage**: Efficient trie structure with shared prefixes
- **Scalability**: O(log k) ranking complexity for top-k results

## Implementation Status

✅ **Completed Tasks:**
- Task 1: Project structure and core interfaces
- Task 2.1: Trie data structure implementation
- Task 2.2: Property test for Trie insertion and retrieval
- Task 2.3: Property test for prefix search correctness

🔄 **Ready for Implementation:**
- Task 3: Frequency tracking and word metadata
- Task 4: Suggestion ranking system
- Task 5: Dictionary management and persistence
- Task 6: Core AutoComplete engine integration
- Task 8: SFML GUI interface
- Task 9: GUI-Engine integration
- Task 10-12: Application lifecycle and final polish