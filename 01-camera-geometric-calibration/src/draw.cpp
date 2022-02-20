#include "cvpch.h"
#include "camera.h"

namespace util
{	
	/*
		@param Points that need to be transformed
		@param 3x1 translation vector
		@param 3x1 rotation vector
	*/
	std::vector<cv::Point3f> transform(std::vector<cv::Point3f> const& points, std::vector<float> translation, std::vector<float> rotation)
	{
		cv::Mat M = createTransform(translation, rotation);
		return transform(points, M);
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

		return M;
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

	void drawAxis(cv::Mat& frame, Camera& cam)
	{
		auto imgPoints = cam.Project(util::axis);
		// Draw the x-axis 
		cv::line(frame, imgPoints[0], imgPoints[1], util::red, 2);
		// Draw the y-axis 
		cv::line(frame, imgPoints[0], imgPoints[2], util::green, 2);
		// Draw the z-axis 
		cv::line(frame, imgPoints[0], imgPoints[3], util::blue, 2);
	}

	// https://medium.com/acmvit/how-to-project-an-image-in-perspective-view-of-a-background-image-opencv-python-d101bdf966bc
	void drawImage(cv::Mat& frame, std::vector<cv::Point2f> dstPoints, std::string imgPath)
	{
		cv::Mat image = cv::imread(imgPath);
		std::vector<cv::Point2f> imgPoints{
			cv::Point2f(0.f, 0.f),
			cv::Point2f(image.cols, 0.f),
			cv::Point2f(0.f, image.rows),
			cv::Point2f(image.cols, image.rows) };

		// Find the perspective transformation between planes
		cv::Mat h = cv::findHomography(imgPoints, dstPoints, cv::RANSAC, 5.0);
		cv::Mat warped;

		// Use the homography matrix to warp our image
		cv::warpPerspective(image, warped, h, cv::Size(frame.cols, frame.rows));

		// Create a mask where everything is white, but the image is black
		cv::Mat invMask;
		cv::threshold(warped, invMask, 0, 255, cv::THRESH_BINARY_INV);

		// Combine the mask and the current frame
		cv::Mat frameMasked;
		cv::bitwise_and(frame, invMask, frameMasked);

		// Combine the frame with the warped image
		cv::bitwise_or(frameMasked, warped, frame);
	}

}