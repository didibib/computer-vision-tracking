#pragma once 

namespace team45
{
	class Histogram
	{
	public:
		/*
		 * Calculate histogram for each channel
		 */
		void calculate(std::vector<cv::Point3f> const& colors, std::vector<cv::Point3f>const& bins);
		
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
		std::vector<float> m_hist;
		static const int m_hist_size = 16; 
	};
}