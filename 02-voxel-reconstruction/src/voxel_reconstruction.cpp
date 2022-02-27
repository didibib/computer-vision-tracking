#include "cvpch.h"
#include "voxel_reconstruction.h"
#include "voxel_camera.h"
#include "util.h"

using namespace std;
using namespace cv;

namespace team45
{
	/**
	 * Constructor
	 * Voxel reconstruction class
	 */
	VoxelReconstruction::VoxelReconstruction(
		const vector<VoxelCamera*>& cs) :
		m_cameras(cs),
		m_height(2048),
		m_step(64)
	{
		for (size_t c = 0; c < m_cameras.size(); ++c)
		{
			if (m_plane_size.area() > 0)
				assert(m_plane_size.width == m_cameras[c]->getSize().width && m_plane_size.height == m_cameras[c]->getSize().height);
			else
				m_plane_size = m_cameras[c]->getSize();
		}

		const size_t edge = 2 * m_height;
		m_voxels_amount = (edge / m_step) * (edge / m_step) * (m_height / m_step);

		m_toggle_camera.resize(cs.size());
		initialize();
	}

	/**
	 * Deconstructor
	 * Free the memory of the pointer vectors
	 */
	VoxelReconstruction::~VoxelReconstruction()
	{
		for (size_t c = 0; c < m_corners.size(); ++c)
			delete m_corners.at(c);
		for (size_t v = 0; v < m_voxels.size(); ++v)
			delete m_voxels.at(v);
	}

	/**
	 * Create some Look Up Tables
	 * 	- LUT for the scene's box corners
	 * 	- LUT with a map of the entire voxelspace: point-on-cam to voxels
	 * 	- LUT with a map of the entire voxelspace: voxel to cam points-on-cam
	 */
	void VoxelReconstruction::initialize()
	{
		// Cube dimensions from [(-m_height, m_height), (-m_height, m_height), (0, m_height)]
		const int xL = -m_height;
		const int xR = m_height;
		const int yL = -m_height;
		const int yR = m_height;
		const int zL = 0;
		const int zR = m_height;
		const int plane_y = (yR - yL) / m_step;
		const int plane_x = (xR - xL) / m_step;
		const int plane = plane_y * plane_x;

		// Save the 8 volume corners
		// bottom
		m_corners.push_back(new Point3f((float)xL, (float)yL, (float)zL));
		m_corners.push_back(new Point3f((float)xL, (float)yR, (float)zL));
		m_corners.push_back(new Point3f((float)xR, (float)yR, (float)zL));
		m_corners.push_back(new Point3f((float)xR, (float)yL, (float)zL));

		// top
		m_corners.push_back(new Point3f((float)xL, (float)yL, (float)zR));
		m_corners.push_back(new Point3f((float)xL, (float)yR, (float)zR));
		m_corners.push_back(new Point3f((float)xR, (float)yR, (float)zR));
		m_corners.push_back(new Point3f((float)xR, (float)yL, (float)zR));

		// Acquire some memory for efficiency
		cout << "Initializing " << m_voxels_amount << " voxels ";
		m_voxels.resize(m_voxels_amount);

		// Initialize lookup table
		// Could be with done with resize?
		m_lookup.resize(m_cameras.size());
		/*for (int i = 0; i < m_cameras.size(); i++)
		{
			std::map<int, std::vector<Voxel*>> map;
			m_lookup.push_back(map);
		}*/

		int z;
		int pdone = 0;
		std::vector<int> camCount{ 0,0,0,0 };
#pragma omp parallel for schedule(static) private(z) shared(pdone, camCount)
		for (z = zL; z < zR; z += m_step)
		{
			const int zp = (z - zL) / m_step;
			int done = cvRound((zp * plane / (double)m_voxels_amount) * 100.0);

#pragma omp critical
			if (done > pdone)
			{
				pdone = done;
				cout << done << "%..." << flush;
			}

			int y, x;
			for (y = yL; y < yR; y += m_step)
			{
				const int yp = (y - yL) / m_step;

				for (x = xL; x < xR; x += m_step)
				{
					const int xp = (x - xL) / m_step;

					// Create all voxels
					Voxel* voxel = new Voxel;
					voxel->visibleIndex = -1;
					voxel->x = x;
					voxel->y = y;
					voxel->z = z;
					voxel->camera_flags = 0;

					const int p = zp * plane + yp * plane_x + xp;  // The voxel's index

					for (size_t c = 0; c < m_cameras.size(); ++c)
					{
						Point point = m_cameras[c]->projectOnView(Point3f((float)x, (float)y, (float)z));

						// Calculate pixel index in the lookup table  
						int absPos = point.y * m_cameras[c]->getSize().width + point.x;
						auto iterator = m_lookup[c].find(absPos);
						if (iterator != m_lookup[c].end())
						{
							// Pair exists, so add it
							iterator->second.push_back(voxel);
						}
						else
						{
							// Pair didn't exist yet, so we insert a vector containing the current voxel at this pixel's location
							m_lookup[c].insert({ absPos, { voxel } });
						}
					}
					//Writing voxel 'p' is not critical as it's unique (thread safe)
					m_voxels[p] = voxel;
				}
			}
		}
		INFO("Voxels projected per cam {} {} {} {}", camCount[0], camCount[1], camCount[2], camCount[3]);
		cout << "done!" << endl;
	}

