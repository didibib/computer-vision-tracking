set(PCH 
    "include/cvpch.h"
    "src/cvpch.cpp"
)

set(CAMERA
    "include/camera.h"
    "src/camera.cpp"
)

set(CHECKERBOARD
    "include/checkerboard.h"
    "src/checkerboard.cpp"
)

set(BALL
    "include/ball.h"
    "src/ball.cpp"
)

set(ROOT_FILES
    "include/util.h"
    "main.cpp"
)

set(SOURCE_LIST
    ${PCH}
    ${CAMERA}
    ${CAMERA_CALIBRATION}
    ${CAMERA_PROJECTION}
    ${CHECKERBOARD}
    ${BALL}
    ${ROOT_FILES}
)

set(TARGET camera-geometric-calibration)

add_executable(${TARGET} ${SOURCE_LIST})

# Group sources according to the folder structure
source_group(\\ FILES ${ROOT_FILES})
source_group(pch FILES ${PCH})
source_group(camera FILES ${CAMERA})
source_group(checkerboard FILES ${CHECKERBOARD})
source_group(ball FILES ${BALL})

# Set output directories (function found in: cmake/)    
set_target_output_directories(${TARGET})

# Set precompiled headers
set_target_precompiled_header_msvc(${TARGET} "cvpch.h" "src/cvpch.cpp")

# Add macro specifying assets directory
# ${ASSETS_DRI} is set in the root 
target_compile_definitions(${TARGET} PUBLIC IMAGES_DIR=${IMAGES_DIR})

target_include_directories(${TARGET} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${TARGET} 
    ${OpenCV_LIBS}
)    