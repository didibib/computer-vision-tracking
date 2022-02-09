#include "pch/cvpch.h"
#include "camera_calibration/camera_calibration.h"

Checkerboard checkerboard { 6, 9 };
CameraCalibration calibrator;

int main()
{
    calibrator.Calibrate(checkerboard);

    return 0;
}

