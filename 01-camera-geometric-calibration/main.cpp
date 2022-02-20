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
	{ util::cube[0], util::cube[1], util::white },
	{ util::cube[1], util::cube[2], util::white  },
	{ util::cube[2], util::cube[3], util::white  },
	{ util::cube[3], util::cube[0], util::white  },

	{ util::cube[4], util::cube[5], util::white  },
	{ util::cube[5], util::cube[6], util::white  },
	{ util::cube[6], util::cube[7], util::white  },
	{ util::cube[7], util::cube[4], util::white  },

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
			drawAxis(frame, *camera);

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

void drawAxis(cv::Mat& frame, Camera& cam)
{
	auto imgPoints = cam.Project(util::axis);
	// Draw the x-axis 
	cv::line(frame, imgPoints[0], imgPoints[1], util::red, 4);
	// Draw the y-axis 
	cv::line(frame, imgPoints[0], imgPoints[2], util::green, 4);
	// Draw the z-axis 
	cv::line(frame, imgPoints[0], imgPoints[3], util::blue, 4);
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
	auto R = std::vector<float>{ t, t, 2 * t };
	auto M = util::createTransform(T, R);

	for (int i = 0; i < cubeSegments.size(); i++)
		cubeSegments[i].Update(cam, M);

	std::sort(cubeSegments.begin(), cubeSegments.end(), LineSegmentDepthSort);

	for (int i = 0; i < cubeSegments.size(); i++)
		cubeSegments[i].Draw(frame, cam);

	//drawImage(frame, { imgPoints[4], imgPoints[5], imgPoints[0], imgPoints[1] }, util::IMAGES_DIR_STR + "/square-face.jpg");

	//// Draw the cube
	//// Bottom frame
	//cv::line(frame, imgPoints[0], imgPoints[1], util::white, 4);
	//cv::line(frame, imgPoints[1], imgPoints[2], util::white, 4);
	//cv::line(frame, imgPoints[2], imgPoints[3], util::white, 4);
	//cv::line(frame, imgPoints[3], imgPoints[0], util::white, 4);
	//// Top frame								
	//cv::line(frame, imgPoints[4], imgPoints[5], util::white, 4);
	//cv::line(frame, imgPoints[5], imgPoints[6], util::white, 4);
	//cv::line(frame, imgPoints[6], imgPoints[7], util::white, 4);
	//cv::line(frame, imgPoints[7], imgPoints[4], util::white, 4);
	//// Connecting pillars						
	//cv::line(frame, imgPoints[0], imgPoints[4], util::white, 4);
	//cv::line(frame, imgPoints[1], imgPoints[5], util::white, 4);
	//cv::line(frame, imgPoints[2], imgPoints[6], util::white, 4);
	//cv::line(frame, imgPoints[3], imgPoints[7], util::white, 4);
} 



// https://medium.com/acmvit/how-to-project-an-image-in-perspective-view-of-a-background-image-opencv-python-d101bdf966bc
void drawImage(cv::Mat& frame, std::vector<cv::Point2f> dstPoints, std::string imgPath)
{
	cv::Mat image = cv::imread(imgPath);
	std::vector<cv::Point2f> imgPoints{
		cv::Point2f(0.f, 0.f),
		cv::Point2f(image.cols, 0.f),
		cv::Point2f(0.f, image.rows),
		cv::Point2f(image.cols, image.rows) };

	// Find the perspective transformation between planes
	cv::Mat h = cv::findHomography(imgPoints, dstPoints, cv::RANSAC, 5.0);
	cv::Mat warped;

	// Use the homography matrix to warp our image
	cv::warpPerspective(image, warped, h, cv::Size(frame.cols, frame.rows));

	// Create a mask where everything is white, but the image is black
	cv::Mat invMask;
	cv::threshold(warped, invMask, 0, 255, cv::THRESH_BINARY_INV);

	// Combine the mask and the current frame
	cv::Mat frameMasked;
	cv::bitwise_and(frame, invMask, frameMasked);

	// Combine the frame with the warped image
	cv::bitwise_or(frameMasked, warped, frame);
}