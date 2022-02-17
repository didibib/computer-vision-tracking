#include "cvpch.h"
#include "checkerboard.h"
#include "camera.h"
#include "ball.h"

const std::string window = "Video";

Checkerboard* checkerboard;
Camera* camera;
// if your webcam does not open, change the number in the constructor below
cv::VideoCapture webcam(1);

float t = 0.f;

std::vector<Ball> balls{
	{cv::Point3d(0, 0, -10), 4, util::red},
	{cv::Point3d(5, 1, -5), 2, util::blue},
	{cv::Point3d(-3, -4, -2), 2, util::green},
	{cv::Point3d(1, 1, -1), 2, util::gray},
};

int main()
{
	/*
		Load config file
	*/
	cv::FileStorage fs(util::CONFIG, cv::FileStorage::READ);
	if (!fs.isOpened())
	{
		throw "No config file found";
	}
	auto node = fs["Settings"];
	checkerboard = new Checkerboard(node);
	camera = new Camera(node, *checkerboard);

	printf("Starting camera\n");
	if (!webcam.isOpened())
		throw "Error when reading steam_avi";

	cv::Mat frame;
	while (webcam.read(frame))
	{
		/*
			Then, for each frame, we calculate our extrinsic matrix (R and T) using our intrinsic matrix.
		*/
		if (camera->SolveFrame(*checkerboard, frame))
		{
			util::drawAxis(frame, *camera);

			for (int i = 0; i < balls.size(); i++)
				balls[i].Update(t, *camera);

			std::sort(balls.begin(), balls.end(), ballDepthSort);

			for (int i = 0; i < balls.size(); i++)
				balls[i].Draw(frame, *camera);
			//util::drawCube(frame, camera, t);
			//ball.Draw(frame, *camera);
		}

		imshow(window, frame);
		cv::waitKey(1);
		t += .05f;
	}

	webcam.release();
	delete checkerboard;
	delete camera;
	return 0;
}

