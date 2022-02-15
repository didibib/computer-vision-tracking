#pragma once

class Checkerboard;
class Camera
{
public:	
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
	std::vector<cv::Point2f> Project(std::vector<cv::Point3f> const& objPoints);

	/*
	   @return The intrinsic floating-point camera matrix.
	*/
	cv::Mat Intrinsic() { return mIntrinsic; }
	/*
	   @return the Rotation matrix R and Translation matrix T concatenate together, like [R | T].
		Also known as the extrinsic floating-point camera matrix.
	*/
	cv::Mat Extrinsic()
	{
		cv::Mat extrinsic;
		// maybe we need to make this 4x4 instead of 3x4? 
		cv::hconcat(mR, mT, extrinsic);
		return extrinsic;
	}
private:
	cv::Mat mIntrinsic, mDistCoeffs, mR, mT;
};