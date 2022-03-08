#include "cvpch.h"
#include "window.h"

#include "util.h"
#include "voxel_camera.h"
#include "voxel_reconstruction.h"
#include "scene_renderer.h"
#include "shader.h"
#include "vertex_buffer.h"
#include "scene_camera.h"
#include "cube.h"
#include "voxel_buffer.h"

namespace team45
{
	Window& WINDOW = Window::GetInstance();

	Window::~Window()
	{
		delete m_scene3d;
		delete m_scene_camera;

		delete m_basic_shader;
		delete m_voxel_shader;

		delete m_voxel_shader;
		delete m_cam_coord_vb;
		delete m_floor_grid_vb;
		delete m_volume_vb;
		delete m_cube_vb;

		glfwDestroyWindow(m_glfwWindow);
		glfwTerminate();
	}

	/**
	 * OpenGL context initialisation
	 */
	bool Window::init(const char* win_name, Scene3DRenderer& s3d)
	{
		m_scene3d = &s3d;
		int width = m_scene3d->getWidth();
		int height = m_scene3d->getHeight();

		glfwInit();
		// Window hints need to be set before the creation of the window. They function as additional arguments to glfwCreateWindow.
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		m_glfwWindow = glfwCreateWindow(width, height, win_name, nullptr, nullptr);

		if (!m_glfwWindow)
		{
			CRITICAL("Failed to create window");
			return false;
		}
		glfwSetWindowUserPointer(m_glfwWindow, this);
		glfwMakeContextCurrent(m_glfwWindow);

		// Load OpenGL extensions
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			CRITICAL("Failed to initialize OpengGL context");
			return false;
		}

		glEnable(GL_DEPTH_TEST);

		// Setup callbacks
		glfwSetKeyCallback(m_glfwWindow, keyCallback);
		glfwSetMouseButtonCallback(m_glfwWindow, mouseCallback);
		glfwSetScrollCallback(m_glfwWindow, scrollCallback);
		glfwSetCursorPosCallback(m_glfwWindow, cursorCallback);
		glfwSetFramebufferSizeCallback(m_glfwWindow, frameBufferCallback);
		glfwSetCursorEnterCallback(m_glfwWindow, cursorEnterCallback);
		glfwSetWindowFocusCallback(m_glfwWindow, windowFocusCallback);

		// Clear color
		m_clear_color = glm::vec4(245.f / 255.0f, 245.f / 255.0f, 220.f / 255.0f, 1.0f);

		// Load Shaders
		m_basic_shader = new Shader();
		m_basic_shader->Load(util::SHADER_DIR_STR + "basic");

		m_voxel_shader = new Shader();
		m_voxel_shader->Load(util::SHADER_DIR_STR + "voxel");

		// Create scene camera
		m_scene_camera = new SceneCamera(60.f, width, height, 0.1f, 10000.f);
		int size = m_scene3d->getReconstructor().getSize();
		m_scene_camera->SetPos(1000, 0, 100);

		// Create voxel
		m_cube_vb = new VertexBuffer();
		m_cube_vb->SetName("Cube");
		m_cube_vb->Create(Cube::GetVertices());

		// Create floor grid
		auto verts = m_scene3d->createFloorGrid();
		m_floor_grid_vb = new VertexBuffer();
		m_floor_grid_vb->SetName("Floor grid");
		m_floor_grid_vb->Create(verts);

		// Create coords to visualize
		createCamCoord();
		createWCoord();
		createVolume();

		// Create voxel buffer to draw instanced voxels
		m_voxel_buffer = new VoxelBuffer();
		m_voxel_buffer->Create(s3d.getReconstructor().getVoxels().size());

