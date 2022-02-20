#pragma once

class Checkerboard;
class Camera
{
public:	
	Camera() = delete;
	Camera(const cv::FileNode& node, Checkerboard);
	/*
	   @param The path to image folder
	*/
	void Calibrate(Checkerboard const&, std::string path);

	/*
		Modifies the R and T matrices to accurately represent the current frame  
	*/
	bool SolveFrame(Checkerboard const&, cv::Mat frame);

	/*
		Projects the given points to the world points 
	*/
	std::vector<cv::Point2f> Project(std::vector<cv::Point3f> const& objPoints) const;

	/*
	   @return The intrinsic floating-point camera matrix.
	*/
	cv::Mat Intrinsic() const { return mIntrinsic; }
	/*
	   @return the Rotation matrix R and Translation matrix T concatenated together, like [R | T].
		Also known as the extrinsic floating-point camera matrix.
	*/
	cv::Mat Extrinsic() const { return mExtrinsic; }

	cv::Mat Position() const { return -1 * mT; }

private:
	cv::Mat mIntrinsic, mDistCoeffs, mR, mRvec, mT, mExtrinsic;
	void Save(std::string fileName);
};