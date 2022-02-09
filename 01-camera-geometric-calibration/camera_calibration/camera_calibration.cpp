#include "pch/cvpch.h"
#include "camera_calibration.h"


// We used the camera calibration from the following tutorial:
// https://learnopencv.com/camera-calibration-using-opencv/
void CameraCalibration::Calibrate(Checkerboard const& checkerboard)
{
	// Creating vector to store vectors of 3D points for each checkerboard image
	std::vector<std::vector<cv::Point3f> > objPoints;

	// Creating vector to store vectors of 2D points for each checkerboard image
	std::vector<std::vector<cv::Point2f> > imgPoints;

	// Defining the world coordinates for 3D points
	std::vector<cv::Point3f> objp;
	for (int i{ 0 }; i < checkerboard.Height; i++)
	{
		for (int j{ 0 }; j < checkerboard.Width; j++)
			objp.push_back(cv::Point3f(j, i, 0));
	}

	// Extracting path of individual image stored in a given directory
	std::vector<cv::String> images;
	cv::glob(IMAGES_DIR_STR, images);

	cv::Mat frame, gray;
	// vector to store the pixel coordinates of detected checker board corners 
	std::vector<cv::Point2f> cornerPoints;
	bool success;

	// Looping over all the images in the directory
	for (int i{ 0 }; i < images.size(); i++)
	{
		frame = cv::imread(images[i]);
		cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

		// Finding checker board corners
		// If desired number of corners are found in the image then success = true  
		success = cv::findChessboardCorners(gray, cv::Size(checkerboard.Width, checkerboard.Height), cornerPoints, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);

		/*
		 * If desired number of corner are detected,
		 * we refine the pixel coordinates and display
		 * them on the images of checker board
		*/
		if (success)
		{
			cv::TermCriteria criteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.001);

			// refining pixel coordinates for given 2d points.
			cv::cornerSubPix(gray, cornerPoints, cv::Size(7, 7), cv::Size(-1, -1), criteria);

			// Displaying the detected corner points on the checker board
			cv::drawChessboardCorners(frame, cv::Size(checkerboard.Width, checkerboard.Height), cornerPoints, success);

			objPoints.push_back(objp);
			imgPoints.push_back(cornerPoints);
		}

		/*cv::imshow("Image", frame);
		cv::waitKey(0);*/
	}

	cv::destroyAllWindows();

	/*
	 * Performing camera calibration by
	 * passing the value of known 3D points (objpoints)
	 * and corresponding pixel coordinates of the
	 * detected corners (imgpoints)
	 */
	printf("calibrating camera");
	cv::calibrateCamera(objPoints, imgPoints, cv::Size(gray.rows, gray.cols), intrinsicMatrix, distCoeffs, R, T);

	std::cout << "cameraMatrix : " << intrinsicMatrix << std::endl;
	std::cout << "distCoeffs : " << distCoeffs << std::endl;
	std::cout << "Rotation vector : " << R << std::endl;
	std::cout << "Translation vector : " << T << std::endl;
}
