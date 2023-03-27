#pragma once
#ifndef VOXELRECONSTRUCTION_H
#define VOXELRECONSTRUCTION_H

namespace team45
{
	class VoxelCamera;
	class Histogram;

	class VoxelReconstruction
	{
		const std::vector<VoxelCamera*>& m_cameras;			// vector of pointers to cameras
		const int m_height;									// Cube half-space height from floor to ceiling
		const int m_step;									// Step size (space between voxels)

		std::vector<bool> m_toggle_camera;

		std::vector<cv::Point3f*> m_corners;				// Cube half-space corner locations

		size_t m_voxels_amount;								// Voxel count
		cv::Size m_plane_size;								// Camera FoV plane WxH

		std::vector<Voxel*> m_voxels;						// Pointer vector to all voxels in the half-space
		std::vector<Voxel*> m_visible_voxels;				// Pointer vector to all visible voxels
		std::vector<VoxelGPU> m_visible_voxels_gpu;
		cv::Mat m_labels;									// Clustering labels for each voxel
		cv::Mat m_cluster_centers;							// Cluster centers for each person in the 3d voxel space

		// Lookup table per camera, where a pixel (y * width + x) maps to all of the voxels that are projected onto it   
		std::vector<std::map<int, std::vector<Voxel*>>> m_lookup;

		int m_all_camera_flags;
		bool m_saved_2d_tracking = false;

		std::vector<cv::Point3f> m_bins;
		const std::vector<std::vector<int>> m_permutations;

		std::vector<std::vector<Vertex>> m_2d_tracking;	// Keeping track of 2d coordinates per person, over time

		void initVoxels(int offsetX, int offsetY);
		void updateVoxels();
		void labelVoxels();
		void trackClusters(int permutation);
		void colorVoxels(int permutation);
		bool colorVoxel(Voxel* voxel, int cam);
		VoxelGPU createVoxelGPU(Voxel const& voxel);
		/*
		 * Call after voxels have been labeled
		 */
		void createColorModels(int cam, std::vector<Histogram*>&);
		int matchClusters();
		void initColorModels();
		void initBins();
		float matchModels(std::vector<Histogram*>&, std::vector<Histogram*>&, int& outPermutation);

	public:
		VoxelReconstruction(const std::vector<VoxelCamera*>&, int height, int step);
		virtual ~VoxelReconstruction();

		void update();

		const std::vector<Voxel*>& getVisibleVoxels() const
		{
			return m_visible_voxels;
		}

		const std::vector<VoxelGPU>& getVisibleVoxelsGPU() const
		{
			return m_visible_voxels_gpu;
		}

		const std::vector<Voxel*>& getVoxels() const
		{
			return m_voxels;
		}

		void toggleCamera(const int& cam_id)
		{
			if (cam_id >= 0 && cam_id < m_toggle_camera.size())
			{
				bool b = m_toggle_camera[cam_id];
				m_toggle_camera[cam_id] = !b;
			}
		}

		const std::vector<cv::Point3f*>& getCorners() const
		{
			return m_corners;
		}

		int getSize() const
		{
			return m_height;
		}

		const cv::Size& getPlaneSize() const
		{
			return m_plane_size;
		}

		const int& getStep() const
		{
			return m_step;
		}

		const std::vector<std::vector<Vertex>>& get2dTracking() const
		{
			return m_2d_tracking;
		}
		
		void smooth2dTracking();
		void save2dTracking();

	};

} /* namespace team45 */

#endif /* VOXELRECONSTRUCTION_H */