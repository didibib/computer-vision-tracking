#include "cvpch.h"
#include "checkerboard.h"
#include "camera.h"
#include "ball.h"
#include "line_segment.h"

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

std::vector<LineSegment> cubeSegments{
	{ util::cube[0], util::cube[1], util::blue },
	{ util::cube[1], util::cube[2], util::blue  },
	{ util::cube[2], util::cube[3], util::blue  },
	{ util::cube[3], util::cube[0], util::blue  },
	{ util::cube[0], util::cube[2], util::blue  },

	{ util::cube[4], util::cube[5], util::red  },
	{ util::cube[5], util::cube[6], util::red  },
	{ util::cube[6], util::cube[7], util::red  },
	{ util::cube[7], util::cube[4], util::red  },
	{ util::cube[4], util::cube[6], util::red  },

	{ util::cube[0], util::cube[4], util::white  },
	{ util::cube[1], util::cube[5], util::white  },
	{ util::cube[2], util::cube[6], util::white  },
	{ util::cube[3], util::cube[7], util::white  }
};

std::vector<cv::Point3f> transform(std::vector<cv::Point3f> const& points, std::vector<float> T, std::vector<float> rotation);
void drawAxis(cv::Mat& frame, Camera& cam);
void drawCube(cv::Mat& frame, Camera& cam, float t);
void drawImage(cv::Mat& frame, std::vector<cv::Point2f> dstPoints, std::string imgPath);

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

			drawCube(frame, *camera, t);
			//drawCube(frame, camera, t);
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


void drawBalls(cv::Mat& frame, Camera& cam, float t)
{
	for (int i = 0; i < balls.size(); i++)
		balls[i].Update(t, *camera);

	std::sort(balls.begin(), balls.end(), BallDepthSort);

	for (int i = 0; i < balls.size(); i++)
		balls[i].Draw(frame, *camera);
}

void drawCube(cv::Mat& frame, Camera& cam, float t)
{
	auto T = std::vector<float>{ sin(t) * 10, cos(t) * 10, 0.f };
	auto R = std::vector<float>{ t, t, 0 };
	auto M = util::createTransform(T, R);

	for (int i = 0; i < cubeSegments.size(); i++)
		cubeSegments[i].Update(cam, M);

	std::sort(cubeSegments.begin(), cubeSegments.end(), LineSegmentDepthSort);

	for (int i = 0; i < cubeSegments.size(); i++)
		cubeSegments[i].Draw(frame, cam);
}
