#pragma once

// STD 
#include <stdio.h>
#include <stddef.h>
#include <iostream>
#include <sstream>
#include <string>
#include <limits.h>
#include <algorithm>
#include <random>
#include <fstream>
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

// Own headers
#include "logger.h"