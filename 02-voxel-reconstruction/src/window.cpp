#include "cvpch.h"
#include "window.h"

#include "util.h"
#include "camera.h"
#include "reconstructor.h"
#include "scene_renderer.h"

namespace team45
{

	Window* Window::m_Window;

	Window::Window(Scene3DRenderer& s3d) : m_scene3d(s3d)
	{
		// static pointer to this class so we can get to it from the static GL events
		m_Window = this;
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_glfwWindow);
		glfwTerminate();
	}

	/**
	 * Main OpenGL initialisation for Windows-like system (without Glut)
	 */
	bool Window::init(const char* win_name)
	{
		Scene3DRenderer& scene3d = m_Window->getScene3d();

		/*      Screen/display attributes*/
		int width = scene3d.getWidth();
		int height = scene3d.getHeight();
		int bits = 32;

		glfwInit();
		// Window hints need to be set before the creation of the window. They function as additional arguments to glfwCreateWindow.
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
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

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// Setup callbacks
		glfwSetKeyCallback(m_glfwWindow, keyCallback);
		glfwSetMouseButtonCallback(m_glfwWindow, mouseCallback);
		glfwSetScrollCallback(m_glfwWindow, scrollCallback);
		glfwSetCursorPosCallback(m_glfwWindow, cursorCallback);

		return true;
	}

	/**
	 * This loop updates and displays the scene every iteration
	 */
	void Window::run()
	{
		while (!glfwWindowShouldClose(m_glfwWindow))
		{
			update(0);
			draw();
			glfwSwapBuffers(m_glfwWindow);
			glfwPollEvents();
		}
	}

	/**
	 * http://nehe.gamedev.net/article/replacement_for_gluperspective/21002/
	 * replacement for gluPerspective();
	 */
	void Window::perspectiveGL(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar)
	{
		GLdouble fW, fH;
		fH = tan(fovY / 360 * CV_PI) * zNear;
		fW = fH * aspect;
		glFrustum(-fW, fW, -fH, fH, zNear, zFar);
	}

	void Window::reset()
	{
		Scene3DRenderer& scene3d = m_Window->getScene3d();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		perspectiveGL(50, scene3d.getAspectRatio(), 1, 40000);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	void Window::quit()
	{
		m_Window->getScene3d().setQuit(true);
		exit(EXIT_SUCCESS);
	}

	/**
	 * Handle all keyboard input
	 */
	void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Scene3DRenderer& scene3d = m_Window->getScene3d();
		if (action != GLFW_PRESS) return;
		switch (key)
		{
		case GLFW_KEY_Q:
		{
			scene3d.setQuit(true);
		}
		break;
		case GLFW_KEY_P:
		{
			bool paused = scene3d.isPaused();
			scene3d.setPaused(!paused);
		}
		break;
		case GLFW_KEY_B:
		{
			scene3d.setCurrentFrame(scene3d.getCurrentFrame() - 1);

		}
		break;
		case GLFW_KEY_N:
		{
			scene3d.setCurrentFrame(scene3d.getCurrentFrame() + 1);
		}
		break;
		case GLFW_KEY_R:
		{
			bool rotate = scene3d.isRotate();
			scene3d.setRotate(!rotate);
		break;
		}
		break;
		case GLFW_KEY_S:
		{
#ifdef _WIN32
			std::cerr << "ShowArcball() not supported on Windows!" << std::endl;
#endif
			bool arcball = scene3d.isShowArcball();
			scene3d.setShowArcball(!arcball);
		}
		break;
		case GLFW_KEY_V:
		{
			bool volume = scene3d.isShowVolume();
			scene3d.setShowVolume(!volume);
		}
		break;
		case GLFW_KEY_G:
		{
			bool floor = scene3d.isShowGrdFlr();
			scene3d.setShowGrdFlr(!floor);
		}
		break;
		case GLFW_KEY_C:
		{
			bool cam = scene3d.isShowCam();
			scene3d.setShowCam(!cam);
		}
		break;
		case GLFW_KEY_I:
		{
#ifdef _WIN32
			std::cerr << "ShowInfo() not supported on Windows!" << std::endl;
#endif
			bool info = scene3d.isShowInfo();
			scene3d.setShowInfo(!info);
		}
		break;
		case GLFW_KEY_O:
		{
			bool origin = scene3d.isShowOrg();
			scene3d.setShowOrg(!origin);
		}
		break;
		case GLFW_KEY_T:
		{
			scene3d.setTopView();
			reset();
			//arcball_reset();
		}
		break;
		default:
			break;
		}

		int num = key - GLFW_KEY_0;
		if (num >= 0 && num <= (int)scene3d.getCameras().size())
		{
			scene3d.setCamera(num);
			reset();
			//arcball_reset();
		}
	}

	void Window::mouseCallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		{
			double xpos, ypos;
			//getting cursor position
			glfwGetCursorPos(window, &xpos, &ypos); 
			const int invert_ypos = (m_Window->getScene3d().getHeight() - ypos) - 1;  // OpenGL viewport coordinates are Cartesian
			//arcball_start(xpos, invert_ypos);
		}
	}

	void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (yoffset < 0 && !m_Window->getScene3d().isCameraView())
		{
			//arcball_add_distance(+250);
		}
		else if (yoffset > 0 && !m_Window->getScene3d().isCameraView())
		{
			//arcball_add_distance(-250);
		}
	}

	void Window::cursorCallback(GLFWwindow* window, double xpos, double ypos)
	{
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			motion(xpos, ypos);
		}
	}

	/**
	 * Rotate the scene
	 */
	void Window::motion(int x, int y)
	{
		// motion is only called when a mouse button is held down
		int invert_y = (m_Window->getScene3d().getHeight() - y) - 1;
		//arcball_move(x, invert_y);
	}

	/**
	 * Reshape the GL-window
	 */
	void Window::reshape(int width, int height)
	{
		float ar = (float)width / (float)height;
		m_Window->getScene3d().setSize(width, height, ar);
		glViewport(0, 0, width, height);
		reset();
	}

	/**
	 * Render the 3D scene
	 */
	void Window::draw()
	{
		// Enable depth testing
		glEnable(GL_DEPTH_TEST);

		// Here's our rendering. Clears the screen
		// to black, clear the color and depth
		// buffers, and reset our modelview matrix.
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);  //set modelview matrix
		glLoadIdentity();  //reset modelview matrix

		//arcball_rotate();

		Scene3DRenderer& scene3d = m_Window->getScene3d();
		if (scene3d.isShowGrdFlr())
			drawGrdGrid();
		if (scene3d.isShowCam())
			drawCamCoord();
		if (scene3d.isShowVolume())
			drawVolume();

		drawVoxels();

		if (scene3d.isShowOrg())
			drawWCoord();

		glFlush();

