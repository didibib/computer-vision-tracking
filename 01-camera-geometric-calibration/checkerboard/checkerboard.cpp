#include "pch/cvpch.h"
#include "checkerboard.h"

// We used the camera calibration from the following tutorial:
// https://learnopencv.com/camera-calibration-using-opencv/
int Checkerboard::FindPoints(cv::Mat& frame, std::vector<cv::Point3f>& objPoints, std::vector<cv::Point2f>& imgPoints) const
{
	cv::Mat gray;
	cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

	// Finding checker board corners
	// If desired number of corners are found in the image then success = true  
	bool success = cv::findChessboardCorners(gray, cv::Size(mWidth, mHeight), imgPoints, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);

	/*
		* If desired number of corner are detected,
		* we refine the pixel coordinates and display
		* them on the images of checker board
	*/
	if (success)
	{
		cv::TermCriteria criteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.001);

		// refining pixel coordinates for given 2d points.
		cv::cornerSubPix(gray, imgPoints, cv::Size(7, 7), cv::Size(-1, -1), criteria);

		// Displaying the detected corner points on the checker board
		cv::drawChessboardCorners(frame, cv::Size(mWidth, mHeight), imgPoints, success);

		// Defining the world coordinates for 3D points
		// We multiply our grid with the real size of the checkerboard
		for (int i{ 0 }; i < mHeight; i++)
		{
			for (int j{ 0 }; j < mWidth; j++)
				objPoints.push_back(cv::Point3f(j * mCm, i * mCm, 0));
		}
	}

	return success;
}
