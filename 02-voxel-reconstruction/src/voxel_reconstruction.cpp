/*
 * VoxelReconstruction.cpp
 *
 *  Created on: Nov 13, 2013
 *      Author: coert
 */

#include "cvpch.h"
#include "voxel_reconstruction.h"

#include "window.h"
#include "reconstructor.h"
#include "scene_renderer.h"
#include "util.h"

namespace nl_uu_science_gmt
{

	/**
	 * Main constructor, initialized all cameras
	 */
	VoxelReconstruction::VoxelReconstruction(const std::string& dp, const int cva) :
		m_data_path(dp), m_cam_views_amount(cva)
	{
		const std::string cam_path = m_data_path + "cam";

		for (int v = 0; v < m_cam_views_amount; ++v)
		{
			std::stringstream full_path;
			full_path << cam_path << (v + 1) << PATH_SEP;

			/*
			 * Assert that there's a background image or video file and \
			 * that there's a video file
			 */
			std::cout << full_path.str() << Util::VideoFile << std::endl;
			std::cout << full_path.str() << Util::BackgroundImageFile << std::endl;
			assert(
				Util::fexists(full_path.str() + Util::BackgroundImageFile)
				&&
				Util::fexists(full_path.str() + Util::VideoFile)
			);

			/*
			 * Assert that if there's no config.xml file, there's an intrinsics file and
			 * a checkerboard video to create the extrinsics from
			 */
			assert(
				(!Util::fexists(full_path.str() + Util::ConfigFile) ?
					Util::fexists(full_path.str() + Util::IntrinsicsFile) &&
					Util::fexists(full_path.str() + Util::CheckerboadVideo)
					: true)
			);

			m_cam_views.push_back(new Camera(full_path.str(), Util::ConfigFile, v));
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
		std::cout << "VoxelReconstruction v" << VERSION << std::endl << std::endl;
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
	void VoxelReconstruction::run(int argc, char** argv)
	{
		for (int v = 0; v < m_cam_views_amount; ++v)
		{
			bool has_cam = Camera::detExtrinsics(m_cam_views[v]->getDataPath(), Util::CheckerboadVideo,
				Util::IntrinsicsFile, m_cam_views[v]->getCamPropertiesFile());
			assert(has_cam);
			if (has_cam) has_cam = m_cam_views[v]->initialize();
			assert(has_cam);
		}

		cv::destroyAllWindows();
		cv::namedWindow(VIDEO_WINDOW, CV_WINDOW_KEEPRATIO);

		Reconstructor reconstructor(m_cam_views);
		Scene3DRenderer scene3d(reconstructor, m_cam_views);
		Window window(scene3d);

		window.init(SCENE_WINDOW.c_str());
		window.run();
	}

} /* namespace nl_uu_science_gmt */
