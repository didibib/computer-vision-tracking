#pragma once

namespace util
{
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

	const static std::string IMAGES_DIR_STR = TOSTRING(IMAGES_DIR);
	const static std::string SETTINGS_DIR_STR = TOSTRING(SETTINGS_DIR);

	const cv::Scalar red(0, 0, 255);
	const cv::Scalar green(0, 255, 0);
	const cv::Scalar blue(255, 0, 0);
	const cv::Scalar white(255, 255, 255);
}