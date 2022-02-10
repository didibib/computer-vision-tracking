#pragma once

class Checkerboard;
class CameraCalibration
{
public:
	/*
	   @return The intrinsic floating-point camera matrix.
	*/
	cv::Mat GetIntrinsicMatrix() { return mIntrinsicMatrix; }
	/* 
		@return the Rotation matrix R and Translation matrix T concatenate together, like [R | T].
		Also known as the extrinsic floating-point camera matrix.
	*/
	cv::Mat GetExtrinsicMatrix() 
	{
		cv::Mat extrinsic;
		cv::hconcat(mR, mT, extrinsic);
		return extrinsic;
	}


	/* 
	   @param The path to image folder
	*/
	void Calibrate(Checkerboard const&, std::string path);
private:
	cv::Mat mIntrinsicMatrix, mDistCoeffs, mR, mT;
};