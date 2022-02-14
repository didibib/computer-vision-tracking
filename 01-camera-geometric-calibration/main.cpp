#include "cvpch.h"
#include "checkerboard.h"
#include "camera.h"

const int fps = 60;
const int delay = int(1000 / fps);

Checkerboard checkerboard(6, 9, 2.5f);
Camera camera;
cv::VideoCapture webcam(0);

void drawAxis(cv::Mat& frame, Camera& cam, std::vector<cv::Point3f> objPoints);

int main()
{
	camera.Calibrate(checkerboard, IMAGES_DIR_STR);

	printf("Starting camera\n");
	if (!webcam.isOpened())
		throw "Error when reading steam_avi";

	cv::Mat frame;
	while (webcam.read(frame))
	{
		if (camera.SolveFrame(checkerboard, frame))
		{
			std::vector<cv::Point3f> axis = {
				{ 0.f, 0.f, 0.f },
				{ 10.f, 0.f, 0.f },
				{ 0.f, 10.f, 0.f },
				{ 0.f, 0.f, -10.f }
			};

			drawAxis(frame, camera, axis);
		}
		imshow("Video", frame);
		cv::waitKey(1);
	}

	webcam.release();
	return 0;
}

void drawAxis(cv::Mat& frame, Camera& cam, std::vector<cv::Point3f> objPoints)
{
	auto imgPoints = cam.Project(objPoints);
	// Draw the x-axis 
	cv::line(frame, imgPoints[0], imgPoints[1], cv::Scalar(0,0,255), 4);
	// Draw the y-axis 
	cv::line(frame, imgPoints[0], imgPoints[2], cv::Scalar(0,255,0), 4);
	// Draw the z-axis 
	cv::line(frame, imgPoints[0], imgPoints[3], cv::Scalar(255,0,0), 4);
}