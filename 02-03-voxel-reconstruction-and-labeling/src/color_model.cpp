#include "cvpch.h"
#include "color_model.h"
#include "util.h"

using namespace cv;
namespace team45
{
	void Histogram::calculate(std::vector<cv::Point3f>const& colors, std::vector<cv::Point3f> const& bins)
	{
		bees = bins;
		m_hist.resize(bins.size());

		// For each color that we find, calculate the distance to each bin
		for (int i = 0; i < colors.size(); i++)
		{
			double shortestDist = DBL_MAX;
			int closestBin = 0;
			for (int b = 0; b < bins.size(); b++)
			{
				cv::Vec3f v = colors[i] - bins[b];
				// Is actually |v|^2, but does not matter
				double distance = v.dot(v);
				if (distance < shortestDist)
				{
					closestBin = b;
					shortestDist = distance;
				}
			}
			m_hist[closestBin]++;
		}

		// Normalize our histogram by dividing every element by the maximum element;
		float maxElem = -1;
		for (int b = 0; b < m_hist.size(); b++)
		{
			if (m_hist[b] > maxElem)
			{
				maxElem = m_hist[b];
			}
		}

		for (int b = 0; b < m_hist.size(); b++)
			m_hist[b] /= maxElem;
	}

	float Histogram::compare(Histogram& other)
	{
		float d = 0;

		assert(m_hist.size() == other.m_hist.size());

		// Chi-Square dist
		for (int b = 0; b < m_hist.size(); b++)
		{
			float diff = (m_hist[b] - other.m_hist[b]);
			d += diff * diff / (float)m_hist[b];
		}

		return d;
	}

	// https://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/histogram_calculation/histogram_calculation.html
	void Histogram::draw()
	{
		//// Draw the histograms for H, S, V
		int hist_w = 512; int hist_h = 400;
		int bin_w = cvRound((double)hist_w / (m_hist.size() + 1));
		Mat hist_image(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

		for (int i = 1; i < m_hist.size(); i++)
		{
			rectangle(hist_image,
				Point2f(bin_w * (i - .5f), hist_h),
				Point2f(bin_w * (i + .5f), hist_h - m_hist[i] * hist_h),
				Scalar(bees[i].x, bees[i].y, bees[i].z), -2, 8, 0);
		}

		// Display
		imshow(util::get_name_rand("Histogram", m_id), hist_image);
		waitKey();
	}

	void Histogram::save(cv::FileStorage fs, std::string nodename)
	{
		fs << nodename << "{";
		fs << "Id" << m_id;
		fs << "Hist" << m_hist;
		fs << "}";
	}

	void Histogram::load(cv::FileNode fn)
	{
		fn["Id"] >> m_id;
		fn["Hist"] >> m_hist;
	}
}

