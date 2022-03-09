#include "cvpch.h"
#include "voxel_camera.h"
#include "color_model.h"
#include "util.h"

using namespace std;
using namespace cv;

namespace team45
{
	vector<Point>* VoxelCamera::m_BoardCorners;  // marked checkerboard corners

	VoxelCamera::VoxelCamera(const string& cdp, const int id) :
		m_data_path(cdp),
		m_id(id)
	{
		m_fx = 0;
		m_fy = 0;
		m_cx = 0;
		m_cy = 0;
		m_frame_amount = 0;
	}

	VoxelCamera::~VoxelCamera()
	{
	}

	/**
	 * Initialize this camera
	 */
	bool VoxelCamera::initialize()
	{
		INFO("Initializing camera {}", m_id);
		FileStorage fs;
		// Read the checkerboard properties (XML)
		fs.open(util::DATA_DIR_STR + util::CB_CONFIG_FILE, FileStorage::READ);
		if (fs.isOpened())
		{
			fs["CheckerBoardWidth"] >> m_cb_width;
			fs["CheckerBoardHeight"] >> m_cb_height;
			fs["CheckerBoardSquareSize"] >> m_cb_square_size;
		}
		fs.release();

		initBgModel();
		initCameraProp();		
		loadVideo(m_data_path + util::VIDEO_FILE);
		initCamLoc();
		camPtInWorld();

		return true;
	}

	bool VoxelCamera::initCameraProp()
	{
		FileStorage fs;
		// Read the camera properties (XML)
		fs.open(m_data_path + util::CAM_CONFIG_FILE, FileStorage::READ);

		// If the config doesn't exist, calibrate the camera
		if (!fs.isOpened())
		{
			INFO("Unable to locate: {}{}", m_data_path, util::CAM_CONFIG_FILE);
			if (!(detIntrinsics(m_data_path + util::INTRINSICS_FILE) && detExtrinsics())) return false;
		}

		// Open it again because now we ensured that detExtrinsics has been completed  
		fs.open(m_data_path + util::CAM_CONFIG_FILE, FileStorage::READ);

		if (fs.isOpened())
		{
			Mat cam_mat, dis_coe, rot_val, tra_val;
			fs["CameraMatrix"] >> cam_mat;
			fs["DistortionCoeffs"] >> dis_coe;
			fs["RotationValues"] >> rot_val;
			fs["TranslationValues"] >> tra_val;

			cam_mat.convertTo(m_camera_matrix, CV_32F);
			dis_coe.convertTo(m_distortion_coeffs, CV_32F);
			rot_val.convertTo(m_rotation_values, CV_32F);
			tra_val.convertTo(m_translation_values, CV_32F);

			fs.release();

			/*
				* [ [ fx  0 cx ]
				*   [  0 fy cy ]
				*   [  0  0  0 ] ]
			*/
			m_fx = m_camera_matrix.at<float>(0, 0);
			m_fy = m_camera_matrix.at<float>(1, 1);
			m_cx = m_camera_matrix.at<float>(0, 2);
			m_cy = m_camera_matrix.at<float>(1, 2);
		}
		return true;
	}

	void VoxelCamera::loadVideo(std::string path)
	{
		// Open the video for this camera
		m_video = VideoCapture(path);
		assert(m_video.isOpened());

		// Assess the image size
		m_plane_size.width = (int)m_video.get(CAP_PROP_FRAME_WIDTH);
		m_plane_size.height = (int)m_video.get(CAP_PROP_FRAME_HEIGHT);
		assert(m_plane_size.area() > 0);

		// Get the amount of video frames
		m_video.set(CAP_PROP_POS_AVI_RATIO, 1);  // Go to the end of the video; 1 = 100%
		m_frame_amount = (long)m_video.get(CAP_PROP_POS_FRAMES);
		assert(m_frame_amount > 1);
		m_video.set(CAP_PROP_POS_AVI_RATIO, 0);  // Go back to the start

		m_video.release(); // Re-open the file because m_video.set(CAP_PROP_POS_AVI_RATIO, 1) may screw it up
		m_video = cv::VideoCapture(path);
	}

