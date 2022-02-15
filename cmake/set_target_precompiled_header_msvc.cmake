# Set PCH for VS project
function(set_target_precompiled_header_msvc target header_file source_path)
    if(MSVC)
        message(STATUS "Setting precompiled headers for: '${target}'")
        message(STATUS "header file: ${header_file}, source path: ${source_path}")
        set_target_properties(${target} 
            PROPERTIES 
                COMPILE_OPTIONS "/Yu${header_file}"
        )
        set_source_files_properties(${source_path}
            PROPERTIES 
                COMPILE_OPTIONS "/Yc${header_file}"
        )
    endif(MSVC)
endfunction(set_target_precompiled_header_msvc)