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

		void save(cv::FileStorage fs, std::string nodename);
		void load(cv::FileNode fn);
		void setId(int const& id) { m_id = id; }
		int const& getId() const { return m_id; }

	private:
		int m_id;							// ID of person
		cv::Mat m_H_hist;
		cv::Mat m_S_hist;
		cv::Mat m_V_hist;
		static const int m_hist_size = 16; 
	};
}