	void VoxelCamera::initColorModels(std::string path)
	{
		if (util::fexists(path))
		{

		}
	}

	bool VoxelCamera::detIntrinsics(std::string path)
	{
		cv::FileStorage fs;

        if (util::fexists(path))
        {
            INFO("Reading intrinics from {}", path);
            fs.open(path, FileStorage::READ);
            if (fs.isOpened())
            {
                fs["CameraMatrix"] >> m_intrinsic;
                fs["DistortionCoeffs"] >> m_dist_coeffs;
                fs.release();
            }
            return true;
        }

		// Creating vector to store vectors of 3D points for each checkerboard image
		std::vector<std::vector<cv::Point3f>> objPoints;

		// Creating vector to store vectors of 2D points for each checkerboard image
		std::vector<std::vector<cv::Point2f>> imgPoints;

		cv::VideoCapture vc(m_data_path + util::CALIBRATION_VIDEO);
		int totalNrOfFrames = vc.get(cv::CAP_PROP_FRAME_COUNT);
		int skipFrames = totalNrOfFrames / util::CALIB_MAX_NR_FRAMES - 1;

		cv::Mat frame;

		// Get the frame size 
		vc.read(frame);
		cv::Size frameSize(frame.rows, frame.cols);

		// When we dont find corners we try for a couple more frames
		int notFoundCorners = 0;

		while (vc.read(frame))
		{
			INFO("Current frame {}", vc.get(cv::CAP_PROP_POS_FRAMES));
			// Find chessboard corners
			std::vector<cv::Point3f> objp;
			std::vector<cv::Point2f> imgp;

			// Increase contrast
			frame *= 1.20f;

			bool success = findCbCorners(frame, objp, imgp);
			if (success)
			{
				objPoints.push_back(objp);
				imgPoints.push_back(imgp);
			}
			if (success || notFoundCorners++ > util::CALIB_LOCAL_FRAMES)
			{
				// Skip an amount of frames
				for (int i = 0; i < skipFrames; i++)
					vc.grab();
				notFoundCorners = 0;
				continue;
			}

			// Skip an amount of frames
			for (int i = 0; i < 9; i++)
				vc.grab();
		}
		INFO("Detected a total of {} images with checkerboard", objPoints.size());

		double finalError = cv::calibrateCamera(objPoints, imgPoints, frameSize, m_intrinsic, m_dist_coeffs, m_R, m_T);
		INFO("Calibrated camera with error: {}", finalError);

		fs.open(path, FileStorage::WRITE);
		if (fs.isOpened())
		{
			fs << "CameraMatrix" << m_intrinsic;
			fs << "DistortionCoeffs" << m_dist_coeffs;
			fs.release();
		}

		return true;
	}

	bool VoxelCamera::findCbCorners(cv::Mat& frame, std::vector<cv::Point3f>& objPoints, std::vector<cv::Point2f>& imgPoints)
	{
		INFO("Finding chessboard corners");
		cv::Mat gray;
		cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

		// Finding checker board corners
		// If desired number of corners are found in the image then success = true  
		bool success = cv::findChessboardCorners(gray, cv::Size(m_cb_width, m_cb_height), imgPoints, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);

		/*
			* If desired number of corner are detected,
			* we refine the pixel coordinates and display
			* them on the images of checker board
		*/
		if (success)
		{
			INFO("Found!");
			cv::TermCriteria criteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.001);

			// refining pixel coordinates for given 2d points.
			cv::cornerSubPix(gray, imgPoints, cv::Size(7, 7), cv::Size(-1, -1), criteria);

			// Defining the world coordinates for 3D points
			// We multiply our grid with the real size of the checkerboard
			for (int i{ 0 }; i < m_cb_height; i++)
			{
				for (int j{ 0 }; j < m_cb_width; j++)
					objPoints.push_back(cv::Point3f(j * m_cb_square_size, i * m_cb_square_size, 0));
			}

			cv:drawChessboardCorners(frame, cv::Size(m_cb_width, m_cb_height), imgPoints, true);
		}

