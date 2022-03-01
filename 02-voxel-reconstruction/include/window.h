#pragma once

namespace team45
{
	class SceneCamera;
	class Scene3DRenderer;
	class Cube;
	class Shader;
	class VertexBuffer;

	// https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
	class Window
	{
		Window() {};

		float m_deltaTime = 0;
		bool m_paused = false;

		glm::vec4 m_clear_color;

		SceneCamera* m_scene_cam;
		Scene3DRenderer* m_scene3d;
		GLFWwindow* m_glfwWindow = nullptr;

		Shader* m_basic_shader;
		Shader* m_voxel_shader;
		
		VertexBuffer* m_cube;
		VertexBuffer* m_floor_grid;
		VertexBuffer* m_cam_coord;
		VertexBuffer* m_w_coord;
		VertexBuffer* m_volume;

		bool reset_cursor;
		cv::Point2f m_cursor_last_pos;

		void createCamCoord();
		void createVolume();
		void createWCoord();

		void drawWireframe(VertexBuffer*);
		void drawVoxels();

		void update();
		void draw();

	public:
		static Window& GetInstance()
		{
			static Window instance;
			return instance;

		}
		~Window();
		Window(Window const&) = delete;
		void operator=(Window const&) = delete;

		bool init(const char*, Scene3DRenderer&);
		void run();

		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void mouseCallback(GLFWwindow* window, int button, int action, int mods);
		static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		static void cursorCallback(GLFWwindow* window, double xpos, double ypos);
		static void frameBufferCallback(GLFWwindow* window, int width, int height);
		static void cursorEnterCallback(GLFWwindow* window, int entered);
		static void windowFocusCallback(GLFWwindow* window, int focused);

		Scene3DRenderer& getScene3d() const
		{
			return *m_scene3d;
		}

		SceneCamera& getSceneCam() const
		{
			return *m_scene_cam;
		}
	};
} /* namespace team45 */