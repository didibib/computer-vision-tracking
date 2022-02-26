#include "cvpch.h"
#include "scene_renderer.h"
#include "util.h"

namespace team45
{
	/**
	 * Constructor
	 * Scene properties class (mostly called by Window)
	 */
	Scene3DRenderer::Scene3DRenderer(Reconstructor& r, const std::vector<Camera*>& cs) :
		m_reconstructor(r),
		m_cameras(cs),
		m_num(4),
		m_sphere_radius(1850)
	{
		m_width = 640;
		m_height = 480;
		m_quit = false;
		m_paused = false;
		m_rotate = false;
		m_camera_view = true;
		m_show_volume = true;
		m_show_grd_flr = true;
		m_show_cam = true;
		m_show_org = true;
		m_show_arcball = false;
		m_show_info = true;
		m_fullscreen = false;

		// Read the checkerboard properties (XML)
		cv::FileStorage fs;
		fs.open(util::DATA_DIR_STR + util::CB_CONFIG_FILE, cv::FileStorage::READ);
		if (fs.isOpened())
		{
			fs["CheckerBoardWidth"] >> m_board_size.width;
			fs["CheckerBoardHeight"] >> m_board_size.height;
			fs["CheckerBoardSquareSize"] >> m_square_side_len;
		}
		fs.release();

		m_current_camera = 0;
		m_previous_camera = 0;

		m_number_of_frames = m_cameras.front()->getFramesAmount();
		m_current_frame = 0;
		m_previous_frame = -1;

		const int H = 0;
		const int S = 0;
		const int V = 0;
		m_h_threshold = H;
		m_ph_threshold = H;
		m_s_threshold = S;
		m_ps_threshold = S;
		m_v_threshold = V;
		m_pv_threshold = V;

		cv::createTrackbar("Frame", util::VIDEO_WINDOW, &m_current_frame, m_number_of_frames - 2);
		cv::createTrackbar("H", util::VIDEO_WINDOW, &m_h_threshold, 255);
		cv::createTrackbar("S", util::VIDEO_WINDOW, &m_s_threshold, 255);
		cv::createTrackbar("V", util::VIDEO_WINDOW, &m_v_threshold, 255);

		createFloorGrid();
		setTopView();
	}

	/**
	 * Deconstructor
	 * Free the memory of the floor_grid pointer vector
	 */
	Scene3DRenderer::~Scene3DRenderer()
	{
		for (size_t f = 0; f < m_floor_grid.size(); ++f)
			for (size_t g = 0; g < m_floor_grid[f].size(); ++g)
				delete m_floor_grid[f][g];
	}

	/**
	 * Process the current frame on each camera
	 */
	bool Scene3DRenderer::processFrame()
	{
		for (size_t c = 0; c < m_cameras.size(); ++c)
		{
			if (m_current_frame == m_previous_frame + 1)
			{
				m_cameras[c]->advanceVideoFrame();
			}
			else if (m_current_frame != m_previous_frame)
			{
				m_cameras[c]->getVideoFrame(m_current_frame);
			}
			assert(m_cameras[c] != NULL);
			m_cameras[c]->createForegroundImage();
		}
		return true;
	}

	/**
	 * Set currently visible camera to the given camera id
	 */
	void Scene3DRenderer::setCamera(
		int camera)
	{
		m_camera_view = true;

		if (m_current_camera != camera)
		{
			m_previous_camera = m_current_camera;
			m_current_camera = camera;
			m_arcball_eye.x = m_cameras[camera]->getCameraPlane()[0].x;
			m_arcball_eye.y = m_cameras[camera]->getCameraPlane()[0].y;
			m_arcball_eye.z = m_cameras[camera]->getCameraPlane()[0].z;
			m_arcball_up.x = 0.0f;
			m_arcball_up.y = 0.0f;
			m_arcball_up.z = 1.0f;
		}
	}

	/**
	 * Set the 3D scene to bird's eye view
	 */
	void Scene3DRenderer::setTopView()
	{
		m_camera_view = false;
		if (m_current_camera != -1)
			m_previous_camera = m_current_camera;
		m_current_camera = -1;

		m_arcball_eye = vec(0.0f, 0.0f, 10000.0f);
		m_arcball_centre = vec(0.0f, 0.0f, 0.0f);
		m_arcball_up = vec(0.0f, 1.0f, 0.0f);
	}

	/**
	 * Create a LUT for the floor grid
	 */
	void Scene3DRenderer::createFloorGrid()
	{
		const int size = m_reconstructor.getSize() / m_num;
		const int z_offset = 3;

		// edge 1
		std::vector<cv::Point3i*> edge1;
		for (int y = -size * m_num; y <= size * m_num; y += size)
			edge1.push_back(new cv::Point3i(-size * m_num, y, z_offset));

		// edge 2
		std::vector<cv::Point3i*> edge2;
		for (int x = -size * m_num; x <= size * m_num; x += size)
			edge2.push_back(new cv::Point3i(x, size * m_num, z_offset));

		// edge 3
		std::vector<cv::Point3i*> edge3;
		for (int y = -size * m_num; y <= size * m_num; y += size)
			edge3.push_back(new cv::Point3i(size * m_num, y, z_offset));

		// edge 4
		std::vector<cv::Point3i*> edge4;
		for (int x = -size * m_num; x <= size * m_num; x += size)
			edge4.push_back(new cv::Point3i(x, -size * m_num, z_offset));

		m_floor_grid.push_back(edge1);
		m_floor_grid.push_back(edge2);
		m_floor_grid.push_back(edge3);
		m_floor_grid.push_back(edge4);
	}

} /* namespace team45 */
