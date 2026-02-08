# ============================================================================
# AddDllDependencies.cmake
# ============================================================================
# Simple, explicit DLL dependency management for C++/CLI projects.
# Generates MSBuild .targets files with configuration-specific ItemGroups.
# ============================================================================

# ----------------------------------------------------------------------------
# _write_itemgroup: Write an MSBuild ItemGroup to the targets file
# ----------------------------------------------------------------------------
# Internal helper function to generate XML ItemGroup elements.
#
# Arguments:
#   file      - The .targets file path to write to
#   dlls      - List of DLL paths to include
#   condition - MSBuild condition (e.g., "'$(Configuration)'=='Debug'")
#               Empty string means unconditional
# ----------------------------------------------------------------------------
function(_write_itemgroup file dlls condition)
    if(NOT dlls)
        return()
    endif()

    # Remove any duplicate DLL paths
    list(REMOVE_DUPLICATES dlls)

    # Start ItemGroup with optional condition
    if(condition)
        file(APPEND ${file} "  <ItemGroup Condition=\"${condition}\">\n")
    else()
        file(APPEND ${file} "  <ItemGroup>\n")
    endif()

    # Add each DLL as a None item with CopyToOutputDirectory
    foreach(dll ${dlls})
        file(APPEND ${file} "    <None Include=\"${dll}\">\n")
        file(APPEND ${file} "      <CopyToOutputDirectory>PreserveNewest</CopyToOutputDirectory>\n")
        file(APPEND ${file} "    </None>\n")
    endforeach()

    file(APPEND ${file} "  </ItemGroup>\n")
endfunction()

# ============================================================================
# PUBLIC API
# ============================================================================

