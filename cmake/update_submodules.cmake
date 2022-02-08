# Updates submodules
# Should be called in the root folder
function(update_submodules)
    message(STATUS "Checking for Git")
    find_package(Git QUIET)
    if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git" )
        # Update submodules
        option(GIT_SUBMODULE "Check submodules during build" ON)
        if(GIT_SUBMODULE)
            message(STATUS "Updating submodules")
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                            RESULT_VARIABLE SUBMODULE_RESULT)
            if(NOT SUBMODULE_RESULT EQUAL "0")
                message(FATAL_ERROR "Git submodule update --init --recursive failed with ${SUBMODULE_RESULT")
            endif()
        endif()
    endif()
endfunction(update_submodules)