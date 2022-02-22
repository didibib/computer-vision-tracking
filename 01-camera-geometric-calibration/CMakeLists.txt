set(TARGET 01-camera-geometric-calibration)

set(ROOT_FILES
    "include/util.h"
    "src/draw.cpp"
    "main.cpp"
)

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

set(LINE_SEGMENT
    "include/line_segment.h"
)

set(SOURCE_LIST
    ${PCH}
    ${CAMERA}
    ${CAMERA_CALIBRATION}
    ${CAMERA_PROJECTION}
    ${CHECKERBOARD}
    ${BALL}
    ${LINE_SEGMENT}
    ${ROOT_FILES}
)

add_executable(${TARGET} ${SOURCE_LIST})

# Group sources according to the folder structure
source_group(\\ FILES ${ROOT_FILES})
source_group(pch FILES ${PCH})
source_group(camera FILES ${CAMERA})
source_group(checkerboard FILES ${CHECKERBOARD})
source_group(ball FILES ${BALL})
source_group(line_segment FILES ${LINE_SEGMENT})

# Set output directories (function found in: cmake/)
set_target_output_directories(${TARGET})

# Set precompiled headers
set_target_precompiled_header_msvc(${TARGET} "cvpch.h" "src/cvpch.cpp")

# Add macro specifying assets directory
# ${ASSETS_DRI} is set in the root
target_compile_definitions(${TARGET} PUBLIC IMAGES_DIR=${IMAGES_DIR})
target_compile_definitions(${TARGET} PUBLIC SETTINGS_DIR=${SETTINGS_DIR})

target_include_directories(${TARGET} PUBLIC
    "include"
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(${TARGET} PUBLIC
    ${OpenCV_LIBS}
)
