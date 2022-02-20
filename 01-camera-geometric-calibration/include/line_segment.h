#pragma once
#include "cvpch.h"
#include "camera.h"

class LineSegment
{
public:
	LineSegment(cv::Point3f start, cv::Point3f end, cv::Scalar color)
	{
		mStart = start;
		mEnd = end;
		mColor = color;
	}

	/*
		Updates the segment, using 4x4 transformation matrix M
	*/
	void Update(Camera const& camera, cv::Mat& M)
	{
		mTransformedStart = util::transform(mStart, M);
		mTransformedEnd = util::transform(mEnd, M);
		mStartDistToCam = DistToCam(camera, mTransformedStart);
		mEndDistToCam = DistToCam(camera, mTransformedEnd);
	}

	void Draw(cv::Mat& frame, Camera const& camera)
	{
		auto imgPoints = camera.Project({ mTransformedStart, mTransformedEnd });
		cv::line(frame, imgPoints[0], imgPoints[1], mColor, 4);
	}

	float GetDistToCam() { return MAX(mStartDistToCam, mEndDistToCam); }
	cv::Point3f GetTransformedStart() { return mTransformedStart; }
	cv::Point3f GetTransformedEnd() { return mTransformedEnd; }

private:
	// starting point
	cv::Point3f mStart;
	cv::Point3f mTransformedStart;
	// ending point
	cv::Point3f mEnd;
	cv::Point3f mTransformedEnd;
	float mStartDistToCam;
	float mEndDistToCam;
	cv::Scalar mColor;

	float DistToCam(Camera const& camera, cv::Point3f point)
	{
		cv::Mat pointMat = (cv::Mat_<double>(3, 1) << point.x, point.y, point.z);
		cv::Mat camToPoint = pointMat - camera.Position();
		cv::Point3f p(camToPoint.at<double>(0), camToPoint.at<double>(1), camToPoint.at<double>(2));
		return sqrtf(p.x * p.x + p.y * p.y + p.z * p.z);
	}
};

static bool LineSegmentDepthSort(LineSegment i, LineSegment j)
{
	return i.GetDistToCam() > j.GetDistToCam();
}