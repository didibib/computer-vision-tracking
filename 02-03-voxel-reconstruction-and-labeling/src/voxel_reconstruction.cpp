#include "cvpch.h"
#include "util.h"
#include "voxel_reconstruction.h"
#include "voxel_camera.h"
#include "color_model.h"

using namespace std;
using namespace cv;

namespace team45
{
	/**
	 * Constructor
	 * Voxel reconstruction class
	 */
	VoxelReconstruction::VoxelReconstruction(const vector<VoxelCamera*>& cs, int width, int height, int depth, int step) :
		m_cameras(cs), m_width(width), m_height(height), m_depth(depth), m_step(step)
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
		initVoxels();
		initColorModels();
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
	void VoxelReconstruction::initVoxels()
	{
		// Cube dimensions from [(-m_height, m_height), (-m_height, m_height), (0, m_height)]
		const int xL = -m_width;
		const int xR = m_width;
		const int yL = -m_height;
		const int yR = m_height;
		const int zL = 0;
		const int zR = m_depth;
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
		m_lookup.resize(m_cameras.size());

		// Prepare our flag that determines if a voxel is on in all cameras
		// 00.....01111
		m_all_camera_flags = 0;
		for (int c = 0; c < m_cameras.size(); c++)
			m_all_camera_flags |= (1 << c);

		std::vector<Point3f> cameraPositions;
		for (int c = 0; c < m_cameras.size(); c++)
		{
			auto pos = m_cameras[c]->getCameraLocation();
			cameraPositions.push_back(pos);
		}

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
					voxel->position = glm::ivec3(x, y, z);
					voxel->camera_flags = 0;
					voxel->color = glm::vec3(0);

					const int p = zp * plane + yp * plane_x + xp;  // The voxel's index

					for (size_t c = 0; c < m_cameras.size(); ++c)
					{
						float xdiff = x - cameraPositions[c].x;
						float ydiff = y - cameraPositions[c].y;
						float zdiff = z - cameraPositions[c].z;
						float distance = glm::sqrt(xdiff * xdiff + ydiff * ydiff + zdiff * zdiff);
						voxel->distances.push_back(distance);

						Point point = m_cameras[c]->projectOnView(Point3f((float)x, (float)y, (float)z));
						cv::Size camSize = m_cameras[c]->getSize();

						// Check if point is seen by camera
						if (point.x >= 0 && point.x < camSize.width
							&& point.y >= 0 && point.y < camSize.height)
						{
							voxel->pixelProjections.push_back(point);
						}
						else
						{
							// Don't add it to the lookup table
							voxel->pixelProjections.push_back(cv::Point(-1, -1));
							continue;
						}

#pragma omp critical
						{
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
					}
					//Writing voxel 'p' is not critical as it's unique (thread safe)
					m_voxels[p] = voxel;
				}
			}
		}

		// Sort each vector so that the voxel closest to the pixel is in front
		for (int c = 0; c < m_cameras.size(); c++)
		{
			for (auto it = m_lookup[c].begin(); it != m_lookup[c].end(); it++)
			{
				std::sort(it->second.begin(), it->second.end(),
					[c](Voxel* a, Voxel* b) {
						return a->distances[c] < b->distances[c];
					});
			}
		}

		INFO("Voxels projected per cam {} {} {} {}", camCount[0], camCount[1], camCount[2], camCount[3]);

		initColorModels();
	}

	void VoxelReconstruction::initColorModels()
	{
		INFO("Initializing color models");
		// Initialize the color models for each camera!
		for (int c = 0; c < m_cameras.size(); c++)
		{
			if (!m_cameras[c]->loadColorModels() || true)
			{
				// We need to create a new color model and save it
				int frame = m_cameras[c]->getFrameAllVisible();
				for (int i = 0; i < m_cameras.size(); i++)
				{
					// Set it to the current frame
					// Create a foreground mask
					m_cameras[i]->setVideoFrame(frame);
					m_cameras[i]->createForegroundImage();
				}

				// Update and label the voxels
				updateVoxels();
				labelVoxels();

				// Create the models and save them 
				std::vector<Histogram*> models;
				createColorModels(c, models);
				m_cameras[c]->setColorModels(models);

				// Now we have to make sure that each model refers to the same person!
				// We do this by comparing all different permutations of each color model from a camera with the first camera

				auto baseModels = m_cameras[0]->getColorModels();

				if (c == 0)
				{
					// First camera defines person Ids
					for (int i = 0; i < baseModels.size(); i++)
						baseModels[i]->setId(i);
					m_cameras[c]->saveColorModels(baseModels);
				}
				else
				{
					auto currModels = m_cameras[c]->getColorModels();
					matchModels(baseModels, currModels);
					m_cameras[c]->saveColorModels(currModels);
				}

				// Reload the video from all the cams because of some weird issue with video.set?
				for (int i = 0; i < m_cameras.size(); i++)
					m_cameras[i]->reloadVideo();
			}
		}
	}

	/*
		Changes m2, so that each model (person) in m2 matches the correct model in m1.
		So m1 already knows which model is which person, and m2 uses the correlation between the models to estimate it's own matches
	*/
	void VoxelReconstruction::matchModels(std::vector<Histogram*>& m1, std::vector<Histogram*>& m2)
	{
		// Generate permutations
		std::vector<std::vector<int>> perms = util::permutations(util::K_NR_OF_PERSONS);

		// Scores for each permutation
		std::vector<float> scores;

		// Per permutation compare baseModels to other camera models
		for (int p = 0; p < perms.size(); p++)
		{
			std::vector<int> permutation = perms[p];
			float score = 0;

			for (int i = 0; i < m1.size(); i++)
			{
				score += m1[i]->compare(*m2[permutation[i]]);
			}
			scores.push_back(score);
		}

		// Find the best score, which is the lowest distance
		int index = 0;
		float best = FLT_MAX;
		for (int s = 0; s < scores.size(); s++)
			if (scores[s] < best)
			{
				best = scores[s];
				index = s;
			}

		// Use the index to set the id's
		std::vector<int> bestPermutation = perms[index];
		for (int i = 0; i < bestPermutation.size(); i++)
			m2[i]->setId(bestPermutation[i]);

		// Sort on id
		std::sort(m2.begin(), m2.end(),
			[](Histogram* a, Histogram* b) {
				return a->getId() < b->getId();
			});

		for (int i = 0; i < 4; i++)
		{
			m1[i]->draw();
			m2[i]->draw();
		}
		cv::destroyAllWindows();
	}

	/**
	 * The order of operations matters
	 */
	void VoxelReconstruction::update()
	{
		updateVoxels();
		labelVoxels();
		//matchClusters();
		colorVoxels();
	}

	/**
	 * Count the amount of camera's each voxel in the space appears on,
	 * if that amount equals the amount of cameras, add that voxel to the
	 * visible_voxels vector
	 */
	void VoxelReconstruction::updateVoxels()
	{
		for (int c = 0; c < m_cameras.size(); c++)
		{
			cv::Size camSize = m_cameras[c]->getSize();
			int nrOfPixels = camSize.width * camSize.height;

			int p;
#pragma omp parallel for schedule(static) private(p) shared(m_visible_voxels)
			for (p = 0; p < nrOfPixels; ++p)
			{
				int py = p / camSize.width;
				int px = p % camSize.width;
				Point point = cv::Point2f(px, py);

				// Only continue if this pixel has changed compared to the previous frame
				// This means that the pixel should be on in the binary difference  
				if (m_cameras[c]->getBinaryDifference().at<uchar>(point) < 255) continue;

				// Now we know that this pixel was on in the binary difference
				auto iterator = m_lookup[c].find(p);
				// This pixel does not have voxels mapped to it
				if (iterator == m_lookup[c].end()) continue;

				// Voxels mapped to this pixel, so evaluate them
				for (int v = 0; v < iterator->second.size(); v++)
				{
					Voxel* voxel = iterator->second[v];

					if (voxel->position.z < 700)
						continue;

					// Get the current status of the pixel at the point
					int voxelFlag = m_cameras[c]->getForegroundImage().at<uchar>(point) == 255;
					// Check if the voxel was on in the previous frame
					bool voxelOnPrev = voxel->camera_flags == m_all_camera_flags;

					// Set flag c to voxelOnPrev's value
					// First use a mask to turn off flag c
					voxel->camera_flags &= ~(1 << c);
					// Then make flag c equal to voxelFlag's value
					voxel->camera_flags |= voxelFlag << c;

					bool voxelOnNow = voxel->camera_flags == m_all_camera_flags;

#pragma omp critical // The following operations are critical, since visible_voxels is shared by the threads
					{
						if (voxelOnPrev && !voxelOnNow)
						{
							// Remove the voxel from visible_voxels
							m_visible_voxels[voxel->visibleIndex] = m_visible_voxels[m_visible_voxels.size() - 1];
							m_visible_voxels[voxel->visibleIndex]->visibleIndex = voxel->visibleIndex;

							m_visible_voxels_gpu[voxel->visibleIndex] = m_visible_voxels_gpu[m_visible_voxels_gpu.size() - 1];

							voxel->visibleIndex = -1;

							m_visible_voxels.resize(m_visible_voxels.size() - 1);
							m_visible_voxels_gpu.resize(m_visible_voxels_gpu.size() - 1);
						}
						else if (!voxelOnPrev && voxelOnNow)
						{
							// Add the voxel to visible_voxels
							m_visible_voxels.push_back(voxel);
							m_visible_voxels_gpu.push_back(createVoxelGPU(*voxel));
							voxel->visibleIndex = m_visible_voxels.size() - 1;
						}
					}
				}
			}
		}
	}

	void VoxelReconstruction::labelVoxels()
	{
		std::vector<cv::Point2f> voxel_points;
		// Reserve memory so that we can parallelize the projection to 2d
		voxel_points.resize(m_visible_voxels.size());

		int v;
#pragma omp parallel for schedule(static) private(v) shared(voxel_points, m_visible_voxels)
		for (v = 0; v < m_visible_voxels.size(); v++)
		{
			Voxel* voxel = m_visible_voxels[v];
			// Discard the z-coordinate
			cv::Point2f point(voxel->position.x, voxel->position.y);
			voxel_points[v] = point;
		}

		TermCriteria criteria(cv::TermCriteria::EPS, 0, 0);
		int flags = cv::KMEANS_PP_CENTERS;

		double compactness = cv::kmeans(voxel_points, util::K_NR_OF_PERSONS, m_labels, criteria, util::K_NR_OF_ATTEMPTS, flags, m_cluster_centers);

		INFO("Clustered voxels with compactness: {}", compactness);
	}

	/*
		Match each of the voxel clusters with a color model.
		To do this, we first need to create a model for each view.
		Then compare all the views and use an appropriate scale to compare them.
	*/
	void VoxelReconstruction::matchClusters()
	{
		// A color model, per view, per person
		std::vector<std::vector<Histogram*>> models;
		for (int c = 0; c < m_cameras.size(); c++)
		{
			// Create color model for this camera
			// So we can compare it to the offline models  
			//models.push_back(createColorModels(c));
		}
	}

	void VoxelReconstruction::createColorModels(int cam, std::vector<Histogram*>& histograms)
	{
		cv::Mat frame = m_cameras[cam]->getFrame();

		std::vector<cv::Mat> voxel_images;
		for (int i = 0; i < util::K_NR_OF_PERSONS; i++)
		{
			cv::Mat black = cv::Mat::zeros(cv::Size(frame.cols, frame.rows), frame.type());
			voxel_images.push_back(black);
		}

		int v;
		//#pragma omp parallel for schedule(static) private(v) shared(voxel_bitmap)
		for (v = 0; v < m_visible_voxels.size(); v++)
		{
			Voxel* voxel = m_visible_voxels[v];
			int label = m_labels.at<int>(v);

			cv::Point2f p = voxel->pixelProjections[cam];
			int xOff = 1;
			int yOff = 1;
			for (int y = p.y - yOff; y <= p.y + yOff; y++)
			{
				for (int x = p.x - xOff; x <= p.x + xOff; x++)
				{
					cv::Point2f pOff = cv::Point2f(x, y);
					voxel_images[label].at<Vec3b>(pOff) = frame.at<Vec3b>(pOff);
				}
			}
		}

		for (int i = 0; i < voxel_images.size(); i++)
		{
			Mat hsv;
			Mat src = voxel_images[i];
			cv::cvtColor(src, hsv, COLOR_BGR2HSV);
			Histogram* h = new Histogram();
			h->calculate(hsv);
			histograms.push_back(h);
			//histograms[i].draw();
		}
	}

	void VoxelReconstruction::colorVoxels()
	{
		for (int v = 0; v < m_visible_voxels.size(); v++)
		{
			Voxel* voxel = m_visible_voxels[v];

			/*
			// Find the closest camera
			// Can be changed to either the camera with the smallest angle,
			// Or maybe the camera to which the normal on the voxel (if we can calculate it with it's neighbourhood) points towards?
			int cam;
			float closestDistance = 3.40281e+038;
			for (int c = 0; c < m_cameras.size(); c++)
			{
				if (voxel->distances[c] >= closestDistance)
					continue;
				cam = c;
				closestDistance = voxel->distances[c];
			}

			// For now, color the voxel using the front camera
			colorVoxel(voxel, 1);
			*/

			// Color the voxel based on it's labelling (not yet matched to a person)
			std::vector<glm::vec3> colors
			{
				{1,0,0},	// red
				{0,1,0},	// green
				{0,0,1},	// blue
				{1,0,1}		// purple
			};

			voxel->color = colors[m_labels.at<int>(v)];

			m_visible_voxels_gpu[v].color = voxel->color;
		}
	}

	bool VoxelReconstruction::colorVoxel(Voxel* voxel, int cam)
	{
		// Area around the pixel that we check for occlusions
		int xOffset = 2;
		int yOffset = xOffset;

		cv::Point pixelPoint = voxel->pixelProjections[cam];
		int pixelIndex = pixelPoint.x + pixelPoint.y * m_cameras[cam]->getSize().width;

		Voxel* closestVoxel = voxel;

		// Look in an area around the voxel to see if we can find an occluding voxel
		for (int y = pixelPoint.y - yOffset; y <= pixelPoint.y + yOffset; y++)
		{
			// Early stop once we find a closer voxel that occludes the current one
			if (closestVoxel != voxel)
				break;
			for (int x = pixelPoint.x - xOffset; x <= pixelPoint.x + xOffset; x++)
			{
				// Early stop again once we find a closer voxel that occludes the current one
				if (closestVoxel != voxel)
					break;

				// Iterator over the lookup table from the camera (all pixels)
				auto mapIt = m_lookup[cam].find(pixelIndex);
				if (mapIt == m_lookup[cam].end())
				{
					ERROR("Pixel from voxel projection has no lookup!");
					return false;
				}
				auto voxels = mapIt->second;

				// Iterator over the voxels from a single pixel
				auto vecIt = voxels.begin();
				// Move our iterator to the first voxel that is on in all cameras
				while ((*vecIt)->camera_flags != m_all_camera_flags && vecIt != voxels.end())
					vecIt++;

				// Double check that we found a voxel (not actually necessary if no rounding errors have occured)
				if (vecIt == voxels.end())
				{
					ERROR("Visible voxel was not found in the projected pixel vector!");
					return false;
				}
				if ((*vecIt)->distances[cam] < closestVoxel->distances[cam])
					closestVoxel = *vecIt;
			}
		}

		if (closestVoxel == voxel)
		{
			// We encounted no voxels that occlude the original voxel
			// So we can color this voxel
			Vec3b color = m_cameras[cam]->getFrame().at<Vec3b>(pixelPoint);
			voxel->color = glm::vec3(color.val[2], color.val[1], color.val[0]) / 255.f;
			return true;
		}

		// The voxel is occluded, so color it black
		voxel->color = glm::vec3(0);
		return false;
	}

	VoxelGPU VoxelReconstruction::createVoxelGPU(Voxel const& voxel)
	{
		VoxelGPU vgpu;
		vgpu.color = voxel.color;
		vgpu.position = voxel.position;

		glm::vec3 scale = glm::vec3(m_step);
		glm::mat4 model0 = glm::mat4(1.0f);
		model0 = glm::scale(model0, scale);

		vgpu.model = glm::translate(model0, voxel.position / scale);

		return vgpu;
	}
} /* namespace team45 */