	/**
	 * Count the amount of camera's each voxel in the space appears on,
	 * if that amount equals the amount of cameras, add that voxel to the
	 * visible_voxels vector
	 */
	void VoxelReconstruction::update()
	{
		// Prepare our flag that determines if a voxel is on in all cameras
		// 00.....01111
		int flag = 0;
		for (int c = 0; c < m_cameras.size(); c++)
		{
			//if(m_toggle_camera[c])
				flag |= (1 << c);
		}

		for (int c = 0; c < m_cameras.size(); c++)
		{
			/*if (!m_toggle_camera[c])
				continue;*/

			cv::Size camSize = m_cameras[c]->getSize();
			int nrOfPixels = camSize.width * camSize.height;

			int p;
#pragma omp parallel for schedule(static) private(p) shared(m_visible_voxels)
			for (p = 0; p < nrOfPixels; ++p)
			{
				int py = p / camSize.width;
				int px = p % camSize.width;
				Point point = cv::Point2f(px, py);

				// chance foregroundimage to binary diff
				if (m_cameras[c]->getBinaryDifference().at<uchar>(point) < 255)
					continue;

				// Now we know that this pixel was on in the binary difference
				auto iterator = m_lookup[c].find(p);
				if (iterator != m_lookup[c].end())
				{
					// Voxels mapped to this pixel, so evaluate them
					for (int v = 0; v < iterator->second.size(); v++)
					{
						Voxel* voxel = iterator->second[v];
						int voxelFlag = m_cameras[c]->getForegroundImage().at<uchar>(point) == 255;
						// Check if the voxel is currently on
						bool voxelOnPrev = voxel->camera_flags == flag;

						// Set flag c to voxelOnPrev's value
						// First use a mask to turn off flag c
						voxel->camera_flags &= ~(1 << c);
						// Then make flag c equal to voxelFlag's value
						voxel->camera_flags |= voxelFlag << c;

						bool voxelOnNow = voxel->camera_flags == flag;

#pragma omp critical // The following operations are critical, since visible_voxels is shared by the threads
						{
							if (voxelOnPrev && !voxelOnNow)
							{
								// Remove the voxel from visible_voxels
								m_visible_voxels[voxel->visibleIndex] = m_visible_voxels[m_visible_voxels.size() - 1];
								m_visible_voxels[voxel->visibleIndex]->visibleIndex = voxel->visibleIndex;
								voxel->visibleIndex = -1;
								m_visible_voxels.resize(m_visible_voxels.size() - 1);
							}
							else if (!voxelOnPrev && voxelOnNow)
							{
								// Add the voxel to visible_voxels
								m_visible_voxels.push_back(voxel);
								voxel->visibleIndex = m_visible_voxels.size() - 1;
							}
						}
					}
				}
				else
				{
					// This pixel does not have voxels mapped to it
					continue;
				}
			}
		}
	}

} /* namespace team45 */