# ----------------------------------------------------------------------------
# add_manual_dll_dependencies: Explicitly specify DLLs to copy
# ----------------------------------------------------------------------------
# Simple function for manually specifying DLLs with Debug/Release paths.
# Allows full control over which DLLs are copied for each configuration.
#
# The DLLs will be copied to the target's output directory and will also
# flow through to any C# projects that reference this target.
#
# Usage examples:
#
#   Example 1: GDAL with separate debug/release DLL names
#   add_manual_dll_dependencies(lwsfade25d
#       DEBUG_DLL_NAMES gdald projd geotiffd tiffd zlibd
#       RELEASE_DLL_NAMES gdal proj geotiff tiff zlib
#       DEBUG_DIR "${GDAL_ROOT}/debug/bin"
#       RELEASE_DIR "${GDAL_ROOT}/bin"
#   )
#
#   Example 2: Fade25D with config-specific and shared DLLs
#   add_manual_dll_dependencies(lwsfade25d
#       DEBUG_DLL_NAMES fade25D_x64_v143_Debug
#       RELEASE_DLL_NAMES fade25D_x64_v143_Release
#       SHARED_DLL_NAMES libgmp-10
#       DEBUG_DIR "${FADE25D_ROOT}/bin"
#       RELEASE_DIR "${FADE25D_ROOT}/bin"
#   )
#
#   Example 3: Multiple calls to same target (accumulates DLLs)
#   add_manual_dll_dependencies(lwsfade25d
#       DEBUG_DLL_NAMES fade25D_x64_v143_Debug
#       RELEASE_DLL_NAMES fade25D_x64_v143_Release
#       DEBUG_DIR "${FADE25D_ROOT}/bin"
#       RELEASE_DIR "${FADE25D_ROOT}/bin"
#   )
#   add_manual_dll_dependencies(lwsfade25d
#       DEBUG_DLL_NAMES gdald
#       RELEASE_DLL_NAMES gdal
#       DEBUG_DIR "${GDAL_ROOT}/debug/bin"
#       RELEASE_DIR "${GDAL_ROOT}/bin"
#   )
#
# Arguments:
#   target            - The C++/CLI target that needs the DLLs
#   DEBUG_DLL_NAMES   - List of DLL names (without .dll) for Debug config
#   RELEASE_DLL_NAMES - List of DLL names (without .dll) for Release config
#   SHARED_DLL_NAMES  - List of DLL names (without .dll) used in both configs
#   DEBUG_DIR         - Directory containing Debug DLLs
#   RELEASE_DIR       - Directory containing Release DLLs
#
# Notes:
#   - DLL names should NOT include the .dll extension
#   - Multiple calls to this function for the same target will accumulate DLLs
#   - The function generates a .targets file that gets imported into the .vcxproj
# ----------------------------------------------------------------------------
function(add_manual_dll_dependencies target)
    # Parse arguments
    cmake_parse_arguments(ARG
        ""
        "DEBUG_DIR;RELEASE_DIR"
        "DEBUG_DLL_NAMES;RELEASE_DLL_NAMES;SHARED_DLL_NAMES"
        ${ARGN}
    )

    # Validate that at least some DLL names were provided
    if(NOT ARG_DEBUG_DLL_NAMES AND NOT ARG_RELEASE_DLL_NAMES AND NOT ARG_SHARED_DLL_NAMES)
        message(FATAL_ERROR "add_manual_dll_dependencies: No DLL names specified for target '${target}'")
    endif()

    # Build full DLL path lists
    set(debug_dlls "")
    set(release_dlls "")

    # Add debug-specific DLLs
    if(ARG_DEBUG_DIR AND ARG_DEBUG_DLL_NAMES)
        foreach(dll_name ${ARG_DEBUG_DLL_NAMES})
            list(APPEND debug_dlls "${ARG_DEBUG_DIR}/${dll_name}.dll")
        endforeach()
    endif()

    # Add release-specific DLLs
    if(ARG_RELEASE_DIR AND ARG_RELEASE_DLL_NAMES)
        foreach(dll_name ${ARG_RELEASE_DLL_NAMES})
            list(APPEND release_dlls "${ARG_RELEASE_DIR}/${dll_name}.dll")
        endforeach()
    endif()

    # Add shared DLLs (same DLL for both configs)
    if(ARG_SHARED_DLL_NAMES)
        foreach(dll_name ${ARG_SHARED_DLL_NAMES})
            if(ARG_DEBUG_DIR)
                list(APPEND debug_dlls "${ARG_DEBUG_DIR}/${dll_name}.dll")
            endif()
            if(ARG_RELEASE_DIR)
                list(APPEND release_dlls "${ARG_RELEASE_DIR}/${dll_name}.dll")
            endif()
        endforeach()
    endif()

    # Generate .targets file path
    set(targets_file "${CMAKE_CURRENT_BINARY_DIR}/${target}.DllDependencies.targets")

    # Initialize file if this is the first call for this target
    if(NOT EXISTS ${targets_file})
        file(WRITE ${targets_file} "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
        file(APPEND ${targets_file} "<Project xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n")
    else()
        # File exists - remove the closing </Project> tag so we can append more ItemGroups
        file(READ ${targets_file} existing_content)
        string(REPLACE "</Project>" "" existing_content "${existing_content}")
        file(WRITE ${targets_file} "${existing_content}")
    endif()

    # Write configuration-specific ItemGroups
    _write_itemgroup(${targets_file} "${debug_dlls}" "'$(Configuration)'=='Debug'")
    _write_itemgroup(${targets_file} "${release_dlls}" "'$(Configuration)'=='Release'")

    # Write XML footer (always re-add the closing tag)
    file(APPEND ${targets_file} "</Project>\n")

    # Import the .targets file into the Visual Studio project
    # This makes Visual Studio aware of the DLLs and their copy rules
    set_target_properties(${target} PROPERTIES
        VS_GLOBAL_DllDependenciesImport "${targets_file}"
    )
    set_property(TARGET ${target} APPEND PROPERTY
        VS_PROJECT_IMPORT "${targets_file}"
    )
endfunction()
