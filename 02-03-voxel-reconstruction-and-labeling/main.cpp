#include "cvpch.h"
#include "util.h"
#include "voxel_camera.h"
#include "window.h"
#include "voxel_reconstruction.h"
#include "scene_renderer.h"

using namespace team45;

const int m_cam_views_amount = 4;
const int m_voxel_height = 2048 + 1024;
const int m_voxel_step = 64;

static std::vector<VoxelCamera*> m_cam_views;

const std::string project = "4persons/";
const std::string cam_path = util::DATA_DIR_STR + project + "cam";

void showKeys()
{
	std::cout << "VoxelReconstruction v" << util::VERSION		<< std::endl << std::endl;
	std::cout << "Use these keys:"								<< std::endl;
	std::cout << "	w,a,s,d     : Move camera in 2d"			<< std::endl;
	std::cout << "	space, ctrl : Move camera up and down"		<< std::endl;
	std::cout << "	scroll      : Zoom"							<< std::endl;
	std::cout << "	r           : Rotate camera around scene"	<< std::endl;
	std::cout << "	1,2,3,4     : Toggle voxel camera #"		<< std::endl;
	std::cout << "	p           : Pause"						<< std::endl;
	std::cout << "	b           : Frame back"					<< std::endl;
	std::cout << "	n           : Next frame"					<< std::endl << std::endl;
}

void getCameraData()
{
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
			(!util::fexists(full_cam_path.str() + util::CAM_CONFIG) ?
				util::fexists(full_cam_path.str() + util::INTRINSICS_FILE) &&
				util::fexists(full_cam_path.str() + util::CHECKERBOARD_VIDEO)
				: true)
		);

		m_cam_views.push_back(new VoxelCamera(full_cam_path.str(), v));
	}
}

void initCameras()
{
	for (int v = 0; v < m_cam_views_amount; ++v)
	{
		bool has_cam = m_cam_views[v]->initialize();
		assert(has_cam);
	}

	cv::destroyAllWindows();
	cv::namedWindow(util::VIDEO_WINDOW, CV_WINDOW_KEEPRATIO);
}

int main(int argc, char** argv)
{
	log::init();
	showKeys();
	getCameraData();
	initCameras();


	VoxelReconstruction reconstructor(m_cam_views, m_voxel_height, m_voxel_step);
	Scene3DRenderer scene3d(reconstructor, m_cam_views);

	Window::GetInstance().init(util::SCENE_WINDOW.c_str(), scene3d);
	Window::GetInstance().run();


	log::shutdown();
	for (size_t v = 0; v < m_cam_views.size(); ++v)
		delete m_cam_views[v];
	return EXIT_SUCCESS;
}