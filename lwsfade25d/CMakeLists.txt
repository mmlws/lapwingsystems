cmake_minimum_required(VERSION 3.20)

include(${CMAKE_SOURCE_DIR}/../cmake/AddRuntimeDlls.cmake)

project(lwsfade25d LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "${FADE2D_PATH};${GDAL_PATH}")

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

set(DOTNET_TARGET_FRAMEWORK "net8.0")
set(LWS_GEOMETRY_DLL "${CMAKE_SOURCE_DIR}/../LWS.Geometry/bin/$(Configuration)/${DOTNET_TARGET_FRAMEWORK}/LWS.Geometry.dll")

set_target_properties(lwsfade25d PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    COMMON_LANGUAGE_RUNTIME "netcore"
    DOTNET_TARGET_FRAMEWORK ${DOTNET_TARGET_FRAMEWORK}
    VS_DOTNET_REFERENCE_LWS.Geometry ${LWS_GEOMETRY_DLL}
)

# LWS.Geometry managed assembly reference
target_compile_options(lwsfade25d PRIVATE "/FU${LWS_GEOMETRY_DLL}")

find_package(Fade25d REQUIRED)
find_package(GDAL CONFIG REQUIRED)

target_link_libraries(lwsfade25d
    PRIVATE fade25d::fade25d
    PRIVATE GDAL::GDAL
)

# Fade25D Runtime DLLs
get_target_property(FADE25D_INCLUDE fade25d::fade25d INTERFACE_INCLUDE_DIRECTORIES)
get_filename_component(FADE25D_ROOT "${FADE25D_INCLUDE}" DIRECTORY)
add_runtime_dlls(lwsfade25d
  DEBUG_DLLS fade25D_x64_v143_Debug libgmp-10
  RELEASE_DLLS fade25D_x64_v143_Release libgmp-10
  DEBUG_DIR "${FADE25D_ROOT}/bin"
  RELEASE_DIR "${FADE25D_ROOT}/bin"
)

# GDAL Runtime DLLs
get_target_property(GDAL_INCLUDE GDAL::GDAL INTERFACE_INCLUDE_DIRECTORIES)
get_filename_component(GDAL_ROOT "${GDAL_INCLUDE}" DIRECTORY)
add_runtime_dlls(lwsfade25d
  DEBUG_DLLS gdald proj_9_d geotiff_d tiffd zlibd1 jpeg62 json-c libcurl-d liblzma sqlite3 turbojpeg
  RELEASE_DLLS gdal proj_9 geotiff tiff zlib1 jpeg62 json-c libcurl liblzma sqlite3 turbojpeg
  DEBUG_DIR "${GDAL_ROOT}/debug/bin"
  RELEASE_DIR "${GDAL_ROOT}/bin"
)

  