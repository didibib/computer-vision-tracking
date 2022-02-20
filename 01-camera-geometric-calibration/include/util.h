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

	/*
		@param Points that need to be transformed
		@param 3x1 translation vector
		@param 3x1 rotation vector
	*/
	std::vector<cv::Point3f> transform(std::vector<cv::Point3f> const& points, std::vector<float> translation, std::vector<float> rotation)
	{
		cv::Mat M = createTransform(translation, rotation);
		transform(points, M);
	}

	cv::Mat createTransform(std::vector<float> translation, std::vector<float> rotation)
	{
		// Create rotation matrix from rotation vector
		cv::Mat R;
		cv::Rodrigues(rotation, R);

		cv::Mat T = cv::Mat(translation);
		cv::Mat M;
		cv::Mat filler = (cv::Mat_<float>(1, 4) << 0, 0, 0, 1);

		hconcat(R, T, M);
		vconcat(M, filler, M);
	}

	/*
		@param Points that need to be transformed
		@param 4x4 transformation matrix
	*/
	std::vector<cv::Point3f> transform(std::vector<cv::Point3f> const& points, cv::Mat M)
	{
		std::vector<cv::Point3f> transPoints;
		for (int i = 0; i < points.size(); i++)
		{
			cv::Mat point = M * (cv::Mat_<float>(4, 1) << points[i].x, points[i].y, points[i].z, 1);
			transPoints.push_back(cv::Point3f(point.at<float>(0, 0), point.at<float>(1, 0), point.at<float>(2, 0)));
		}
		return transPoints;
	}

	/*
		@param Points that need to be transformed
		@param 4x4 transformation matrix
	*/
	cv::Point3f transform(cv::Point3f const& point, cv::Mat M)
	{
		cv::Mat pointMat = M * (cv::Mat_<float>(4, 1) << point.x, point.y, point.z, 1);
		return cv::Point3f(pointMat.at<float>(0, 0), pointMat.at<float>(1, 0), pointMat.at<float>(2, 0));
	}
}