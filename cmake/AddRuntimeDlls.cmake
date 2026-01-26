# ============================================================================
# AddRuntimeDlls.cmake
# ============================================================================
# Simple function to add runtime DLLs to C++/CLI projects that flow through
# to C# projects via project references.
# ============================================================================

# ----------------------------------------------------------------------------
# add_runtime_dlls: Add DLLs to copy for Debug/Release configurations
# ----------------------------------------------------------------------------
# Call once per dependency. Multiple calls accumulate DLLs.
#
# Usage:
#   add_runtime_dlls(lwsfade25d
#       DEBUG_DLLS fade25D_x64_v143_Debug libgmp-10
#       RELEASE_DLLS fade25D_x64_v143_Release libgmp-10
#       DEBUG_DIR "${FADE25D_ROOT}/bin"
#       RELEASE_DIR "${FADE25D_ROOT}/bin"
#   )
#
#   add_runtime_dlls(lwsfade25d
#       DEBUG_DLLS gdald projd geotiffd
#       RELEASE_DLLS gdal proj geotiff
#       DEBUG_DIR "${GDAL_ROOT}/debug/bin"
#       RELEASE_DIR "${GDAL_ROOT}/bin"
#   )
# ----------------------------------------------------------------------------
function(add_runtime_dlls target)
    cmake_parse_arguments(ARG "" "DEBUG_DIR;RELEASE_DIR" "DEBUG_DLLS;RELEASE_DLLS" ${ARGN})

    set(targets_file "${CMAKE_CURRENT_BINARY_DIR}/${target}.RuntimeDlls.targets")

    # Check if this is the first call for this target in this CMake run
    get_property(is_initialized GLOBAL PROPERTY _RUNTIME_DLLS_INIT_${target})

    if(NOT is_initialized)
        # First call - create fresh file and register import
        file(WRITE ${targets_file} "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n<Project>\n")
        set_property(GLOBAL PROPERTY _RUNTIME_DLLS_INIT_${target} TRUE)
        set_property(TARGET ${target} APPEND PROPERTY VS_PROJECT_IMPORT "${targets_file}")
    else()
        # Subsequent call - remove closing tag to append more
        file(READ ${targets_file} content)
        string(REPLACE "</Project>" "" content "${content}")
        file(WRITE ${targets_file} "${content}")
    endif()

    # Add Debug DLLs
    if(ARG_DEBUG_DLLS AND ARG_DEBUG_DIR)
        file(APPEND ${targets_file} "  <ItemGroup Condition=\"'\$(Configuration)'=='Debug'\">\n")
        foreach(dll ${ARG_DEBUG_DLLS})
            file(APPEND ${targets_file} "    <None Include=\"${ARG_DEBUG_DIR}/${dll}.dll\">\n")
            file(APPEND ${targets_file} "      <Link>${dll}.dll</Link>\n")
            file(APPEND ${targets_file} "      <CopyToOutputDirectory>PreserveNewest</CopyToOutputDirectory>\n")
            file(APPEND ${targets_file} "    </None>\n")
        endforeach()
        file(APPEND ${targets_file} "  </ItemGroup>\n")
    endif()

    # Add Release DLLs
    if(ARG_RELEASE_DLLS AND ARG_RELEASE_DIR)
        file(APPEND ${targets_file} "  <ItemGroup Condition=\"'\$(Configuration)'=='Release'\">\n")
        foreach(dll ${ARG_RELEASE_DLLS})
            file(APPEND ${targets_file} "    <None Include=\"${ARG_RELEASE_DIR}/${dll}.dll\">\n")
            file(APPEND ${targets_file} "      <Link>${dll}.dll</Link>\n")
            file(APPEND ${targets_file} "      <CopyToOutputDirectory>PreserveNewest</CopyToOutputDirectory>\n")
            file(APPEND ${targets_file} "    </None>\n")
        endforeach()
        file(APPEND ${targets_file} "  </ItemGroup>\n")
    endif()

    # Close XML
    file(APPEND ${targets_file} "</Project>\n")
endfunction()
