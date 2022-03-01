#pragma once

// STD 
#include <stdio.h>
#include <stddef.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits.h>
#include <algorithm>
#include <random>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <complex>
#include <valarray>
#include <vector>

// OpenCV 
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/core/persistence.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/core_c.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/core/operations.hpp>

// OpenGL
// Include glad before glfw!
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#ifdef _WIN32
#include "GLFW/glfw3native.h"
#endif
#ifdef _WIN32
#include <Windows.h>
#include <GL/glu.h>
#endif

// GLM headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/fast_square_root.hpp>

// Own headers
#include "logger.h"

namespace team45
{
	/*
	 * Voxel structure
	 * Represents a 3D pixel in the half space
	 */
	struct Voxel
	{
		glm::ivec3 position;
		glm::vec3 color;
		std::vector<float> distances;				// Distance from this voxel to each camera 
		std::vector<cv::Point> pixelProjections;	// Pixel that this voxel projects to on each camera ((-1,-1) if it doesn't project onto the camera)
		int visibleIndex;							// Coordinates and index in m_visible_voxels (-1 if none)
		int camera_flags;							// Bitwise Flag if voxel was on in camera[c] in the previous frame
	};

	/*
	 * Vertex structure
	 * Represents a 3D vertex
	 */
	struct Vertex
	{
		glm::vec3 Position;		// 3 * float = 12 bytes
		glm::vec4 Color;		// 4 * float = 16 bytes
		glm::vec2 UV;			// 2 * float = 8  bytes
		glm::vec3 Normal;		// 3 * float = 12 bytes
	};
}