		return true;
	}

	/**
	 * This loop updates and displays the scene every iteration
	 */
	void Window::run()
	{
		static float currentTime = 0, previousTime = 0;
		while (!glfwWindowShouldClose(m_glfwWindow))
		{
			currentTime = glfwGetTime();
			m_deltaTime = currentTime - previousTime;
			previousTime = currentTime;

			update();
			draw();

			glfwSwapBuffers(m_glfwWindow);
			glfwPollEvents();
		}
	}
	/**
	 * - Update the scene with a new frame from the video
	 * - Handle the keyboard input from the OpenCV window
	 * - Update the OpenCV video window and frames slider position
	 */
	void Window::update()
	{
		if (WINDOW.m_rotate_camera)
		{
			WINDOW.m_scene_camera->RotateAroundPoint(glm::vec3(0.f, 0.f, 500.f), 5000, -0.5f);
		}

		Scene3DRenderer& scene3d = WINDOW.getScene3d();

		if (!m_rotate_camera)
		{
			if (isKeyRepeat(GLFW_KEY_W)) WINDOW.m_scene_camera->Move(Direction::Forward, WINDOW.m_deltaTime);
			if (isKeyRepeat(GLFW_KEY_A)) WINDOW.m_scene_camera->Move(Direction::Left, WINDOW.m_deltaTime);
			if (isKeyRepeat(GLFW_KEY_S)) WINDOW.m_scene_camera->Move(Direction::Backward, WINDOW.m_deltaTime);
			if (isKeyRepeat(GLFW_KEY_D)) WINDOW.m_scene_camera->Move(Direction::Right, WINDOW.m_deltaTime);
		}
		if (isKeyRepeat(GLFW_KEY_LEFT_CONTROL))  WINDOW.m_scene_camera->Move(Direction::Down, WINDOW.m_deltaTime);
		if (isKeyRepeat(GLFW_KEY_SPACE))  WINDOW.m_scene_camera->Move(Direction::Up, WINDOW.m_deltaTime);

		if (isKeyPressed(GLFW_KEY_P)) WINDOW.m_paused = !WINDOW.m_paused;
		if (isKeyPressed(GLFW_KEY_B)) scene3d.setCurrentFrame(scene3d.getCurrentFrame() - 1);
		if (isKeyPressed(GLFW_KEY_N)) scene3d.setCurrentFrame(scene3d.getCurrentFrame() + 1);
		if (isKeyPressed(GLFW_KEY_R))
		{
			WINDOW.m_rotate_camera = !WINDOW.m_rotate_camera;
			WINDOW.m_scene_camera->Reset(WINDOW.m_rotate_camera);
		}

		if (scene3d.getCurrentFrame() > scene3d.getNumberOfFrames() - 2)
		{
			// Go to the start of the video if we've moved beyond the end
			scene3d.setCurrentFrame(0);
			for (size_t c = 0; c < scene3d.getCameras().size(); ++c)
				scene3d.getCameras()[c]->setVideoFrame(scene3d.getCurrentFrame());
		}
		if (scene3d.getCurrentFrame() < 0)
		{
			// Go to the end of the video if we've moved before the start
			scene3d.setCurrentFrame(scene3d.getNumberOfFrames() - 2);
			for (size_t c = 0; c < scene3d.getCameras().size(); ++c)
				scene3d.getCameras()[c]->setVideoFrame(scene3d.getCurrentFrame());
		}
		if (!m_paused)
		{
			// If not paused move to the next frame
			scene3d.setCurrentFrame(scene3d.getCurrentFrame() + 1);
		}
		if (scene3d.getCurrentFrame() != scene3d.getPreviousFrame())
		{
			// If the current frame is different from the last iteration update stuff
			scene3d.processFrame();
			scene3d.getReconstructor().update();
			scene3d.setPreviousFrame(scene3d.getCurrentFrame());
		}

		// Get the image and the foreground image (of set camera)
		cv::Mat canvas, foreground;
		if (scene3d.getCurrentCamera() != -1)
		{
			canvas = scene3d.getCameras()[scene3d.getCurrentCamera()]->getFrame();
			foreground = scene3d.getCameras()[scene3d.getCurrentCamera()]->getForegroundImage();
		}
		else
		{
			canvas = scene3d.getCameras()[scene3d.getPreviousCamera()]->getFrame();
			foreground = scene3d.getCameras()[scene3d.getPreviousCamera()]->getForegroundImage();
		}

		// Concatenate the video frame with the foreground image (of set camera)
		if (!canvas.empty() && !foreground.empty())
		{
			cv::Mat fg_im_3c;
			cvtColor(foreground, fg_im_3c, CV_GRAY2BGR);
			hconcat(canvas, fg_im_3c, canvas);
			imshow(util::VIDEO_WINDOW, canvas);
		}
		else if (!canvas.empty())
		{
			imshow(util::VIDEO_WINDOW, canvas);
		}

		// Update the frame slider position
		cv::setTrackbarPos("Frame", util::VIDEO_WINDOW, scene3d.getCurrentFrame());
	}

	/**
	 * Render the 3D scene
	 */
	void Window::draw()
	{
		glClearColor(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Scene3DRenderer& scene3d = WINDOW.getScene3d();

		m_basic_shader->Begin();
		// Set camera matrices
		auto projectionView = m_scene_camera->GetProjMatrix() * m_scene_camera->GetViewMatrix();
		m_basic_shader->SetMat4("u_ProjectionView", projectionView);

		glm::mat4 model = glm::mat4(1.0f);
		// Rotate scene
		float angle_radians = -(float)glm::radians(glfwGetTime()) * 2;
		glm::vec3 axis = glm::vec3(0, 0, 1);
		//model = glm::rotate(model, angle_radians, axis);
		m_basic_shader->SetMat4("u_Model", model);

		// Visualize scene
		drawWireframe(m_w_coord_vb);
		drawWireframe(m_floor_grid_vb);
		drawWireframe(m_cam_coord_vb);
		drawWireframe(m_volume_vb);

		m_basic_shader->End();

		// Draw voxels
		drawVoxels();
	}

	void Window::drawWireframe(VertexBuffer* vb)
	{
		if (vb == nullptr)
		{
			ERROR("Vertexbuffer is not instantiated {}", vb->GetName());
			return;
		}
		vb->Bind();
		vb->Draw(GL_LINES);
		vb->Unbind();
	}

	/**
	 * Draw all visible voxels
	 */
	void Window::drawVoxels()
	{
		std::vector<VoxelGPU> voxels = m_scene3d->getReconstructor().getVisibleVoxelsGPU();
		int size = WINDOW.m_scene3d->getReconstructor().getStep();
		auto projectionView = m_scene_camera->GetProjMatrix() * m_scene_camera->GetViewMatrix();

		m_voxel_shader->Begin();
		// Set camera matrices
		m_voxel_shader->SetMat4("u_ProjectionView", projectionView);
		// Draw voxels

		m_voxel_buffer->Draw(voxels);


		m_voxel_shader->End();
	}

	bool Window::isKeyRepeat(int key)
	{
		return m_key_lookup[key] == GLFW_PRESS || m_key_lookup[key] == GLFW_REPEAT;
	}

	bool Window::isKeyPressed(int key)
	{
		return m_key_lookup[key] == GLFW_PRESS;
	}

#pragma region __CALLBACKS
	void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		WINDOW.m_key_lookup[key] = action;
		Scene3DRenderer& scene3d = WINDOW.getScene3d();
		// Check if key is 1 .. 4
		int num = key - GLFW_KEY_1;
		if (num >= 0 && num < (int)scene3d.getCameras().size())
		{
			scene3d.toggleCamera(num);
		}
	}

	void Window::mouseCallback(GLFWwindow* window, int button, int action, int mods)
	{

	}

	void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		WINDOW.m_scene_camera->Zoom(yoffset);
	}

	void Window::cursorCallback(GLFWwindow* window, double xpos, double ypos)
	{
		if (WINDOW.m_reset_cursor)
		{
			WINDOW.m_cursor_last_pos = cv::Point2f(xpos, ypos);
			WINDOW.m_reset_cursor = false;
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS
			&& !WINDOW.m_rotate_camera)
		{
			WINDOW.m_scene_camera->Cursor(xpos - WINDOW.m_cursor_last_pos.x, WINDOW.m_cursor_last_pos.y - ypos);
		}
		WINDOW.m_cursor_last_pos = cv::Point2f(xpos, ypos);
	}

	void Window::frameBufferCallback(GLFWwindow* window, int width, int height)
	{
		WINDOW.m_scene3d->setSize(width, height);
		WINDOW.m_scene_camera->OnWindowResize(width, height);
		glViewport(0, 0, width, height);
	}

	void Window::cursorEnterCallback(GLFWwindow* window, int entered)
	{
		WINDOW.m_reset_cursor = entered;
	}

	void Window::windowFocusCallback(GLFWwindow* window, int focused)
	{
		WINDOW.m_reset_cursor = focused;
	}

