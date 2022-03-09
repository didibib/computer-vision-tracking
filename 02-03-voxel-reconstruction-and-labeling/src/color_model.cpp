#include "cvpch.h"
#include "color_model.h"

using namespace cv;
namespace team45
{
	void Histogram::calculate(cv::Mat& hsv)
	{
		std::vector<cv::Mat> hsv_planes;
		// Split image by channel
		cv::split(hsv, hsv_planes);

		// Create mask
		cv::Mat mask, tmp;
		cv::cvtColor(hsv, tmp, COLOR_BGR2GRAY);
		cv::threshold(tmp, mask, 1, 255, cv::THRESH_BINARY);

		// Calculate histogram
		static const float _hrange[] = { 0, 180 };
		static const float _svrange[] = { 0, 256 };
		static const float* hrange[] = { _hrange };
		static const float* svrange[] = { _svrange };

		cv::calcHist(&hsv_planes[0], 1, 0, mask, m_H_hist, 1, &m_hist_size, hrange);
		cv::calcHist(&hsv_planes[1], 1, 0, mask, m_S_hist, 1, &m_hist_size, svrange);
		cv::calcHist(&hsv_planes[2], 1, 0, mask, m_V_hist, 1, &m_hist_size, svrange);
	}

	float Histogram::compare(Histogram& other)
	{
		float d = 0;
		d += cv::compareHist(m_H_hist, other.m_H_hist, HISTCMP_CHISQR);
		d += cv::compareHist(m_S_hist, other.m_S_hist, HISTCMP_CHISQR);
		d += cv::compareHist(m_V_hist, other.m_V_hist, HISTCMP_CHISQR);
		return d;
	}

	// https://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/histogram_calculation/histogram_calculation.html
	void Histogram::draw()
	{
		// Draw the histograms for H, S, V
		int hist_w = 512; int hist_h = 400;
		int bin_w = cvRound((double)hist_w / m_hist_size);
		Mat hist_image(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

		// Normalize the result to [ 0, hist_image.rows ]
		normalize(m_H_hist, m_H_hist, 0, hist_image.rows, NORM_MINMAX, -1, Mat());
		normalize(m_S_hist, m_S_hist, 0, hist_image.rows, NORM_MINMAX, -1, Mat());
		normalize(m_V_hist, m_V_hist, 0, hist_image.rows, NORM_MINMAX, -1, Mat());

		// Draw for each channel
		for (int i = 1; i < m_hist_size; i++)
		{
			line(hist_image, Point(bin_w * (i - 1), hist_h - cvRound(m_H_hist.at<float>(i - 1))),
				Point(bin_w * (i), hist_h - cvRound(m_H_hist.at<float>(i))),
				Scalar(255, 0, 0), 2, 8, 0);
			line(hist_image, Point(bin_w * (i - 1), hist_h - cvRound(m_S_hist.at<float>(i - 1))),
				Point(bin_w * (i), hist_h - cvRound(m_S_hist.at<float>(i))),
				Scalar(0, 255, 0), 2, 8, 0);
			line(hist_image, Point(bin_w * (i - 1), hist_h - cvRound(m_V_hist.at<float>(i - 1))),
				Point(bin_w * (i), hist_h - cvRound(m_V_hist.at<float>(i))),
				Scalar(0, 0, 255), 2, 8, 0);
		}

		// Display
		namedWindow("Histogram", CV_WINDOW_AUTOSIZE);
		imshow("Histogram", hist_image);

		waitKey();
	}
}

