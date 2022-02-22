set(TARGET 02-voxel-reconstruction)

set(ARCBALL
	"include/arcball.h"
	"src/arcball.cpp"
)

set(CAMERA
	"include/camera.h"
	"src/camera.cpp"
)

set(GLUT
	"include/glut.h"
	"src/glut.cpp"
)

set(RECONSTRUCTOR 
	"include/reconstructor.h"
	"src/reconstructor.cpp"
)

set(SCENE_RENDERER 
	"include/scene_renderer.h"
	"src/scene_renderer.cpp"
)

set(ROOT_FILES
	"include/util.h"
	"src/util.cpp"
	"main.cpp"
)

add_executable (${TARGET}
	${ARCBALL}
	${CAMERA}
	${GLUT}
	${RECONSTRUCTOR}
	${SCENE_RENDERER}
	${ROOT_FILES}
)

source_group(\\ FILES ${ROOT_FILES})
source_group(arcball FILES ${ARCBALL})
source_group(camera FILES ${CAMERA})
source_group(glut FILES ${GLUT})
source_group(reconstructor FILES ${RECONSTRUCTOR})
source_group(scene_renderer FILES ${SCENE_RENDERER})

# Set output directories (function found in: cmake/)
set_target_output_directories(${TARGET})

# Add macro specifying asset directory
target_compile_definitions(${TARGET} PUBLIC VIDEOS_DIR=${VIDEOS_DIR})

target_include_directories(${TARGET} PUBLIC
    "include"
)