#pragma once 

class Checkerboard
{
public:
	Checkerboard(int width, int height, float cm)
	{
		mWidth = width;
		mHeight = height;
		mCm = cm;		
	}
	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }
	float GetCm() const { return mCm; }

	int FindPoints(cv::Mat& frame, std::vector<cv::Point3f>& objPoints, std::vector<cv::Point2f>& imgPoints) const;

private:
	int mWidth;
	int mHeight;
	float mCm;	
};