#pragma once 

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

const static std::string IMAGES_DIR_STR = TOSTRING(IMAGES_DIR);

struct Checkerboard
{
	Checkerboard(int width, int height)
	{
		Width = width;
		Height = height;
	}
	int Width;
	int Height;
};