#pragma endregion


	/**
	 * Draw the cameras
	 */
	void Window::createCamCoord()
	{
		std::vector<VoxelCamera*> cameras = m_scene3d->getCameras();

		std::vector<Vertex> vertices;

		for (size_t i = 0; i < cameras.size(); i++)
		{
			std::vector<cv::Point3f> plane = cameras[i]->getCameraPlane();

			// 0 - 1
			Vertex v0;
			v0.Position = glm::vec3(plane[0].x, plane[0].y, plane[0].z);
			v0.Color = glm::vec4(0.8f, 0.8f, 0.8f, 0.5f);

			Vertex v1 = v0;
			v1.Position = glm::vec3(plane[1].x, plane[1].y, plane[1].z);

			vertices.push_back(v0);
			vertices.push_back(v1);

			// 0 - 2
			Vertex v2 = v0;
			v2.Position = glm::vec3(plane[2].x, plane[2].y, plane[2].z);

			vertices.push_back(v0);
			vertices.push_back(v2);

			// 0 - 3
			Vertex v3 = v0;
			v3.Position = glm::vec3(plane[3].x, plane[3].y, plane[3].z);

			vertices.push_back(v0);
			vertices.push_back(v3);

			// 0 - 4
			Vertex v4 = v0;
			v4.Position = glm::vec3(plane[4].x, plane[4].y, plane[4].z);

			vertices.push_back(v0);
			vertices.push_back(v4);

			// 1 - 2
			vertices.push_back(v1);
			vertices.push_back(v2);

			// 2 - 3
			vertices.push_back(v2);
			vertices.push_back(v3);

			// 3 - 4
			vertices.push_back(v3);
			vertices.push_back(v4);

			// 4 - 1
			vertices.push_back(v4);
			vertices.push_back(v1);
		}

		delete m_cam_coord_vb;
		m_cam_coord_vb = new VertexBuffer();
		m_cam_coord_vb->SetName("Camera Coord");
		m_cam_coord_vb->Create(vertices);
	}

	/**
	 * Draw the voxel bounding box
	 */
	void Window::createVolume()
	{
		std::vector<cv::Point3f*> corners = m_scene3d->getReconstructor().getCorners();

		std::vector<Vertex> vertices;
		Vertex v0;
		v0.Position = glm::vec3(corners[0]->x, corners[0]->y, corners[0]->z);
		v0.Color = glm::vec4(0.9f, 0.9f, 0.9f, 0.5f);

		Vertex v1 = v0;
		v1.Position = glm::vec3(corners[1]->x, corners[1]->y, corners[1]->z);

		Vertex v2 = v0;
		v2.Position = glm::vec3(corners[2]->x, corners[2]->y, corners[2]->z);

		Vertex v3 = v0;
		v3.Position = glm::vec3(corners[3]->x, corners[3]->y, corners[3]->z);

		Vertex v4 = v0;
		v4.Position = glm::vec3(corners[4]->x, corners[4]->y, corners[4]->z);

		Vertex v5 = v0;
		v5.Position = glm::vec3(corners[5]->x, corners[5]->y, corners[5]->z);

		Vertex v6 = v0;
		v6.Position = glm::vec3(corners[6]->x, corners[6]->y, corners[6]->z);

		Vertex v7 = v0;
		v7.Position = glm::vec3(corners[7]->x, corners[7]->y, corners[7]->z);

		// bottom
		vertices.push_back(v0);
		vertices.push_back(v1);

		vertices.push_back(v1);
		vertices.push_back(v2);

		vertices.push_back(v2);
		vertices.push_back(v3);

		vertices.push_back(v3);
		vertices.push_back(v0);

		// top
		vertices.push_back(v4);
		vertices.push_back(v5);

		vertices.push_back(v5);
		vertices.push_back(v6);

		vertices.push_back(v6);
		vertices.push_back(v7);

		vertices.push_back(v7);
		vertices.push_back(v4);

		// connection
		vertices.push_back(v0);
		vertices.push_back(v4);

		vertices.push_back(v1);
		vertices.push_back(v5);

		vertices.push_back(v2);
		vertices.push_back(v6);

		vertices.push_back(v3);
		vertices.push_back(v7);

		delete m_volume_vb;
		m_volume_vb = new VertexBuffer();
		m_volume_vb->SetName("Volume");
		m_volume_vb->Create(vertices);
	}

	/**
	 * Draw origin into scene
	 */
	void Window::createWCoord()
	{
		const Scene3DRenderer& scene3d = getScene3d();
		const int len = scene3d.getSquareSideLen();
		const float x_len = float(len * (scene3d.getBoardSize().height - 1));
		const float y_len = float(len * (scene3d.getBoardSize().width - 1));
		const float z_len = float(len * 3);

		std::vector<Vertex> vertices;

		Vertex v0;
		v0.Position = glm::vec3(0.f);

		// x-axis
		Vertex vx;
		vx.Position = glm::vec3(x_len, 0.f, 0.f);
		vx.Color = glm::vec4(1.f, 0.f, 0.f, 0.5f);
		v0.Color = glm::vec4(1.f, 0.f, 0.f, 0.5f);

		vertices.push_back(v0);
		vertices.push_back(vx);

		// y-axis
		Vertex vy;
		vy.Position = glm::vec3(0.f, y_len, 0.f);
		vy.Color = glm::vec4(0.f, 1.f, 0.f, 0.5f);
		v0.Color = glm::vec4(0.f, 1.f, 0.f, 0.5f);

		vertices.push_back(v0);
		vertices.push_back(vy);

		// z-axis
		Vertex vz;
		vz.Position = glm::vec3(0.f, 0.f, z_len);
		vz.Color = glm::vec4(0.f, 0.f, 1.f, 0.5f);
		v0.Color = glm::vec4(0.f, 0.f, 1.f, 0.5f);

		vertices.push_back(v0);
		vertices.push_back(vz);

		delete m_w_coord_vb;
		m_w_coord_vb = new VertexBuffer();
		m_w_coord_vb->SetName("W Coord");
		m_w_coord_vb->Create(vertices);
	}

} /* namespace team45 */
