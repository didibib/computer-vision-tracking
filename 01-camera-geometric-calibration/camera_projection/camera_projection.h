#pragma once

class CameraProjection
{
public:
	CameraProjection();
	void SolveExtrinsic(cv::Mat intrinsic, cv::Mat distCoeffs);
private:

};