#include "pch/cvpch.h"
#include "checkerboard/checkerboard.h"
#include "camera_calibration/camera_calibration.h"

Checkerboard checkerboard { 6, 9,  2.5f };
CameraCalibration calibrator;

int main()
{
    calibrator.Calibrate(checkerboard, IMAGES_DIR_STR);

    return 0;
}

