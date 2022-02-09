set(PCH_H "pch/cvpch.h")
set(PCH_CPP "pch/cvpch.cpp")

set(SOURCE_LIST
    ${PCH_H}
    ${PCH_CPP}
    "camera_calibration/camera_calibration.h"
    "camera_calibration/camera_calibration.cpp"
    "util.h"
    "main.cpp"
)

add_executable(camera-geometric-calibration ${SOURCE_LIST})

# Group sources according to the folder structure
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_LIST})

# Set output directories (function found in: cmake/)    
set_target_output_directories(camera-geometric-calibration)

# Set precompiled headers (function found in: cmake/)
set_target_precompiled_header(camera-geometric-calibration PCH_H PCH_CPP)

# Add macro specifying assets directory
# ${ASSETS_DRI} is set in the root 
target_compile_definitions(camera-geometric-calibration PUBLIC IMAGES_DIR=${IMAGES_DIR})

target_include_directories(camera-geometric-calibration PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(camera-geometric-calibration 
    ${OpenCV_LIBS}
)    