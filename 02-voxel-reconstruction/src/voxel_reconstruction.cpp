#include "cvpch.h"
#include "voxel_reconstruction.h"

#include "window.h"
#include "reconstructor.h"
#include "scene_renderer.h"
#include "util.h"

namespace team45
{
	/**
	 * Main constructor, initialized all cameras
	 */
	VoxelReconstruction::VoxelReconstruction(const int cva) :
		m_cam_views_amount(cva)
	{
		const std::string cam_path = util::DATA_DIR_STR + "cam";

		for (int v = 0; v < m_cam_views_amount; ++v)
		{
			std::stringstream full_cam_path;
			full_cam_path << cam_path << (v + 1) << PATH_SEP;

			/*
			 * Assert that there is a video file
			 */
			INFO("Video file path: {} {}", full_cam_path.str(), util::VIDEO_FILE);
			assert(util::fexists(full_cam_path.str() + util::VIDEO_FILE));

			/*
			 * Assert that if there's no config.xml file, there's an intrinsics file and
			 * a checkerboard video to create the extrinsics from
			 */
			assert(
				(!util::fexists(full_cam_path.str() + util::CAM_CONFIG_FILE) ?
					util::fexists(full_cam_path.str() + util::INTRINSICS_FILE) &&
					util::fexists(full_cam_path.str() + util::CHECKERBOARD_VIDEO)
					: true)
			);

			m_cam_views.push_back(new Camera(full_cam_path.str(), v));
		}
	}

	/**
	 * Main destructor, cleans up pointer vector memory of the cameras
	 */
	VoxelReconstruction::~VoxelReconstruction()
	{
		for (size_t v = 0; v < m_cam_views.size(); ++v)
			delete m_cam_views[v];
	}

	/**
	 * What you can hit
	 */
	void VoxelReconstruction::showKeys()
	{
		std::cout << "VoxelReconstruction v" << util::VERSION << std::endl << std::endl;
		std::cout << "Use these keys:" << std::endl;
		std::cout << "q       : Quit" << std::endl;
		std::cout << "p       : Pause" << std::endl;
		std::cout << "b       : Frame back" << std::endl;
		std::cout << "n       : Next frame" << std::endl;
		std::cout << "r       : Rotate voxel space" << std::endl;
		std::cout << "s       : Show/hide arcball wire sphere (Linux only)" << std::endl;
		std::cout << "v       : Show/hide voxel space box" << std::endl;
		std::cout << "g       : Show/hide ground plane" << std::endl;
		std::cout << "c       : Show/hide cameras" << std::endl;
		std::cout << "i       : Show/hide camera numbers (Linux only)" << std::endl;
		std::cout << "o       : Show/hide origin" << std::endl;
		std::cout << "t       : Top view" << std::endl;
		std::cout << "1,2,3,4 : Switch camera #" << std::endl << std::endl;
		std::cout << "Zoom with the scrollwheel while on the 3D scene" << std::endl;
		std::cout << "Rotate the 3D scene with left click+drag" << std::endl << std::endl;
	}

	/**
	 * - If the xml-file with camera intrinsics, extrinsics and distortion is missing,
	 *   create it from the checkerboard video and the measured camera intrinsics
	 * - After that initialize the scene rendering classes
	 * - Run it!
	 */
	void VoxelReconstruction::init(int argc, char** argv)
	{
		for (int v = 0; v < m_cam_views_amount; ++v)
		{
			bool has_cam = m_cam_views[v]->initialize();
			assert(has_cam);
		}

		cv::destroyAllWindows();
		cv::namedWindow(util::VIDEO_WINDOW, CV_WINDOW_KEEPRATIO);

		Reconstructor reconstructor(m_cam_views);
		Scene3DRenderer scene3d(reconstructor, m_cam_views);

		Window::GetInstance().init(util::SCENE_WINDOW.c_str(), scene3d);
		Window::GetInstance().run();
	}

} /* namespace team45 */
