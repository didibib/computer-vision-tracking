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

namespace team45
{
	Window& WINDOW = Window::GetInstance();

	Window::~Window()
	{
		delete m_scene3d;
		delete m_voxel_shader;
		delete m_cam_coord;
		delete m_floor_grid;
		glfwDestroyWindow(m_glfwWindow);
		glfwTerminate();
	}

	/**
	 * Main OpenGL initialisation for Windows-like system
	 */
	bool Window::init(const char* win_name, Scene3DRenderer& s3d)
	{
		m_scene3d = &s3d;
		/*      Screen/display attributes*/
		int width = m_scene3d->getWidth();
		int height = m_scene3d->getHeight();
		int bits = 32;

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

		// Load Shader
		m_voxel_shader = new Shader();
		m_voxel_shader->Load(util::SHADER_DIR_STR + "voxel");

		// Create camera
		m_scene_cam = new SceneCamera(60.f, width, height, 0.1f, 10000.f);
		int size = m_scene3d->getReconstructor().getSize();
		m_scene_cam->SetPos(size, 0, size);

		// Create voxel
		m_cube = new VertexBuffer();
		m_cube->Create(Cube::GetVertices());

		// Create floor grid
		auto verts = m_scene3d->createFloorGrid();
		m_floor_grid = new VertexBuffer();
		m_floor_grid->Create(verts);

		// Create coords
		createCamCoord();
		createWCoord();

		return true;
	}

	/**
	 * This loop updates and displays the scene every iteration
	 */
	void Window::run()
	{
		while (!glfwWindowShouldClose(m_glfwWindow))
		{
			m_currentTime = glfwGetTime();
			m_deltaTime = m_currentTime - m_previousTime;
			m_previousTime = m_currentTime;

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
		Scene3DRenderer& scene3d = WINDOW.getScene3d();
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
		glClearColor(245.f / 255.0f, 245.f / 255.0f, 220.f / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Scene3DRenderer& scene3d = WINDOW.getScene3d();

		m_voxel_shader->Begin();
		// Set camera matrices
		m_voxel_shader->SetMat4("u_Projection", m_scene_cam->GetProjMatrix());
		m_voxel_shader->SetMat4("u_View", m_scene_cam->GetViewMatrix());

		glm::mat4 model = glm::mat4(1.0f);
		m_voxel_shader->SetMat4("u_Model", model);

		// Draw origin
		m_w_coord->Bind();
		m_w_coord->Draw(GL_LINES);
		m_w_coord->Unbind();

		// Draw floor grid
		m_floor_grid->Bind();
		m_floor_grid->Draw(GL_LINES);
		m_floor_grid->Unbind();

		// Draw cameras
		m_cam_coord->Bind();
		m_cam_coord->Draw(GL_LINES);
		m_cam_coord->Unbind();

		// Draw voxels
		drawVoxels();

		m_voxel_shader->End();
	}

	void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Scene3DRenderer& scene3d = WINDOW.getScene3d();
		if (action == GLFW_RELEASE) return;
		switch (key)
		{
		case GLFW_KEY_W: { WINDOW.m_scene_cam->Move(Direction::Forward, WINDOW.m_deltaTime); } break;
		case GLFW_KEY_A: { WINDOW.m_scene_cam->Move(Direction::Left, WINDOW.m_deltaTime); } break;
		case GLFW_KEY_S: { WINDOW.m_scene_cam->Move(Direction::Backward, WINDOW.m_deltaTime); } break;
		case GLFW_KEY_D: { WINDOW.m_scene_cam->Move(Direction::Right, WINDOW.m_deltaTime); } break;
		case GLFW_KEY_LEFT_CONTROL: { WINDOW.m_scene_cam->Move(Direction::Down, WINDOW.m_deltaTime); } break;
		case GLFW_KEY_SPACE: { WINDOW.m_scene_cam->Move(Direction::Up, WINDOW.m_deltaTime); }break;
		case GLFW_KEY_P: { WINDOW.m_paused = !WINDOW.m_paused; } break;
		case GLFW_KEY_B: { scene3d.setCurrentFrame(scene3d.getCurrentFrame() - 1); } break;
		case GLFW_KEY_N: { scene3d.setCurrentFrame(scene3d.getCurrentFrame() + 1); } break;
		default:
			break;
		}

		int num = key - GLFW_KEY_1;
		// Check if key is 1 .. 4
		if (num >= 1 && num <= (int)scene3d.getCameras().size())
		{
			scene3d.toggleCamera(--num);
		}
		
	}

	void Window::mouseCallback(GLFWwindow* window, int button, int action, int mods)
	{

	}

	void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{

	}

	void Window::cursorCallback(GLFWwindow* window, double xpos, double ypos)
	{
		if (WINDOW.reset_cursor)
		{
			WINDOW.m_cursor_last_pos = cv::Point2f(xpos, ypos);
			WINDOW.reset_cursor = false;
		}
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			WINDOW.m_scene_cam->Cursor(xpos - WINDOW.m_cursor_last_pos.x, WINDOW.m_cursor_last_pos.y - ypos);
		}
		WINDOW.m_cursor_last_pos = cv::Point2f(xpos, ypos);
	}

