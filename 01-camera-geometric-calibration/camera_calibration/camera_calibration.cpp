#include "pch/cvpch.h"
#include "checkerboard/checkerboard.h"
#include "camera_calibration.h"

// We used the camera calibration from the following tutorial:
// https://learnopencv.com/camera-calibration-using-opencv/
void CameraCalibration::Calibrate(Checkerboard const& checkerboard, std::string path)
{
	// Creating vector to store vectors of 3D points for each checkerboard image
	std::vector<std::vector<cv::Point3f> > objPoints;

	// Creating vector to store vectors of 2D points for each checkerboard image
	std::vector<std::vector<cv::Point2f> > imgPoints;

	// Extracting path of individual image stored in a given directory
	std::vector<cv::String> images;
	cv::glob(path, images);

	cv::Mat frame;

	// Looping over all the images in the directory
	for (int i{ 0 }; i < images.size(); i++)
	{
		frame = cv::imread(images[i]);
		std::vector<cv::Point3f> objp;
		std::vector<cv::Point2f> imgp;
		if (checkerboard.FindPoints(frame, objp, imgp))
		{
			objPoints.push_back(objp);
			imgPoints.push_back(imgp);
			// Possible to imshow the current frame with drawn points
#ifdef _DEBUG
			cv::imshow("Image", frame);
			cv::waitKey(0);
#endif // DEBUG	
		}
	}

	cv::destroyAllWindows();

	/*
	 * Performing camera calibration by
	 * passing the value of known 3D points (objpoints)
	 * and corresponding pixel coordinates of the
	 * detected corners (imgpoints)
	 */
	printf("calibrating camera");
	cv::calibrateCamera(objPoints, imgPoints, cv::Size(frame.rows, frame.cols), mIntrinsicMatrix, mDistCoeffs, mR, mT);

	std::cout << "cameraMatrix : " << mIntrinsicMatrix << std::endl;
	std::cout << "distCoeffs : " << mDistCoeffs << std::endl;
	std::cout << "Rotation vector : " << mR << std::endl;
	std::cout << "Translation vector : " << mT << std::endl;
}
