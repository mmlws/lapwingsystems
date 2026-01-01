# Get the directory where this config file is located
get_filename_component(FADE25D_ROOT "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)

if (NOT TARGET fade25d::gmp)
    add_library(fade25d::gmp SHARED IMPORTED)

    set_target_properties(fade25d::gmp PROPERTIES
        IMPORTED_LOCATION "${FADE25D_ROOT}/bin/libgmp-10.dll"
        IMPORTED_IMPLIB "${FADE25D_ROOT}/lib/libgmp-10.lib"
    )

endif()


if(NOT TARGET fade25d::fade25d)
    add_library(fade25d::fade25d SHARED IMPORTED)
    
    set_target_properties(fade25d::fade25d PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${FADE25D_ROOT}/include"
        IMPORTED_LOCATION_RELEASE "${FADE25D_ROOT}/bin/fade25D_x64_v143_Release.dll"
        IMPORTED_IMPLIB_RELEASE "${FADE25D_ROOT}/lib/fade25D_x64_v143_Release.lib"
        IMPORTED_LOCATION_DEBUG "${FADE25D_ROOT}/bin/fade25D_x64_v143_Debug.dll"
        IMPORTED_IMPLIB_DEBUG "${FADE25D_ROOT}/lib/fade25D_x64_v143_Debug.lib"
        INTERFACE_LINK_LIBRARIES fade25d::gmp
    )

endif()