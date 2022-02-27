set(TARGET 02-voxel-reconstruction)

set(ROOT_FILES
	"main.cpp"
)

set(UTIL 	
	"${UTIL_DIR}/logger.h"
	"${UTIL_DIR}/logger.cpp"
	"${UTIL_DIR}/shader.h"
	"${UTIL_DIR}/shader.cpp"
	"${UTIL_DIR}/vertex_buffer.h"
	"${UTIL_DIR}/vertex_buffer.cpp"
	"${UTIL_DIR}/util.h"
)

set(PCH 
	"include/cvpch.h"
	"src/cvpch.cpp"
)

set(ARCBALL
	"include/arcball.h"
	"src/arcball.cpp"
)

set(CAMERA
	"include/camera.h"
	"src/camera.cpp"
)

set(WINDOW
	"include/window.h"
	"src/window.cpp"
)

set(RECONSTRUCTOR 
	"include/reconstructor.h"
	"src/reconstructor.cpp"
)

set(SCENE_RENDERER 
	"include/scene_renderer.h"
	"src/scene_renderer.cpp"
)

set(SCENE_CAMERA 
	"include/scene_camera.h"
	"src/scene_camera.cpp"
)

set(VOXEL_RECONSTRUCTION
	"include/voxel_reconstruction.h"
	"src/voxel_reconstruction.cpp"
)

set(VOXEL
	"include/voxel.h"
	"src/voxel.cpp"
)

set(GLAD 
	"include/glad/glad.h"
	"src/glad.c"
)

add_executable (${TARGET}
	${ROOT_FILES}
	${UTIL}
	${CAMERA}
	${WINDOW}
	${RECONSTRUCTOR}
	${VOXEL_RECONSTRUCTION}
	${SCENE_RENDERER}
	${GLAD}
	${PCH}
	${VOXEL}
	${SCENE_CAMERA}
)

source_group(\\ FILES ${ROOT_FILES})
source_group(pch FILES ${PCH})
source_group(util FILES ${UTIL})
source_group(glad FILES ${GLAD})
source_group(camera FILES ${CAMERA})
source_group(window FILES ${WINDOW})
source_group(voxel FILES ${RECONSTRUCTOR} ${VOXEL_RECONSTRUCTION} ${VOXEL})
source_group(scene FILES ${SCENE_RENDERER} ${SCENE_CAMERA})

# Set output directories (function found in: cmake/)
set_target_output_directories(${TARGET})

# Set precompiled headers
set_target_precompiled_header_msvc(${TARGET} "cvpch.h" "src/cvpch.cpp")

set(IGNORE_PCH 
	"src/glad.c"
)
ignore_precompiled_header_msvc(IGNORE_PCH)

# Add macro specifying asset directory
set(DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data/")
target_compile_definitions(${TARGET} PUBLIC DATA_DIR_M=${DATA_DIR})

target_include_directories(${TARGET} PUBLIC
    "include"
	${UTIL_DIR}
	${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(${TARGET} PUBLIC
	${OpenCV_LIBS}
	glfw
    spdlog::spdlog
	glm
)