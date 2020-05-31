cmake_minimum_required(VERSION 3.10)
project(Chip8Emu)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if (CMAKE_COMPILER_IS_GNUCXX)
    LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules")
    include(CodeCoverage)
    APPEND_COVERAGE_COMPILER_FLAGS()
    SETUP_TARGET_FOR_COVERAGE_LCOV(NAME ${CMAKE_PROJECT_NAME}_coverage
            EXECUTABLE ${CMAKE_PROJECT_NAME}_tst
            DEPENDENCIES ${CMAKE_PROJECT_NAME}_tst)
    set(COVERAGE_LCOV_EXCLUDES '${PROJECT_SOURCE_DIR}/test/*')
endif ()

enable_testing()

include_directories(src)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib/googletest)
