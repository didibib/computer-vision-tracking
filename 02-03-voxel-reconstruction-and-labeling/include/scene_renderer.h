#ifndef SCENE3DRENDERER_H
#define SCENE3DRENDERER_H

#include <opencv2/core/core.hpp>
#include "voxel_camera.h"
#include "voxel_reconstruction.h"
#include "vertex_buffer.h"

namespace team45
{
	class Scene3DRenderer
	{
		VoxelReconstruction& m_reconstructor;          // Reference to Reconstructor
		const std::vector<VoxelCamera*>& m_cameras;  // Reference to camera's vector
		const int m_num;                        // Floor grid scale
		const float m_sphere_radius;            // ArcBall sphere radius

		int m_square_side_len;                   // Checkerboard square side length (mm)
		cv::Size m_board_size;                   // WxH checkerboard

		int m_width, m_height;                   // GL window size
		float m_aspect_ratio;                    // GL window frustrum aspect ratio

		/*
		 * Three vectors for the arcball that allows for manipulating the scene with
		 * the mouse like a globe
		 */
		bool m_camera_view;                       // flag if scene viewed from a camera

		long m_number_of_frames;                  // number of video frames
		int m_current_frame;                      // current frame index
		int m_previous_frame;                     // previously drawn frame index

		int m_current_camera;                     // number of currently selected camera view point
		int m_previous_camera;                    // number of previously selected camera view point

	public:
		Scene3DRenderer(VoxelReconstruction&, const std::vector<VoxelCamera*>&);
		~Scene3DRenderer();

		bool processFrame();
		void toggleCamera(int);

		std::vector<Vertex> createFloorGrid();

		const std::vector<VoxelCamera*>& getCameras() const { return m_cameras; }

		bool isCameraView() const { return m_camera_view; }

		void setCameraView(bool cameraView) { m_camera_view = cameraView; }

		int getCurrentCamera() const { return m_current_camera; }

		void setCurrentCamera(int currentCamera) { m_current_camera = currentCamera; }

		int getCurrentFrame() const { return m_current_frame; }

		void setCurrentFrame(int currentFrame) { m_current_frame = currentFrame; }

		long getNumberOfFrames() const { return m_number_of_frames; }

		void setNumberOfFrames( long numberOfFrames) { m_number_of_frames = numberOfFrames; }

		int getPreviousFrame() const { return m_previous_frame; }

		void setPreviousFrame( int previousFrame) { m_previous_frame = previousFrame; }

		int getHeight() const { return m_height; }

		int getWidth() const { return m_width; }

		void setSize( int w, int h) { m_width = w; m_height = h; m_aspect_ratio = (float)w / (float)h; }

		float getSphereRadius() const { return m_sphere_radius; }

		float getAspectRatio() const { return m_aspect_ratio; }

		int getNum() const { return m_num; }

		VoxelReconstruction& getReconstructor() const { return m_reconstructor; }

		int getPreviousCamera() const { return m_previous_camera; }

		const cv::Size& getBoardSize() const { return m_board_size; }

		int getSquareSideLen() const { return m_square_side_len; }
	};

} /* namespace team45 */

#endif /* SCENE3DRENDERER_H_ */
