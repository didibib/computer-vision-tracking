function(create_source_groups source_list)
    message(STATUS "Creating source groups")
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${source_list})
endfunction(create_source_groups)