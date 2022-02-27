#include "cvpch.h"
#include "scene_renderer.h"
#include "util.h"

namespace team45
{
	/**
	 * Constructor
	 * Scene properties class (mostly called by Window)
	 */
	Scene3DRenderer::Scene3DRenderer(VoxelReconstruction& r, const std::vector<VoxelCamera*>& cs) :
		m_reconstructor(r),
		m_cameras(cs),
		m_num(4),
		m_sphere_radius(1850)
	{
		m_width = 640;
		m_height = 480;
		m_camera_view = true;

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

		cv::createTrackbar("Frame", util::VIDEO_WINDOW, &m_current_frame, m_number_of_frames - 2);

		createFloorGrid();
	}

	/**
	 * Deconstructor
	 * Free the memory of the floor_grid pointer vector
	 */
	Scene3DRenderer::~Scene3DRenderer()
	{

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
	void Scene3DRenderer::toggleCamera(int camera_id)
	{
		m_reconstructor.toggleCamera(camera_id);
	}

	/**
	 * Create a LUT for the floor grid
	 */
	std::vector<Vertex> Scene3DRenderer::createFloorGrid()
	{
		// edge points of the virtual ground floor grid
		std::vector<std::vector<glm::vec3*> > floor_grid;
		const int size = m_reconstructor.getSize() / m_num;
		const int z_offset = 3;

		// edge 1
		std::vector<glm::vec3*> edge1;
		for (int y = -size * m_num; y <= size * m_num; y += size)
			edge1.push_back(new glm::vec3(-size * m_num, y, z_offset));

		// edge 2
		std::vector<glm::vec3*> edge2;
		for (int x = -size * m_num; x <= size * m_num; x += size)
			edge2.push_back(new glm::vec3(x, size * m_num, z_offset));

		// edge 3
		std::vector<glm::vec3*> edge3;
		for (int y = -size * m_num; y <= size * m_num; y += size)
			edge3.push_back(new glm::vec3(size * m_num, y, z_offset));

		// edge 4
		std::vector<glm::vec3*> edge4;
		for (int x = -size * m_num; x <= size * m_num; x += size)
			edge4.push_back(new glm::vec3(x, -size * m_num, z_offset));

		floor_grid.push_back(edge1);
		floor_grid.push_back(edge2);
		floor_grid.push_back(edge3);
		floor_grid.push_back(edge4);

		std::vector<Vertex> vertices;
		glm::vec4 color(0.9f, 0.9f, 0.9f, 0.5f);

		int gSize =m_num * 2 + 1;
		for (int g = 0; g < gSize; g++)
		{
			// y lines
			Vertex v1;
			v1.Position = *floor_grid[0][g];
			v1.Color = color;

			Vertex v2;
			v2.Position = *floor_grid[2][g];
			v2.Color = color;

			// x lines
			Vertex v3;
			v3.Position = *floor_grid[1][g];
			v3.Color = color;

			Vertex v4;
			v4.Position = *floor_grid[3][g];
			v4.Color = color;

			vertices.push_back(v1);
			vertices.push_back(v2);
			vertices.push_back(v3);
			vertices.push_back(v4);
		}
		return vertices;
	}

} /* namespace team45 */
