set(CAMERA_CALIBRATION 
    "include/camera_calibration.h"
    "src/camera_calibration.cpp"
)

set(CAMERA_PROJECTION
    "include/camera_projection.h"
    "src/camera_projection.cpp"
)

set(CHECKERBOARD
    "include/checkerboard.h"
    "src/checkerboard.cpp"
)

set(ROOT_FILES
    "include/util.h"
    "main.cpp"
)

set(SOURCE_LIST
    ${CAMERA_CALIBRATION}
    ${CAMERA_PROJECTION}
    ${CHECKERBOARD}
    ${ROOT_FILES}
)

set(TARGET camera-geometric-calibration)

add_executable(${TARGET} ${SOURCE_LIST})

# Group sources according to the folder structure
source_group(\\ FILES ${ROOT_FILES})
source_group(camera FILES ${CAMERA_CALIBRATION} ${CAMERA_PROJECTION})
source_group(checkerboard FILES ${CHECKERBOARD})

# Set output directories (function found in: cmake/)    
set_target_output_directories(${TARGET})

# Set precompiled headers
target_precompile_headers(${TARGET} PRIVATE
    <stdio.h>
    <iostream>
    <string>
    <opencv2/opencv.hpp>
    <opencv2/calib3d/calib3d.hpp>
    <opencv2/highgui/highgui.hpp>
    <opencv2/imgproc/imgproc.hpp>
    [["util.h"]]
)

# Add macro specifying assets directory
# ${ASSETS_DRI} is set in the root 
target_compile_definitions(${TARGET} PUBLIC IMAGES_DIR=${IMAGES_DIR})

target_include_directories(${TARGET} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${TARGET} 
    ${OpenCV_LIBS}
)    