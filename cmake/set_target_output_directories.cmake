function(set_target_output_directories target)
    message(STATUS "Setting output directories for '${target}'")
    get_target_property(target_type ${target} TYPE)
    if (target_type STREQUAL "EXECUTABLE")
        set_target_properties(${target}
            PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${${target}}"        
                VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${${target}}"
        )
    elseif(target_type STREQUAL "STATIC_LIBRARY" OR target_type STREQUAL "SHARED_LIBRARY")
        set_target_properties(${target}
            PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${${target}}"
                ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${${target}}"
                LIBRARY_OUTPUT_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${${target}}"
        )
    endif()
endfunction(set_target_output_directories)

