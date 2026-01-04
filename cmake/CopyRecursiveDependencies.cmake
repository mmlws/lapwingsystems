function(_get_imported_dlls target debug_out release_out any_out)
    get_target_property(type ${target} TYPE)
    if(NOT type STREQUAL "SHARED_LIBRARY" AND NOT type STREQUAL "UNKNOWN_LIBRARY")
        return()
    endif()

    set(debug_dlls "")
    set(release_dlls "")
    set(any_dlls "")

    # Check for config-specific locations
    get_target_property(loc_debug ${target} IMPORTED_LOCATION_DEBUG)
    if(loc_debug)
        list(APPEND debug_dlls "${loc_debug}")
    endif()

    get_target_property(loc_release ${target} IMPORTED_LOCATION_RELEASE)
    if(loc_release)
        list(APPEND release_dlls "${loc_release}")
    endif()

    # Check for general location
    get_target_property(loc ${target} IMPORTED_LOCATION)
    if(loc)
        list(APPEND any_dlls "${loc}")
    endif()

    get_target_property(loc_noconfig ${target} IMPORTED_LOCATION_NOCONFIG)
    if(loc_noconfig)
        list(APPEND any_dlls "${loc_noconfig}")
    endif()

    set(${debug_out} ${debug_dlls} PARENT_SCOPE)
    set(${release_out} ${release_dlls} PARENT_SCOPE)
    set(${any_out} ${any_dlls} PARENT_SCOPE)
endfunction()

function(_collect_dependencies target visited_var debug_gathered release_gathered any_gathered)
    if(${target} IN_LIST ${visited_var})
        return()
    endif()
    list(APPEND ${visited_var} ${target})
    set(${visited_var} ${${visited_var}} PARENT_SCOPE)

    get_target_property(libs ${target} INTERFACE_LINK_LIBRARIES)
    
    if(libs)
        foreach(lib ${libs})
            if(TARGET ${lib})
                get_property(is_imported TARGET ${lib} PROPERTY IMPORTED)
                if(is_imported)
                    _get_imported_dlls(${lib} cur_debug cur_release cur_any)
                    list(APPEND ${debug_gathered} ${cur_debug})
                    list(APPEND ${release_gathered} ${cur_release})
                    list(APPEND ${any_gathered} ${cur_any})
                endif()
                _collect_dependencies(${lib} ${visited_var} ${debug_gathered} ${release_gathered} ${any_gathered})
            endif()
        endforeach()
    endif()
    
    set(${debug_gathered} ${${debug_gathered}} PARENT_SCOPE)
    set(${release_gathered} ${${release_gathered}} PARENT_SCOPE)
    set(${any_gathered} ${${any_gathered}} PARENT_SCOPE)
endfunction()

function(add_native_dll_dependencies target dependency)
    set(visited "")
    set(debug_files "")
    set(release_files "")
    set(any_files "")
    
    if(TARGET ${dependency})
         get_property(is_imported TARGET ${dependency} PROPERTY IMPORTED)
         if(is_imported)
            _get_imported_dlls(${dependency} cur_debug cur_release cur_any)
            set(debug_files ${cur_debug})
            set(release_files ${cur_release})
            set(any_files ${cur_any})
         endif()
         
         _collect_dependencies(${dependency} visited debug_files release_files any_files)
    endif()

    if(debug_files)
        list(REMOVE_DUPLICATES debug_files)
        target_sources(${target} PRIVATE $<$<CONFIG:Debug>:${debug_files}>)
        set_source_files_properties(${debug_files} PROPERTIES
            VS_TOOL_OVERRIDE "None"
            VS_COPY_TO_OUT_DIR "PreserveNewest"
        )
    endif()

    if(release_files)
        list(REMOVE_DUPLICATES release_files)
        target_sources(${target} PRIVATE $<$<CONFIG:Release>:${release_files}>)
        set_source_files_properties(${release_files} PROPERTIES
            VS_TOOL_OVERRIDE "None"
            VS_COPY_TO_OUT_DIR "PreserveNewest"
        )
    endif()

    if(any_files)
        list(REMOVE_DUPLICATES any_files)
        target_sources(${target} PRIVATE ${any_files})
        set_source_files_properties(${any_files} PROPERTIES
            VS_TOOL_OVERRIDE "None"
            VS_COPY_TO_OUT_DIR "PreserveNewest"
        )
    endif()
endfunction()