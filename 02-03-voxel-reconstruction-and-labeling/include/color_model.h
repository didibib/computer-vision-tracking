#pragma once 

namespace team45
{

	class Histogram
	{
	public:
		/*
		 * Calculate histogram for each channel
		 */
		void calculate(cv::Mat& hsv);
		
		/*
		 * @return d(H1, H2) using Chi-Sqr
		 */
		float compare(Histogram& other);
		
		/* 
		 * Call after calculate()
		 */
		void draw();

	private:
		cv::Mat m_H_hist;
		cv::Mat m_S_hist;
		cv::Mat m_V_hist;
		static const int m_hist_size = 32; 
	};
}