	void Window::frameBufferCallback(GLFWwindow* window, int width, int height)
	{
		WINDOW.m_scene3d->setSize(width, height);
		WINDOW.m_scene_cam->OnWindowResize(width, height);
		glViewport(0, 0, width, height);
	}

	void Window::cursorEnterCallback(GLFWwindow* window, int entered)
	{
		WINDOW.reset_cursor = entered;
	}

	void Window::windowFocusCallback(GLFWwindow* window, int focused)
	{
		WINDOW.reset_cursor = focused;
	}

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

		delete m_cam_coord;
		m_cam_coord = new VertexBuffer();
		m_cam_coord->Create(vertices);
	}

	/**
	 * Draw the voxel bounding box
	 */
	void Window::drawVolume()
	{
		std::vector<cv::Point3f*> corners = m_scene3d->getReconstructor().getCorners();

		glLineWidth(1.0f);
		glPushMatrix();
		glBegin(GL_LINES);

		// VR->volumeCorners[0]; // what's this frank?
		// bottom
		glColor4f(0.9f, 0.9f, 0.9f, 0.5f);
		glVertex3f(corners[0]->x, corners[0]->y, corners[0]->z);
		glVertex3f(corners[1]->x, corners[1]->y, corners[1]->z);

		glColor4f(0.9f, 0.9f, 0.9f, 0.5f);
		glVertex3f(corners[1]->x, corners[1]->y, corners[1]->z);
		glVertex3f(corners[2]->x, corners[2]->y, corners[2]->z);

		glColor4f(0.9f, 0.9f, 0.9f, 0.5f);
		glVertex3f(corners[2]->x, corners[2]->y, corners[2]->z);
		glVertex3f(corners[3]->x, corners[3]->y, corners[3]->z);

		glColor4f(0.9f, 0.9f, 0.9f, 0.5f);
		glVertex3f(corners[3]->x, corners[3]->y, corners[3]->z);
		glVertex3f(corners[0]->x, corners[0]->y, corners[0]->z);

		// top
		glColor4f(0.9f, 0.9f, 0.9f, 0.5f);
		glVertex3f(corners[4]->x, corners[4]->y, corners[4]->z);
		glVertex3f(corners[5]->x, corners[5]->y, corners[5]->z);

		glColor4f(0.9f, 0.9f, 0.9f, 0.5f);
		glVertex3f(corners[5]->x, corners[5]->y, corners[5]->z);
		glVertex3f(corners[6]->x, corners[6]->y, corners[6]->z);

		glColor4f(0.9f, 0.9f, 0.9f, 0.5f);
		glVertex3f(corners[6]->x, corners[6]->y, corners[6]->z);
		glVertex3f(corners[7]->x, corners[7]->y, corners[7]->z);

		glColor4f(0.9f, 0.9f, 0.9f, 0.5f);
		glVertex3f(corners[7]->x, corners[7]->y, corners[7]->z);
		glVertex3f(corners[4]->x, corners[4]->y, corners[4]->z);

		// connection
		glColor4f(0.9f, 0.9f, 0.9f, 0.5f);
		glVertex3f(corners[0]->x, corners[0]->y, corners[0]->z);
		glVertex3f(corners[4]->x, corners[4]->y, corners[4]->z);

		glColor4f(0.9f, 0.9f, 0.9f, 0.5f);
		glVertex3f(corners[1]->x, corners[1]->y, corners[1]->z);
		glVertex3f(corners[5]->x, corners[5]->y, corners[5]->z);

		glColor4f(0.9f, 0.9f, 0.9f, 0.5f);
		glVertex3f(corners[2]->x, corners[2]->y, corners[2]->z);
		glVertex3f(corners[6]->x, corners[6]->y, corners[6]->z);

		glColor4f(0.9f, 0.9f, 0.9f, 0.5f);
		glVertex3f(corners[3]->x, corners[3]->y, corners[3]->z);
		glVertex3f(corners[7]->x, corners[7]->y, corners[7]->z);

		glEnd();
		glPopMatrix();
	}

	/**
	 * Draw all visible voxels
	 */
	void Window::drawVoxels()
	{
		std::vector<VoxelReconstruction::Voxel*> voxels = m_scene3d->getReconstructor().getVisibleVoxels();
		int size = WINDOW.m_scene3d->getReconstructor().getStep();
		for (size_t v = 0; v < voxels.size(); v++)
		{
			glm::vec3 pos = glm::vec3(voxels[v]->x, voxels[v]->y, voxels[v]->z);
			glm::vec3 scale = glm::vec3(size);

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::scale(model, scale);
			model = glm::translate(model, pos / scale);

			m_voxel_shader->SetMat4("u_Model", model);

			m_cube->Bind();
			m_cube->Draw();
		}
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

		delete m_w_coord;
		m_w_coord = new VertexBuffer();
		m_w_coord->Create(vertices);
	}

} /* namespace team45 */
