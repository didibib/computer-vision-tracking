#include "pch/cvpch.h"
#include "checkerboard/checkerboard.h"
#include "camera_calibration/camera_calibration.h"

Checkerboard checkerboard { 6, 9,  2.5f };
CameraCalibration calibrator;
cv::VideoCapture cam(0);


int main()
{
    //calibrator.Calibrate(checkerboard, IMAGES_DIR_STR);

    cv::Mat frame;

    if (!cam.isOpened())
    {
        std::cout << "is not opened";
    }

    while(true)
    {
        cam >> frame;
        cv::imshow("Video", frame);
    }

    return 0;
}

