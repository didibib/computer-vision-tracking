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

	double currError = std::numeric_limits<double>::max();

	// Looping over all the images in the directory
	for (int i{ 0 }; i < images.size() / 2; i++)
	{
		frame = cv::imread(images[i]);
		std::vector<cv::Point3f> objp;
		std::vector<cv::Point2f> imgp;
		// Find the image points of the checkerboard corners / intersections
		if (checkerboard.FindPoints(frame, objp, imgp, true))
		{
			// Add the pattern to our correct frames, aka frames where we recognize a checkerboard
			objPoints.push_back(objp);
			imgPoints.push_back(imgp);

			auto size = cv::Size(frame.rows, frame.cols);
			// Our new result matricess
//			cv::Mat newIntrinsic, newDistCoeffs, newR, newT;
//			double newError = cv::calibrateCamera(objPoints, imgPoints, size, newIntrinsic, newDistCoeffs, newR, newT);
//			printf("%d Error: %f\n", i, newError);
//			if (newError <= currError)
//			{
//				// We found a better calibration
//				mIntrinsic = newIntrinsic;
//				mDistCoeffs = newDistCoeffs;
//				currError = newError;
//				// Possible to imshow the current frame with drawn points
//				printf("Using image %d\n", i);
//#ifdef _DEBUG
//				cv::imshow("Image", frame);
//				cv::waitKey(1);
//#endif
//			}
//			else
//			{
//				// Calibration got worse, so remove this view from our set 
//				objPoints.pop_back();
//				imgPoints.pop_back();
//				printf("Discarding image %d\n", i);
//			}
		}
	}

	cv::destroyAllWindows();

	// We calibrate our camera using our known 3D points and respective image points
	printf("Calibrating camera\n");
	cv::calibrateCamera(objPoints, imgPoints, cv::Size(frame.rows, frame.cols), mIntrinsic, mDistCoeffs, mR, mT);
	printf("Calibrating finished\n");

	std::cout << "cameraMatrix : " << mIntrinsic << std::endl;
	std::cout << "distCoeffs : " << mDistCoeffs << std::endl;
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
	bool succes = cv::solvePnP(objPoints, imgPoints, mIntrinsic, mDistCoeffs, mRvec, mT);
	// Create rotation matrix from vector
	cv::Rodrigues(mRvec, mR);
	// Convert intrinsic
	mIntrinsic.convertTo(mIntrinsic, CV_32F);
	// Create extrinsic and convert
	//mR.convertTo(mR, CV_32F);
	//mT.convertTo(mT, CV_32F);
	cv::hconcat(mR, mT, mExtrinsic);
	mExtrinsic.convertTo(mExtrinsic, CV_32F);
	return succes;
}

/*
	Converts the given object points (in 3D coordinates) to corresponding image points (pixel coordinates)
*/
std::vector<cv::Point2f> Camera::Project(std::vector<cv::Point3f> const& objPoints) const
{
	std::vector<cv::Point2f> imgPoints;
	cv::projectPoints(objPoints, mR, mT, mIntrinsic, mDistCoeffs, imgPoints);
	return imgPoints;
}