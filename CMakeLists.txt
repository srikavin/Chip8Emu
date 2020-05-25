cmake_minimum_required(VERSION 3.10)
project(Chip8Emu)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if(CMAKE_COMPILER_IS_GNUCXX)
    LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules")
    include(CodeCoverage)
    APPEND_COVERAGE_COMPILER_FLAGS()
    SETUP_TARGET_FOR_COVERAGE_LCOV(NAME ${CMAKE_PROJECT_NAME}_coverage
            EXECUTABLE ${CMAKE_PROJECT_NAME}_tst
            DEPENDENCIES ${CMAKE_PROJECT_NAME}_tst)
    endif()

enable_testing()

include_directories(src)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib/googletest)


#add_executable(Chip8Emu src/main.cpp src/Cpu.cpp src/Cpu.h src/Memory.cpp src/Memory.h src/Graphics.cpp src/Graphics.h)