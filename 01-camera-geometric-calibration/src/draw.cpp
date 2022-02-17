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
		// Create rotation matrix from rotation vector
		cv::Mat R;
		cv::Rodrigues(rotation, R);

		cv::Mat T = cv::Mat(translation);
		cv::Mat M;
		cv::Mat filler = (cv::Mat_<float>(1, 4) << 0, 0, 0, 1);

		hconcat(R, T, M);
		vconcat(M, filler, M);

		std::vector<cv::Point3f> transPoints;
		for (int i = 0; i < points.size(); i++)
		{
			cv::Mat point = M * (cv::Mat_<float>(4, 1) << points[i].x, points[i].y, points[i].z, 1);
			transPoints.push_back(cv::Point3f(point.at<float>(0, 0), point.at<float>(1, 0), point.at<float>(2, 0)));
		}
		return transPoints;
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

	void drawCube(cv::Mat& frame, Camera& cam, float t)
	{
		auto T = std::vector<float>{ sin(t) * 10, cos(t) * 10, 0.f };
		auto R = std::vector<float>{ t, t, 2 * t };
		std::vector<cv::Point2f> imgPoints = cam.Project(transform(util::cube, T, R));

		drawImage(frame, { imgPoints[4], imgPoints[5], imgPoints[0], imgPoints[1] }, util::IMAGES_DIR_STR + "/square-face.jpg");

		// Draw the cube
		// Bottom frame
		cv::line(frame, imgPoints[0], imgPoints[1], util::white, 4);
		cv::line(frame, imgPoints[1], imgPoints[2], util::white, 4);
		cv::line(frame, imgPoints[2], imgPoints[3], util::white, 4);
		cv::line(frame, imgPoints[3], imgPoints[0], util::white, 4);
		// Top frame								
		cv::line(frame, imgPoints[4], imgPoints[5], util::white, 4);
		cv::line(frame, imgPoints[5], imgPoints[6], util::white, 4);
		cv::line(frame, imgPoints[6], imgPoints[7], util::white, 4);
		cv::line(frame, imgPoints[7], imgPoints[4], util::white, 4);
		// Connecting pillars						
		cv::line(frame, imgPoints[0], imgPoints[4], util::white, 4);
		cv::line(frame, imgPoints[1], imgPoints[5], util::white, 4);
		cv::line(frame, imgPoints[2], imgPoints[6], util::white, 4);
		cv::line(frame, imgPoints[3], imgPoints[7], util::white, 4);
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