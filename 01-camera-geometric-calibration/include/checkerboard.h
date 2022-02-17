#pragma once 

class Checkerboard
{
public:
	Checkerboard() = delete;
	Checkerboard(const cv::FileNode& node);

	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }
	float GetCm() const { return mCm; }

	int FindPoints(cv::Mat& frame, std::vector<cv::Point3f>& objPoints, std::vector<cv::Point2f>& imgPoints, bool drawCorners = false) const;

private:
	int mWidth;
	int mHeight;
	float mCm;	
};