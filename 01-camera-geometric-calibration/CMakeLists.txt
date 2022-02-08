set(SOURCE_LIST
    "main.cpp"
)

add_executable(camera-geometric-calibration ${SOURCE_LIST})

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCE_LIST})

set_target_output_directories(camera-geometric-calibration)

target_link_libraries(camera-geometric-calibration ${OpenCV_LIBS})    