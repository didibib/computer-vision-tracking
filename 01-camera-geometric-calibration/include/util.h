#pragma once

namespace util
{
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

	const static std::string IMAGES_DIR_STR = TOSTRING(IMAGES_DIR);
	const static std::string SETTINGS_DIR_STR = TOSTRING(SETTINGS_DIR);
	const static std::string CONFIG = SETTINGS_DIR_STR + "/config.xml";

	const cv::Scalar red(0, 0, 255);
	const cv::Scalar green(0, 255, 0);
	const cv::Scalar blue(255, 0, 0);
	const cv::Scalar white(255, 255, 255);
	const cv::Scalar gray(127, 127, 127);

	const std::vector<cv::Point3f> axis{
				{ 0.f,  0.f,  0.f   },
				{ 10.f, 0.f,  0.f   },
				{ 0.f,  10.f, 0.f   },
				{ 0.f,  0.f,  -10.f }
	};

	const float cbs = 10.f;
	const std::vector<cv::Point3f> cube{
					{ 0.f, 0.f,  0.f },
					{ cbs, 0.f,  0.f },
					{ cbs, cbs,  0.f },
					{ 0.f, cbs,  0.f },
					{ 0.f, 0.f, -cbs },
					{ cbs, 0.f, -cbs },
					{ cbs, cbs, -cbs },
					{ 0.f, cbs, -cbs }
	};

	std::vector<cv::Point3f> transform(std::vector<cv::Point3f> const& points, std::vector<float> T, std::vector<float> rotation);
	void drawAxis(cv::Mat& frame, Camera& cam);
	void drawCube(cv::Mat& frame, Camera& cam, float t);
	void drawImage(cv::Mat& frame, std::vector<cv::Point2f> dstPoints, std::string imgPath);
}