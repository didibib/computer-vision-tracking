#include "cvpch.h"
#include "checkerboard.h"
#include "camera.h"

Camera::Camera(const cv::FileNode& node, Checkerboard checkerboard)
{
	std::string dataLoc;
	node["Calibration_Output_File_Name"] >> dataLoc;

	cv::FileStorage calibData(util::SETTINGS_DIR_STR + "/" + dataLoc, cv::FileStorage::READ); // Read the settings
	if (calibData.isOpened())
	{
		printf("Using existing calibration\n");
		calibData["Intrinsic_Matrix"] >> mIntrinsic;
		calibData["Distance_Coefficients"] >> mDistCoeffs;
	}
	else
	{
		std::string folder;
		node["Calibration_Images_Folder"] >> folder;
		Calibrate(checkerboard, util::IMAGES_DIR_STR + "/" + folder);
		// Write to output
		Save(dataLoc);
	}
}

void Camera::Save(std::string fileName)
{
	cv::FileStorage fs(util::SETTINGS_DIR_STR + "/" + fileName, cv::FileStorage::WRITE);
	fs << "Intrinsic_Matrix" << mIntrinsic;
	fs << "Distance_Coefficients" << mDistCoeffs;
}

// We used the camera calibration from the following tutorial:
// https://learnopencv.com/camera-calibration-using-opencv/
void Camera::Calibrate(Checkerboard const& checkerboard, std::string path)
{
	// Creating vector to store vectors of 3D points for each checkerboard image
	std::vector<std::vector<cv::Point3f>> objPoints;

	// Creating vector to store vectors of 2D points for each checkerboard image
	std::vector<std::vector<cv::Point2f>> imgPoints;

	// Extracting path of individual image stored in a given directory
	std::vector<cv::String> images;
	cv::glob(path, images);

	cv::Mat frame;

	// Looping over all the images in the directory
	for (int i = 0; i < 2/*images.size()*/; i++)
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
#ifdef _DEBUG
			cv::imshow("Image", frame);
			cv::waitKey(1);
#endif
			printf("Found checkerboard on image %d\n", i);
		}
		else
		{
			printf("Did not find checkerboard on image %d\n", i);
		}
	}

	cv::destroyAllWindows();

	// CALIBRATION
	// We calibrate our camera using our known 3D points and respective image points
	double currError = cv::calibrateCamera(objPoints, imgPoints, cv::Size(frame.rows, frame.cols), mIntrinsic, mDistCoeffs, mR, mT);

	for (int i = 0; i < objPoints.size(); i++)
	{
		if (objPoints.size() <= 1)
			continue;

		// Temporarily remove objPoint
		auto tempObjPoint = objPoints[i];
		objPoints[i] = objPoints[objPoints.size() - 1];
		objPoints.resize(objPoints.size() - 1);

		// Temporarily remove imgPoint
		auto tempImgPoint = imgPoints[i];
		imgPoints[i] = imgPoints[imgPoints.size() - 1];
		imgPoints.resize(imgPoints.size() - 1);

		// Calibrate
		double newError = cv::calibrateCamera(objPoints, imgPoints, cv::Size(frame.rows, frame.cols), mIntrinsic, mDistCoeffs, mR, mT);
		printf("i: %i, error: %f", i, newError);
		
		if (newError <= currError)
		{
			// Our new calibration is better, so we can look at the 'old' last element next.
			// We just moved this element to index i, so we want to re-evaluate the current index.
			i--;
			currError = newError;
			printf("Calibration got better, so discard image %i\n", i);
		}
		else
		{
			// Old code
			//objPoints.resize(objPoints.size() + 1);
			//objPoints[objPoints.size()] = objPoints[i];

			// We want to undo the removal of the points on indices i
			objPoints.push_back(objPoints[i]);
			objPoints[i] = tempObjPoint;			
			imgPoints.push_back(imgPoints[i]);
			imgPoints[i] = tempImgPoint;
			printf("Calibration got worse, so keep image %i\n", i);

		}
	}

	double finalError = cv::calibrateCamera(objPoints, imgPoints, cv::Size(frame.rows, frame.cols), mIntrinsic, mDistCoeffs, mR, mT);
	printf("Calibrated camera with error %f\n", finalError);

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