		/*cv::imshow("CB Corner Display", frame);
		cv::waitKey(0);
		cv::destroyWindow("CB Corner Display");*/

		return success;
	}

	/*
		Initializes the background model.
		Cannot be saved to a file (https://stackoverflow.com/questions/27370222/save-opencv-backgroundsubtractormog-to-file)
	*/
	void VoxelCamera::initBgModel()
	{
		INFO("Initialize background model");
		m_bg_model = cv::createBackgroundSubtractorMOG2();
		std::string bg_video_path = m_data_path + util::BACKGROUND_VIDEO_FILE;

		assert(util::fexists(bg_video_path));
		cv::VideoCapture vc(bg_video_path);

		cv::Mat frame;
		cv::Mat tempMask;
		while (vc.read(frame))
		{
			// Learning rate of -1, so it automatically adapts
			m_bg_model->apply(frame, tempMask, -1);
		}
	}

	/**
	 * Set and return the next frame from the video
	 */
	Mat& VoxelCamera::advanceVideoFrame()
	{
		m_video >> m_frame;
		assert(!m_frame.empty());
		return m_frame;
	}

	/**
	 * Set the video location to the given frame number
	 */
	void VoxelCamera::setVideoFrame(int frame_number)
	{
		m_video.set(CAP_PROP_POS_FRAMES, frame_number);
	}

	/**
	 * Set and return frame of the video location at the given frame number
	 */
	Mat& VoxelCamera::getVideoFrame(
		int frame_number)
	{
		setVideoFrame(frame_number);
		return advanceVideoFrame();
	}

	/**
	 * Handle mouse events
	 */
	void VoxelCamera::onMouse(int event, int x, int y, int flags, void* param)
	{
		switch (event)
		{
		case EVENT_LBUTTONDOWN:
			if (flags == (EVENT_FLAG_LBUTTON + EVENT_FLAG_CTRLKEY))
			{
				if (!m_BoardCorners->empty())
				{
					INFO("Removed corner {}... (use Click to add)", m_BoardCorners->size());
					m_BoardCorners->pop_back();
				}
			}
			else
			{
				m_BoardCorners->push_back(Point(x, y));
				INFO("Added corner {}... (use CTRL+Click to remove)", m_BoardCorners->size());
			}
			break;
		default:
			break;
		}
	}

	/**
	 * - Determine the camera's extrinsics based on a checkerboard image and the camera intrinsics
	 * - Allows for hand pointing the checkerboard corners
	 */
	 // const string &data_path, const string &cam_data_path, const string &checker_vid_fname, const string &intr_filename, const string &out_fname
	bool VoxelCamera::detExtrinsics()
	{
		const Size board_size(m_cb_width, m_cb_height);
		const int side_len = m_cb_square_size;  // Actual size of the checkerboard square in millimeters

		Mat camera_matrix, distortion_coeffs;
		cv::FileStorage fs;
		fs.open(m_data_path + util::INTRINSICS_FILE, FileStorage::READ);
		if (fs.isOpened())
		{
			Mat camera_matrix_f, distortion_coeffs_f;
			fs["CameraMatrix"] >> camera_matrix_f;
			fs["DistortionCoeffs"] >> distortion_coeffs_f;

			camera_matrix_f.convertTo(camera_matrix, CV_32F);
			distortion_coeffs_f.convertTo(distortion_coeffs, CV_32F);
			fs.release();
		}
		else
		{
			ERROR("Unable to read camera intrinsics from: ", m_data_path, util::INTRINSICS_FILE);
			return false;
		}

		VideoCapture cap(m_data_path + util::CHECKERBOARD_VIDEO);
		if (!cap.isOpened())
		{
			WARN("Unable to open: ", m_data_path, util::CHECKERBOARD_VIDEO);
			if (util::fexists(m_data_path + util::CAM_CONFIG_FILE))
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		// Read first frame
		Mat frame;
		while (frame.empty())
			cap >> frame;
		assert(!frame.empty());

		m_BoardCorners = new vector<Point>(); // A pointer because we need access to it from static function onMouse

		string corners_file = m_data_path + util::CHECKERBOARD_CORNERS;
		if (util::fexists(corners_file))
		{
			FileStorage fs;
			fs.open(corners_file, FileStorage::READ);
			if (fs.isOpened())
			{
				int corners_amount;
				fs["CornersAmount"] >> corners_amount;

				for (int b = 0; b < corners_amount; ++b)
				{
					stringstream corner_id;
					corner_id << "Corner_" << b;

					vector<int> corner;
					fs[corner_id.str()] >> corner;
					assert(corner.size() == 2);
					m_BoardCorners->push_back(Point(corner[0], corner[1]));
				}

				assert((int)m_BoardCorners->size() == board_size.area());

				fs.release();
			}
		}
		else
		{
			INFO("Estimate camera intrinsics by hand...");
			namedWindow(MAIN_WINDOW, CV_WINDOW_KEEPRATIO);
			setMouseCallback(MAIN_WINDOW, onMouse);

			INFO("Now click the {} interior corners of the checkerboard", board_size.area());
			Mat canvas;
			while ((int)m_BoardCorners->size() < board_size.area())
			{
				canvas = frame.clone();

				if (!m_BoardCorners->empty())
				{
					for (size_t c = 0; c < m_BoardCorners->size(); c++)
					{
						circle(canvas, m_BoardCorners->at(c), 4, util::COLOR_MAGENTA, 1, 8);
						if (c > 0)
							line(canvas, m_BoardCorners->at(c), m_BoardCorners->at(c - 1), util::COLOR_MAGENTA, 1, 8);
					}
				}

				int key = waitKey(10);
				if (key == 'q' || key == 'Q')
				{
					return false;
				}
				else if (key == 'c' || key == 'C')
				{
					m_BoardCorners->pop_back();
				}

				imshow(MAIN_WINDOW, canvas);
			}

			assert((int)m_BoardCorners->size() == board_size.area());
			INFO("Marking finished!");
			destroyAllWindows();

			FileStorage fs;
			fs.open(corners_file, FileStorage::WRITE);
			if (fs.isOpened())
			{
				fs << "CornersAmount" << (int)m_BoardCorners->size();
				for (size_t b = 0; b < m_BoardCorners->size(); ++b)
				{
					stringstream corner_id;
					corner_id << "Corner_" << b;
					fs << corner_id.str() << m_BoardCorners->at(b);
				}
				fs.release();
			}
		}

		vector<Point3f> object_points;
		vector<Point2f> image_points;

		// save the object points and image points
		for (int s = 0; s < board_size.area(); ++s)
		{
			float x = float(s / board_size.width * side_len);
			float y = float(s % board_size.width * side_len);
			float z = 0;

			object_points.push_back(Point3f(x, y, z));
			image_points.push_back(m_BoardCorners->at(s));
		}

		delete m_BoardCorners;

		Mat rotation_values_d, translation_values_d;
		solvePnP(object_points, image_points, camera_matrix, distortion_coeffs, rotation_values_d, translation_values_d);

		Mat rotation_values, translation_values;
		rotation_values_d.convertTo(rotation_values, CV_32F);
		translation_values_d.convertTo(translation_values, CV_32F);

		// Draw the origin
		Mat canvas = frame.clone();

		const float x_len = float(side_len * (board_size.height - 1));
		const float y_len = float(side_len * (board_size.width - 1));
		const float z_len = float(side_len * 3);
		Point o = projectOnView(Point3f(0, 0, 0), rotation_values, translation_values, camera_matrix, distortion_coeffs);
		Point x = projectOnView(Point3f(x_len, 0, 0), rotation_values, translation_values, camera_matrix, distortion_coeffs);
		Point y = projectOnView(Point3f(0, y_len, 0), rotation_values, translation_values, camera_matrix, distortion_coeffs);
		Point z = projectOnView(Point3f(0, 0, z_len), rotation_values, translation_values, camera_matrix, distortion_coeffs);

		line(canvas, o, x, util::COLOR_BLUE, 2, CV_AA);
		line(canvas, o, y, util::COLOR_GREEN, 2, CV_AA);
		line(canvas, o, z, util::COLOR_RED, 2, CV_AA);
		circle(canvas, o, 3, util::COLOR_YELLOW, -1, CV_AA);

		fs.open(m_data_path + util::CAM_CONFIG_FILE, FileStorage::WRITE);
		if (fs.isOpened())
		{
			fs << "CameraMatrix" << camera_matrix;
			fs << "DistortionCoeffs" << distortion_coeffs;
			fs << "RotationValues" << rotation_values;
			fs << "TranslationValues" << translation_values;
			fs.release();
		}
		else
		{
			ERROR("Unable to write camera intrinsics+extrinsics to: ", m_data_path, util::CAM_CONFIG_FILE);
			return false;
		}

		// Show the origin on the checkerboard
		namedWindow("Origin", CV_WINDOW_KEEPRATIO);
		imshow("Origin", canvas);
		waitKey(1000);

		return true;
	}

	/**
	 * Calculate the camera's location in the world
	 */
	void VoxelCamera::initCamLoc()
	{
		Mat r;
		Rodrigues(m_rotation_values, r);

		/*
		 * [ [ r11 r12 r13   0 ]
		 *   [ r21 r22 r23   0 ]
		 *   [ r31 r32 r33   0 ]
		 *   [   0   0   0 1.0 ] ]
		 */
		Mat rotation = Mat::zeros(4, 4, CV_32F);
		rotation.at<float>(3, 3) = 1.0;
		Mat r_sub = rotation(Rect(0, 0, 3, 3));
		r.copyTo(r_sub);

		/*
		 * [ [ 1.0   0   0   0 ]
		 *   [   0 1.0   0   0 ]
		 *   [   0   0 1.0   0 ]
		 *   [  tx  ty  tz 1.0 ] ]
		 */
		Mat translation = Mat::eye(4, 4, CV_32F);
		translation.at<float>(3, 0) = -m_translation_values.at<float>(0, 0);
		translation.at<float>(3, 1) = -m_translation_values.at<float>(1, 0);
		translation.at<float>(3, 2) = -m_translation_values.at<float>(2, 0);

		Mat camera_mat = translation * rotation;
		m_camera_location = Point3f(
			camera_mat.at<float>(0, 0) + camera_mat.at<float>(3, 0),
			camera_mat.at<float>(1, 1) + camera_mat.at<float>(3, 1),
			camera_mat.at<float>(2, 2) + camera_mat.at<float>(3, 2));

		cout << "Camera " << m_id + 1 << " " << m_camera_location << endl;

		m_rt = rotation;

		/*
		 * [ [ r11 r12 r13 tx ]
		 *   [ r21 r22 r23 ty ]
		 *   [ r31 r32 r33 tz ]
		 *   [   0   0   0  0 ] ]
		 */
		Mat t_sub = m_rt(Rect(3, 0, 1, 3));
		m_translation_values.copyTo(t_sub);

		invert(m_rt, m_inverse_rt);
	}

	/**
	 * Calculate the camera's plane and fov in the 3D scene
	 */
	void VoxelCamera::camPtInWorld()
	{
		m_camera_plane.clear();
		m_camera_plane.push_back(m_camera_location);

		// clockwise four image plane corners
		// 1 image plane's left upper corner
		Point3f p1 = cam3DtoW3D(Point3f(-m_cx, -m_cy, (m_fx + m_fy) / 2));
		m_camera_plane.push_back(p1);
		// 2 image plane's right upper conner
		Point3f p2 = cam3DtoW3D(Point3f(m_plane_size.width - m_cx, -m_cy, (m_fx + m_fy) / 2));
		m_camera_plane.push_back(p2);
		// 3 image plane's right bottom conner
		Point3f p3 = cam3DtoW3D(Point3f(m_plane_size.width - m_cx, m_plane_size.height - m_cy, (m_fx + m_fy) / 2));
		m_camera_plane.push_back(p3);
		// 4 image plane's left bottom conner
		Point3f p4 = cam3DtoW3D(Point3f(-m_cx, m_plane_size.height - m_cy, (m_fx + m_fy) / 2));
		m_camera_plane.push_back(p4);

		// principal point on the image plane
		Point3f p5 = cam3DtoW3D(Point3f(m_cx, m_cy, (m_fx + m_fy) / 2));
		m_camera_plane.push_back(p5);
	}

	/**
	 * Convert a point on the camera image to a point in the world
	 */
	Point3f VoxelCamera::ptToW3D(
		const Point& point)
	{
		return cam3DtoW3D(Point3f(float(point.x - m_cx), float(point.y - m_cy), (m_fx + m_fy) / 2));
	}

	/**
	 * Convert a point on the camera to a point in the world
	 */
	Point3f VoxelCamera::cam3DtoW3D(
		const Point3f& cam_point)
	{
		Mat Xc(4, 1, CV_32F);
		Xc.at<float>(0, 0) = cam_point.x;
		Xc.at<float>(1, 0) = cam_point.y;
		Xc.at<float>(2, 0) = cam_point.z;
		Xc.at<float>(3, 0) = 1;

		Mat Xw = m_inverse_rt * Xc;

		return Point3f(Xw.at<float>(0, 0), Xw.at<float>(1, 0), Xw.at<float>(2, 0));
	}

	/**
	 * Projects points from the scene space to the image coordinates
	 */
	cv::Point VoxelCamera::projectOnView(
		const cv::Point3f& coords, 
		const cv::Mat& rotation_values, 
		const cv::Mat& translation_values, 
		const cv::Mat& camera_matrix,
		const cv::Mat& distortion_coeffs)
	{
		vector<Point3f> object_points;
		object_points.push_back(coords);

		vector<Point2f> image_points;
		cv::projectPoints(object_points, rotation_values, translation_values, camera_matrix, distortion_coeffs, image_points);

		return image_points.front();
	}

	/**
	 * Non-static for backwards compatibility
	 */
	Point VoxelCamera::projectOnView(const Point3f& coords)
	{
		return projectOnView(coords, m_rotation_values, m_translation_values, m_camera_matrix, m_distortion_coeffs);
	}

	void VoxelCamera::createForegroundImage()
	{
		cv::Mat blurred, tmp, tmpMask, foreground_mask;
		//cv::GaussianBlur(getFrame(), blurred, Size(3, 3), 1, 1);
		m_bg_model->apply(getFrame(), tmpMask, 0);
		cv::threshold(tmpMask, tmpMask, 200, 255, cv::THRESH_BINARY);

		// 3x3 morphological kernel, representing a cross 
		cv::Mat kernel = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(5,5));

		cv::erode(tmpMask, tmp, kernel);
		cv::dilate(tmp, foreground_mask, kernel);

		// Determine binary difference between current frames binary mask and previous frame binary mask
		if (m_foreground_image.rows != 0)
		{
			cv::bitwise_xor(foreground_mask, m_foreground_image, m_binary_diff);
		}
		else
		{
			m_binary_diff = foreground_mask;
		}
		cv::Mat tmp2;
		cv::bitwise_not(m_binary_diff, tmp2);
		cv::bitwise_or(m_binary_diff, tmp2, m_foreground_image);
		m_foreground_image = foreground_mask;
	}

} /* namespace team45 */
