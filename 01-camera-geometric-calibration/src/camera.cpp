#include "cvpch.h"
#include "checkerboard.h"
#include "camera.h"

// We used the camera calibration from the following tutorial:
// https://learnopencv.com/camera-calibration-using-opencv/
void Camera::Calibrate(Checkerboard const& checkerboard, std::string path)
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
	for (int i{ 0 }; i < images.size() / 2; i++)
	{
		frame = cv::imread(images[i]);
		std::vector<cv::Point3f> objp;
		std::vector<cv::Point2f> imgp;
		// Find the image points of the checkerboard corners / intersections
		if (checkerboard.FindPoints(frame, objp, imgp, true))
		{
			// Add the pattern to our correct frames, aka frames where we recognize a checkeboard
			objPoints.push_back(objp);
			imgPoints.push_back(imgp);

// Possible to imshow the current frame with drawn points
#ifdef _DEBUG
			printf("Image %d\n", i);
			cv::imshow("Image", frame);
			cv::waitKey(1);
#endif
		}
	}

	cv::destroyAllWindows();

	 // We calibrate our camera using our known 3D points and respective image points
	printf("Calibrating camera\n");
	cv::calibrateCamera(objPoints, imgPoints, cv::Size(frame.rows, frame.cols), mIntrinsic, mDistCoeffs, mR, mT);
	printf("Calibrating finished\n");

	std::cout << "cameraMatrix : "			<< mIntrinsic << std::endl;
	std::cout << "distCoeffs : "			<< mDistCoeffs << std::endl;
}

/*
	Finds the extrinsics of our camera if there is a checkerboard present in the frame.
	@param The checkerboard we expect to find.
	@param The image in which we want to find the checkerboard.
	@return True if successful, else false
*/
bool Camera::SolveFrame(Checkerboard const& checkerboard, cv::Mat frame)
{
	std::vector<cv::Point3f> objPoints;
	std::vector<cv::Point2f> imgPoints;
	/* See if we recognize a checkerboard.
	   If not, we can return early.
	*/
	if (!checkerboard.FindPoints(frame, objPoints, imgPoints)) return false;
	
	// Find the extrinsics of our camera (Rotation and Translation). 
	return cv::solvePnP(objPoints, imgPoints, mIntrinsic, mDistCoeffs, mR, mT);
}

/*
	Converts the given object points (in 3D coordinates) to corresponding image points (pixel coordinates)
*/
std::vector<cv::Point2f> Camera::Project(std::vector<cv::Point3f> const& objPoints)
{
	std::vector<cv::Point2f> imgPoints;
	cv::projectPoints(objPoints, mR, mT, mIntrinsic, mDistCoeffs, imgPoints);
	return imgPoints;
}

