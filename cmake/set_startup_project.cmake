function(set_startup_project target)
    if(MSVC)
        message(STATUS "Setting '${target}' as startup project")
        set_property(
            DIRECTORY 
                ${CMAKE_CURRENT_SOURCE_DIR} 
            PROPERTY 
                VS_STARTUP_PROJECT ${target}
        )
    endif(MSVC)
endfunction(set_startup_project)