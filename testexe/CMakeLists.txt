cmake_minimum_required(VERSION 3.15)
project(testexe)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Fade25D using the config file
set(CMAKE_PREFIX_PATH "${FADE2D_PATH}")
find_package(Fade25d REQUIRED)

# Add executable
add_executable(testexe main.cpp)

# Link against the Fade25D imported target
target_link_libraries(testexe PRIVATE fade25d::fade25d)

add_custom_command(TARGET testexe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:testexe>
        $<TARGET_FILE_DIR:testexe>
    COMMAND_EXPAND_LISTS
)