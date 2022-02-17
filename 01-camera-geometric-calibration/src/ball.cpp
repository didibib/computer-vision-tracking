#include "cvpch.h"
#include "ball.h"
#include "camera.h"

Ball::Ball(cv::Point3d pos, int radius)
{
	mPosition = (cv::Mat_<double>(3, 1) << pos.x, pos.y, pos.z);
	mRadius = radius;
}

void Ball::Update(float t)
{

}

/*
	1. Find the ball's center relative to the camera (using camera extrinsics)
	2. Get orthogonal vector from the camera's position to the ball center (cross product between y-axis and direction)
	3. Make the length of this vector equal to the ball's radius
	4. Now we have 2 points of the circle that get projected by the camera accordingly
	5. Project the points, and find the distance between them in image coordinates
	6. Draw a circle on the projected center with radius equal to the distance from 5.
*/
void Ball::Draw(cv::Mat& frame, Camera const& camera)
{
	// 1. Find the ball's center relative to the camera (using camera extrinsics)
	cv::Mat camToBall = mPosition - camera.Position();
	std::cout << "ballPos: " << mPosition << std::endl;
	std::cout << "camPos: " << camera.Position() << std::endl;

	// 2. Get orthogonal vector from the camera's position to the ball center (cross product between y-axis and direction)
	cv::Mat zaxis = (cv::Mat_<double>(3, 1) << 0, 0, 1);
	cv::Mat ortho = camToBall.cross(zaxis);
	double length = sqrt(ortho.dot(ortho));

	// 3. Make the length of this vector equal to the ball's radius
	cv::Mat ballSurfacePos = mPosition + (ortho / length) * mRadius;

	cv::Point3f center3D(mPosition.at<double>(0), mPosition.at<double>(1), mPosition.at<double>(2));
	cv::Point3f surface3D(ballSurfacePos.at<double>(0), ballSurfacePos.at<double>(1), ballSurfacePos.at<double>(2));

	std::vector<cv::Point2f> points2D = camera.Project({ center3D, surface3D });

	cv::Point2f center2D = points2D[0];
	cv::Point2f surface2D = points2D[1];

	cv::Point2f diff = surface2D - center2D;
	double projectedRadius = sqrt(diff.x * diff.x + diff.y * diff.y);
	cv::circle(frame, center2D, projectedRadius, util::blue, -1);
}