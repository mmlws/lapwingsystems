cmake_minimum_required(VERSION 3.20)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/CopyRecursiveDependencies.cmake)

project(lwsfade25d LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "${FADE2D_PATH}")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h" "*.hpp")

# Create managed library
add_library(lwsfade25d SHARED
    ${SOURCES}
    ${HEADERS}
)

set_target_properties(lwsfade25d PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    COMMON_LANGUAGE_RUNTIME "netcore"
    DOTNET_TARGET_FRAMEWORK "net8.0"
)

find_package(Fade25d REQUIRED)

target_link_libraries(lwsfade25d
    PRIVATE fade25d::fade25d
)

add_native_dll_dependencies(lwsfade25d fade25d::fade25d)