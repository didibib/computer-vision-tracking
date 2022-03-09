#pragma once 

namespace team45
{

	class Histogram
	{
	public:
		void calculate(cv::Mat& hsv);
		void draw();

	private:
		cv::Mat m_H_hist;
		cv::Mat m_S_hist;
		cv::Mat m_V_hist;
		static const int m_hist_size = 32; 
	};
}