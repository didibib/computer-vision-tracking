cmake_minimum_required(VERSION 3.18.0)

# Check if project is build in seperate folder
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there.")
endif()

# Define asset paths
set(UTIL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/util/")

# Include cmake modules
include(cmake/update_submodules.cmake)
include(cmake/set_startup_project.cmake)
include(cmake/ignore_precompiled_header_msvc.cmake)
include(cmake/set_target_output_directories.cmake)
include(cmake/set_target_precompiled_header_msvc.cmake)

# Find OpenCV on system
find_package(OpenCV REQUIRED)               

# Set the project name and version
project(computer-vision VERSION 1.0)

# Use folder structure 
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# Specify the c++ standard
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED true)
# Setup output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)

# Start building
message(STATUS "Adding dependency GLFW")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/glfw)

message(STATUS "Adding dependency 'spdlog'")
add_subdirectory(vendor/spdlog)

message(STATUS "Adding dependency 'glm'")
add_subdirectory(vendor/glm)

message(STATUS "Adding subdirectory '01-camera-geometric-calibration'")
add_subdirectory(01-camera-geometric-calibration)

message(STATUS "Adding subdirectory '02-03-voxel-reconstruction-and-labeling'")
add_subdirectory(02-03-voxel-reconstruction-and-labeling)

# (function found in: cmake/)
set_startup_project(02-03-voxel-reconstruction-and-labeling)