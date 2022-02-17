#pragma once

class Camera;
class Ball
{
public:
	Ball(cv::Point3d position, int radius);
	void Update(float t);
	void Draw(cv::Mat& frame, Camera const& camera);
private:
	cv::Mat mPosition;
	double mRadius;

};