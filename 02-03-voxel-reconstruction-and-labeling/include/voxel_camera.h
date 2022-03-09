#pragma once
#ifndef CAMERA_H
#define CAMERA_H

namespace team45
{
#define MAIN_WINDOW "Checkerboard Marking"

	class Histogram;

	class VoxelCamera
	{
		static std::vector<cv::Point>* m_BoardCorners;		// Marked checkerboard corners

		const std::string m_data_path;						// Path to data directory
		const int m_id;										// Camera ID

		std::vector<Histogram*> m_histograms;				// Histogram per person
		int m_target_frame;									// Frame where all four persons are visible and seperated

		// Background
		cv::Ptr<cv::BackgroundSubtractorMOG2> m_bg_model;
		cv::Mat m_foreground_image;							// This camera's foreground image (binary)
		cv::Mat m_binary_diff;								// Binary difference of the current frame's foreground image and the previous frame

		cv::VideoCapture m_video;							// Video reader

		cv::Size m_plane_size;								// Camera's FoV size
		long m_frame_amount;								// Amount of frames in this camera's video

		cv::Mat m_camera_matrix;							// Camera matrix (3x3)
		cv::Mat m_distortion_coeffs;						// Distortion vector (5x1)
		cv::Mat m_rotation_values;							// Rotation vector (3x1)
		cv::Mat m_translation_values;						// Translation vector (3x1)

		float m_fx, m_fy, m_cx, m_cy;						// Focal lenghth (fx, fy), camera center (cx, cy)

		cv::Mat m_rt;										// R matrix
		cv::Mat m_inverse_rt;								// R's inverse matrix

		cv::Point3f m_camera_location;						// Camera location in the 3D space
		std::vector<cv::Point3f> m_camera_plane;			// Camera plane of view
		std::vector<cv::Point3f> m_camera_floor;			// Projection of the camera itself onto the ground floor view

		cv::Mat m_frame;									// Current video frame (image)

		int m_cb_width = 0, m_cb_height = 0;
		int m_cb_square_size = 0;

		cv::Mat m_intrinsic, m_dist_coeffs, m_R, m_T;

		bool initCameraProp();
		void loadVideo(std::string path);
		void initColorModels(std::string path);
		bool detIntrinsics(std::string path);
		bool findCbCorners(cv::Mat& frame, std::vector<cv::Point3f>& objPoints, std::vector<cv::Point2f>& imgPoints);
		void initBgModel();
		static void onMouse(int, int, int, int, void*);
		void initCamLoc();
		inline void camPtInWorld();

		cv::Point3f ptToW3D(const cv::Point&);
		cv::Point3f cam3DtoW3D(const cv::Point3f&);

	public:
		VoxelCamera(const std::string&, int);
		virtual ~VoxelCamera();

		bool initialize();

		cv::Mat& advanceVideoFrame();
		cv::Mat& getVideoFrame(int);
		void setVideoFrame(int);

		bool detExtrinsics();
		void createForegroundImage();

		cv::Point projectOnView(const cv::Point3f&, const cv::Mat&, const cv::Mat&, const cv::Mat&, const cv::Mat&);
		cv::Point projectOnView(const cv::Point3f&);

		const int getId() const
		{
			return m_id;
		}

		long getFramesAmount() const
		{
			return m_frame_amount;
		}

		const cv::Size& getSize() const
		{
			return m_plane_size;
		}

		const cv::Mat& getForegroundImage() const
		{
			return m_foreground_image;
		}

		const cv::Mat& getBinaryDifference() const
		{
			return m_binary_diff;
		}

		const cv::Mat& getFrame() const
		{
			return m_frame;
		}

		const std::vector<cv::Point3f>& getCameraFloor() const
		{
			return m_camera_floor;
		}

		const cv::Point3f& getCameraLocation() const
		{
			return m_camera_location;
		}

		const std::vector<cv::Point3f>& getCameraPlane() const
		{
			return m_camera_plane;
		}
	};

} /* namespace team45 */

#endif /* CAMERA_H */
