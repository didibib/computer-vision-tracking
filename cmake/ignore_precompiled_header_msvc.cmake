# Ignore PCH for a specified list of files
function(ignore_precompiled_header_msvc sources)
    if(MSVC)  
        message(STATUS "Ignore precompiled header on: ${sources}")
        set_source_files_properties(${${sources}} 
            PROPERTIES 
                COMPILE_OPTIONS "/Y-"
        )
    endif(MSVC)
endfunction(ignore_precompiled_header_msvc)