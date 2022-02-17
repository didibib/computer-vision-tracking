#pragma once

class Camera;
class Ball
{
public:
	Ball(cv::Point3d position, int radius, cv::Scalar color);
	void Update(float t, Camera& camera);
	void Draw(cv::Mat& frame, Camera const& camera);
	float GetDistToCam() const { return mDistToCam; }

private:
	cv::Mat mPosition;
	double mRadius;
	float mDistToCam;
	cv::Mat mCamToBall;
	cv::Scalar mColor;
};

static bool ballDepthSort(Ball i, Ball j)
{
	return i.GetDistToCam() > j.GetDistToCam();
}