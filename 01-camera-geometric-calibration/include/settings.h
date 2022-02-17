#pragma once

#include "cvpch.h"
#include "camera.h"
#include "checkerboard.h"

namespace Settings
{
	static std::string ReadConfig(const cv::FileNode & node, Checkerboard * checkerboard)
	{
		int width, height;
		float size;
		node["BoardSize_Width"] >> width;
		node["BoardSize_Height"] >> height;
		node["Square_Size"] >> size;
		checkerboard = new Checkerboard(width, height, size);

		std::string calibDataLocation;
		node["Write_outputFileName"] >> calibDataLocation;
		return calibDataLocation;
	}

	static void ReadCalibrationData(const cv::FileNode& node, Camera* camera)
	{

	}

	void Write(cv::FileStorage& fs, Camera camera, Checkerboard checkerboard)
	{

	}
}