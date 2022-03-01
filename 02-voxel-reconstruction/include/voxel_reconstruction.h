#pragma once
#ifndef VOXELRECONSTRUCTION_H
#define VOXELRECONSTRUCTION_H

namespace team45
{
	class VoxelCamera;
	class VoxelReconstruction
	{
	public:
		/*
		 * Voxel structure
		 * Represents a 3D pixel in the half space
		 */
		struct Voxel
		{
			glm::ivec3 position;
			glm::vec3 color;    
			std::vector<float> distances;				// Distance from this voxel to each camera 
			std::vector<cv::Point> pixelProjections;	// Pixel that this voxel projects to on each camera ((-1,-1) if it doesn't project onto the camera)
			int visibleIndex;							// Coordinates and index in m_visible_voxels (-1 if none)
			int camera_flags;							// Bitwise Flag if voxel was on in camera[c] in the previous frame
		};

	private:
		const std::vector<VoxelCamera*>& m_cameras;		// vector of pointers to cameras
		const int m_height;								// Cube half-space height from floor to ceiling
		const int m_step;								// Step size (space between voxels)

		std::vector<bool> m_toggle_camera;

		std::vector<cv::Point3f*> m_corners;			// Cube half-space corner locations

		size_t m_voxels_amount;							// Voxel count
		cv::Size m_plane_size;							// Camera FoV plane WxH

		std::vector<Voxel*> m_voxels;					// Pointer vector to all voxels in the half-space
		std::vector<Voxel*> m_visible_voxels;			// Pointer vector to all visible voxels

		// Lookup table per camera, where a pixel (y*width + x) maps to all of the voxels that are projected onto it   
		std::vector<std::map<int, std::vector<Voxel*>>> m_lookup;

		int m_all_camera_flags;

		void initialize();

	public:
		VoxelReconstruction(const std::vector<VoxelCamera*>&, int height = 2048, int step = 128);
		virtual ~VoxelReconstruction();

		void update();
		void colorVoxels();

		const std::vector<Voxel*>& getVisibleVoxels() const
		{
			return m_visible_voxels;
		}

		const std::vector<Voxel*>& getVoxels() const
		{
			return m_voxels;
		}

		void setVisibleVoxels(const std::vector<Voxel*>& visibleVoxels)
		{
			m_visible_voxels = visibleVoxels;
		}

		void setVoxels(const std::vector<Voxel*>& voxels)
		{
			m_voxels = voxels;
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
	};

} /* namespace team45 */

#endif /* VOXELRECONSTRUCTION_H */
