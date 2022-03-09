#include "cvpch.h"
#include "color_model.h"


using namespace cv;
namespace team45
{
	void Histogram::calculate(cv::Mat& hsv)
	{
		std::vector<cv::Mat> hsv_planes;
		cv::split(hsv, hsv_planes);

		cv::Mat mask, tmp;
		cv::cvtColor(hsv, tmp, COLOR_BGR2GRAY);
		cv::threshold(tmp, mask, 1, 255, cv::THRESH_BINARY);

		static const float hrange[] = { 0, 180 };
		static const float srange[] = { 0, 256 };
		static const float vrange[] = { 0, 256 };
		static const float* hhrange[] = { hrange };
		static const float* ssrange[] = { srange };
		static const float* vvrange[] = { vrange };

		cv::calcHist(&hsv_planes[0], 1, 0, mask, m_H_hist, 1, &m_hist_size, hhrange);
		cv::calcHist(&hsv_planes[1], 1, 0, mask, m_S_hist, 1, &m_hist_size, ssrange);
		cv::calcHist(&hsv_planes[2], 1, 0, mask, m_V_hist, 1, &m_hist_size, vvrange);
	}

	// https://docs.opencv.org/2.4/doc/tutorials/imgproc/histograms/histogram_calculation/histogram_calculation.html
	void Histogram::draw()
	{
		// Draw the histograms for H, S, V
		int hist_w = 512; int hist_h = 400;
		int bin_w = cvRound((double)hist_w / m_hist_size);
		Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

		/// Normalize the result to [ 0, histImage.rows ]
		normalize(m_H_hist, m_H_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
		normalize(m_S_hist, m_S_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
		normalize(m_V_hist, m_V_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

		/// Draw for each channel
		for (int i = 1; i < m_hist_size; i++)
		{
			line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(m_H_hist.at<float>(i - 1))),
				Point(bin_w * (i), hist_h - cvRound(m_H_hist.at<float>(i))),
				Scalar(255, 0, 0), 2, 8, 0);
			line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(m_S_hist.at<float>(i - 1))),
				Point(bin_w * (i), hist_h - cvRound(m_S_hist.at<float>(i))),
				Scalar(0, 255, 0), 2, 8, 0);
			line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(m_V_hist.at<float>(i - 1))),
				Point(bin_w * (i), hist_h - cvRound(m_V_hist.at<float>(i))),
				Scalar(0, 0, 255), 2, 8, 0);
		}

		/// Display
		namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
		imshow("calcHist Demo", histImage);

		waitKey();
	}
}

