#include "cvpch.h"
#include "checkerboard.h"
#include "camera_calibration.h"

const int fps = 60;
const int delay = int(1000 / fps);

Checkerboard checkerboard(6, 9, 2.5f);
CameraCalibration calibrator;
cv::VideoCapture cam(0);

int main()
{
	//calibrator.Calibrate(checkerboard, IMAGES_DIR_STR);

	if (!cam.isOpened())
		throw "Error when reading steam_avi";

	cv::Mat frame;
	while (cam.read(frame))
	{
		imshow("Video", frame);
		cv::waitKey(delay);
	}

	cam.release();
	return 0;
}