#ifdef __linux__
		glutSwapBuffers();
#elif defined _WIN32
		SwapBuffers(scene3d.getHDC());
#endif
	}

	/**
	 * - Update the scene with a new frame from the video
	 * - Handle the keyboard input from the OpenCV window
	 * - Update the OpenCV video window and frames slider position
	 */
	void Window::update(int v){

		Scene3DRenderer& scene3d = m_Window->getScene3d();
		if (scene3d.isQuit())
		{
			// Quit signaled
			quit();
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
		if (!scene3d.isPaused())
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
		else if (scene3d.getHThreshold() != scene3d.getPHThreshold() || scene3d.getSThreshold() != scene3d.getPSThreshold()
			|| scene3d.getVThreshold() != scene3d.getPVThreshold())
		{
			// Update the scene if one of the HSV sliders was moved (when the video is paused)
			scene3d.processFrame();
			scene3d.getReconstructor().update();

			scene3d.setPHThreshold(scene3d.getHThreshold());
			scene3d.setPSThreshold(scene3d.getSThreshold());
			scene3d.setPVThreshold(scene3d.getVThreshold());
		}

		// Auto rotate the scene
		if (scene3d.isRotate())
		{
			//arcball_add_angle(2);
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
	 * Draw the floor
	 */
	void Window::drawGrdGrid()
	{
		std::vector<std::vector<cv::Point3i*> > floor_grid = m_Window->getScene3d().getFloorGrid();

		glLineWidth(1.0f);
		glPushMatrix();
		glBegin(GL_LINES);

		int gSize = m_Window->getScene3d().getNum() * 2 + 1;
		for (int g = 0; g < gSize; g++)
		{
			// y lines
			glColor4f(0.9f, 0.9f, 0.9f, 0.5f);
			glVertex3f((GLfloat)floor_grid[0][g]->x, (GLfloat)floor_grid[0][g]->y, (GLfloat)floor_grid[0][g]->z);
			glVertex3f((GLfloat)floor_grid[2][g]->x, (GLfloat)floor_grid[2][g]->y, (GLfloat)floor_grid[2][g]->z);

			// x lines
			glColor4f(0.9f, 0.9f, 0.9f, 0.5f);
			glVertex3f((GLfloat)floor_grid[1][g]->x, (GLfloat)floor_grid[1][g]->y, (GLfloat)floor_grid[1][g]->z);
			glVertex3f((GLfloat)floor_grid[3][g]->x, (GLfloat)floor_grid[3][g]->y, (GLfloat)floor_grid[3][g]->z);
		}

		glEnd();
		glPopMatrix();
	}

	/**
	 * Draw the cameras
	 */
	void Window::drawCamCoord()
	{
		std::vector<Camera*> cameras = m_Window->getScene3d().getCameras();

		glLineWidth(1.0f);
		glPushMatrix();
		glBegin(GL_LINES);

		for (size_t i = 0; i < cameras.size(); i++)
		{
			std::vector<cv::Point3f> plane = cameras[i]->getCameraPlane();

			// 0 - 1
			glColor4f(0.8f, 0.8f, 0.8f, 0.5f);
			glVertex3f(plane[0].x, plane[0].y, plane[0].z);
			glVertex3f(plane[1].x, plane[1].y, plane[1].z);

			// 0 - 2
			glColor4f(0.8f, 0.8f, 0.8f, 0.5f);
			glVertex3f(plane[0].x, plane[0].y, plane[0].z);
			glVertex3f(plane[2].x, plane[2].y, plane[2].z);

			// 0 - 3
			glColor4f(0.8f, 0.8f, 0.8f, 0.5f);
			glVertex3f(plane[0].x, plane[0].y, plane[0].z);
			glVertex3f(plane[3].x, plane[3].y, plane[3].z);

			// 0 - 4
			glColor4f(0.8f, 0.8f, 0.8f, 0.5f);
			glVertex3f(plane[0].x, plane[0].y, plane[0].z);
			glVertex3f(plane[4].x, plane[4].y, plane[4].z);

			// 1 - 2
			glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
			glVertex3f(plane[1].x, plane[1].y, plane[1].z);
			glVertex3f(plane[2].x, plane[2].y, plane[2].z);

			// 2 - 3
			glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
			glVertex3f(plane[2].x, plane[2].y, plane[2].z);
			glVertex3f(plane[3].x, plane[3].y, plane[3].z);

			// 3 - 4
			glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
			glVertex3f(plane[3].x, plane[3].y, plane[3].z);
			glVertex3f(plane[4].x, plane[4].y, plane[4].z);

			// 4 - 1
			glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
			glVertex3f(plane[4].x, plane[4].y, plane[4].z);
			glVertex3f(plane[1].x, plane[1].y, plane[1].z);
		}

		glEnd();
		glPopMatrix();
	}

	/**
	 * Draw the voxel bounding box
	 */
	void Window::drawVolume()
	{
		std::vector<cv::Point3f*> corners = m_Window->getScene3d().getReconstructor().getCorners();

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
		glPushMatrix();

		// apply default translation
		glTranslatef(0, 0, 0);
		glPointSize(2.0f);
		glBegin(GL_POINTS);

		std::vector<Reconstructor::Voxel*> voxels = m_Window->getScene3d().getReconstructor().getVisibleVoxels();
		for (size_t v = 0; v < voxels.size(); v++)
		{
			glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
			glVertex3f((GLfloat)voxels[v]->x, (GLfloat)voxels[v]->y, (GLfloat)voxels[v]->z);
		}

		glEnd();
		glPopMatrix();
	}

	/**
	 * Draw origin into scene
	 */
	void Window::drawWCoord()
	{
		glLineWidth(1.5f);
		glPushMatrix();
		glBegin(GL_LINES);

		const Scene3DRenderer& scene3d = m_Window->getScene3d();
		const int len = scene3d.getSquareSideLen();
		const float x_len = float(len * (scene3d.getBoardSize().height - 1));
		const float y_len = float(len * (scene3d.getBoardSize().width - 1));
		const float z_len = float(len * 3);

		// draw x-axis
		glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(x_len, 0.0f, 0.0f);

		// draw y-axis
		glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, y_len, 0.0f);

		// draw z-axis
		glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, z_len);

		glEnd();
		glPopMatrix();
	}

} /* namespace team45 */
