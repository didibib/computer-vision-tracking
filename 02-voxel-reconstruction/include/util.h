#pragma once
#ifndef UTIL_H
#define UTIL_H

#define PATH_SEP "/"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

namespace util
{
	const static std::string DATA_DIR_STR = TOSTRING(DATA_DIR_M);

	// Version and Main OpenCV window name
	const static std::string VERSION = "2.5";
	const static std::string VIDEO_WINDOW = "Video";
	const static std::string SCENE_WINDOW = "OpenGL 3D scene";

	// Some OpenCV colors
	const static cv::Scalar COLOR_BLUE = cv::Scalar(255, 0, 0);
	const static cv::Scalar COLOR_GREEN = cv::Scalar(0, 200, 0);
	const static cv::Scalar COLOR_RED = cv::Scalar(0, 0, 255);
	const static cv::Scalar COLOR_YELLOW = cv::Scalar(0, 255, 255);
	const static cv::Scalar COLOR_MAGENTA = cv::Scalar(255, 0, 255);
	const static cv::Scalar COLOR_CYAN = cv::Scalar(255, 255, 0);
	const static cv::Scalar COLOR_WHITE = cv::Scalar(255, 255, 255);
	const static cv::Scalar COLOR_BLACK = cv::Scalar(0, 0, 0);


	static const std::string CB_CONFIG_FILE = "checkerboard.xml";
	static const std::string INTRINSICS_FILE = "intrinsics.xml";
	static const std::string CALIBRATION_VIDEO = "intrinsics.avi";
	static const std::string CHECKERBOARD_VIDEO = "checkerboard.avi";
	static const std::string CHECKERBOARD_CORNERS = "boardcorners.xml";
	static const std::string VIDEO_FILE = "video.avi";
	static const std::string BACKGROUND_IMAGE_FILE = "background.png";
	static const std::string BACKGROUND_VIDEO_FILE = "background.avi";
	static const std::string CAM_CONFIG_FILE = "config.xml";

	static const int CALIB_MAX_NR_FRAMES = 40;

	/**
	 * Linux/Windows friendly way to check if a file exists
	 */
	static bool fexists(const std::string& filename)
	{
		std::ifstream ifile(filename.c_str());
		return ifile.is_open();
	}

	static std::string type2str(int type)
	{
		std::string r;

		uchar depth = type & CV_MAT_DEPTH_MASK;
		uchar chans = 1 + (type >> CV_CN_SHIFT);

		switch (depth)
		{
		case CV_8U:  r = "8U"; break;
		case CV_8S:  r = "8S"; break;
		case CV_16U: r = "16U"; break;
		case CV_16S: r = "16S"; break;
		case CV_32S: r = "32S"; break;
		case CV_32F: r = "32F"; break;
		case CV_64F: r = "64F"; break;
		default:     r = "User"; break;
		}

		r += "C";
		r += (chans + '0');

		return r;
	}

} /* namespace team45 */

#endif /* Util_H_ */
