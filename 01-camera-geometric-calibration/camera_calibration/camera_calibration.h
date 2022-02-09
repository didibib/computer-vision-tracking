#pragma once



class CameraCalibration
{
public:
	/*
	   @return The intrinsic floating-point camera matrix.
	*/
	cv::Mat GetIntrinsicMatrix() { return intrinsicMatrix; }
	/* @return the Rotation matrix R and Translation matrix T concatenate together, like [R | T].
		Also known as the extrinsic floating-point camera matrix.
	*/
	cv::Mat GetExtrinsicMatrix() 
	{
		cv::Mat extrinsic;
		cv::hconcat(R, T, extrinsic);
		return extrinsic;
	}


	/* test
	   @param The path to image folder
	*/
	void Calibrate(Checkerboard const&);
private:
	cv::Mat intrinsicMatrix, distCoeffs, R, T;
};