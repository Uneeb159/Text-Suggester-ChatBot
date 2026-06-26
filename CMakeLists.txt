cmake_minimum_required(VERSION 3.16)
project(AutoCompleteEngine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Try to find SFML
find_package(SFML COMPONENTS graphics window system QUIET)
if(NOT SFML_FOUND)
    # Try to find SFML in common locations for Visual Studio
    set(SFML_ROOT "C:/SFML" CACHE PATH "SFML root directory")
    find_package(SFML COMPONENTS graphics window system QUIET PATHS ${SFML_ROOT})
    
    if(NOT SFML_FOUND)
        message(WARNING "SFML not found. Building console version only.")
        message(STATUS "To build GUI version, install SFML and set SFML_ROOT to SFML installation directory")
        set(BUILD_GUI OFF)
    else()
        set(BUILD_GUI ON)
        message(STATUS "SFML found. Building with GUI support.")
    endif()
else()
    set(BUILD_GUI ON)
    message(STATUS "SFML found. Building with GUI support.")
endif()

# Find Google Test
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    # Try to find in vcpkg or common locations
    find_package(GTest QUIET PATHS 
        "C:/vcpkg/installed/x64-windows"
        "C:/vcpkg/installed/x86-windows"
    )
    
    if(NOT GTest_FOUND)
        message(WARNING "Google Test not found. Tests will not be built.")
        message(STATUS "To build tests, install Google Test via vcpkg: vcpkg install gtest")
        set(BUILD_TESTS OFF)
    else()
        set(BUILD_TESTS ON)
        message(STATUS "Google Test found. Building tests.")
    endif()
else()
    set(BUILD_TESTS ON)
    message(STATUS "Google Test found. Building tests.")
endif()

# Find RapidCheck for property-based testing
find_path(RAPIDCHECK_INCLUDE_DIR rapidcheck.h PATHS
    "C:/vcpkg/installed/x64-windows/include"
    "C:/vcpkg/installed/x86-windows/include"
)
find_library(RAPIDCHECK_LIBRARY rapidcheck PATHS
    "C:/vcpkg/installed/x64-windows/lib"
    "C:/vcpkg/installed/x86-windows/lib"
)

if(RAPIDCHECK_INCLUDE_DIR AND RAPIDCHECK_LIBRARY)
    set(BUILD_PROPERTY_TESTS ON)
    message(STATUS "RapidCheck found. Building property-based tests.")
else()
    set(BUILD_PROPERTY_TESTS OFF)
    message(WARNING "RapidCheck not found. Property-based tests will not be built.")
    message(STATUS "To build property tests, install RapidCheck via vcpkg: vcpkg install rapidcheck")
endif()

# Core library
add_library(autocomplete_core
    AutoCompleteEngine.cpp
)

target_include_directories(autocomplete_core PUBLIC .)

# Console application (always built)
add_executable(autocomplete_console
    main_console.cpp
)

target_link_libraries(autocomplete_console autocomplete_core)

# GUI application (if SFML is available)
if(BUILD_GUI)
    add_executable(autocomplete_gui
        main_gui.cpp
        src/gui/GUIInterface.cpp
    )
    
    target_include_directories(autocomplete_gui PRIVATE src)
    target_link_libraries(autocomplete_gui 
        autocomplete_core 
        SFML::Graphics 
        SFML::Window 
        SFML::System
    )
endif()

# Tests (if Google Test is available)
if(BUILD_TESTS)
    enable_testing()
    
    add_executable(unit_tests
        tests/unit_tests.cpp
    )
    
    target_link_libraries(unit_tests 
        autocomplete_core 
        GTest::gtest 
        GTest::gtest_main
    )
    
    add_test(NAME UnitTests COMMAND unit_tests)
endif()

# Property-based tests (if RapidCheck is available)
if(BUILD_PROPERTY_TESTS)
    add_executable(property_tests
        tests/property_tests.cpp
    )
    
    target_include_directories(property_tests PRIVATE ${RAPIDCHECK_INCLUDE_DIR})
    target_link_libraries(property_tests 
        autocomplete_core 
        ${RAPIDCHECK_LIBRARY}
    )
    
    if(BUILD_TESTS)
        target_link_libraries(property_tests GTest::gtest GTest::gtest_main)
        add_test(NAME PropertyTests COMMAND property_tests)
    endif()
endif()

# Compiler-specific options
if(MSVC)
    target_compile_options(autocomplete_core PRIVATE /W4)
else()
    target_compile_options(autocomplete_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Create directories for tests
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Print build configuration
message(STATUS "Build configuration:")
message(STATUS "  Console app: ON")
message(STATUS "  GUI app: ${BUILD_GUI}")
message(STATUS "  Unit tests: ${BUILD_TESTS}")
message(STATUS "  Property tests: ${BUILD_PROPERTY_TESTS}")
message(STATUS "  Memory efficiency test: ON")