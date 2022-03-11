#pragma once

namespace team45
{
	class SceneCamera;
	class Scene3DRenderer;
	class Cube;
	class Shader;
	class VertexBuffer;
	class VoxelBuffer;

	// https://stackoverflow.com/questions/1008019/c-singleton-design-pattern
	class Window
	{
		Window() {};

		float m_deltaTime = 0;
		bool m_paused = false;

		glm::vec4 m_clear_color;

		SceneCamera* m_scene_camera = nullptr;
		Scene3DRenderer* m_scene3d = nullptr;
		GLFWwindow* m_glfwWindow = nullptr;

		Shader* m_basic_shader = nullptr;
		Shader* m_voxel_shader = nullptr;
		
		VertexBuffer* m_cube_vb = nullptr;
		VertexBuffer* m_floor_grid_vb = nullptr;
		VertexBuffer* m_cam_coord_vb = nullptr;
		VertexBuffer* m_w_coord_vb = nullptr;
		VertexBuffer* m_volume_vb = nullptr;

		VoxelBuffer* m_voxel_buffer = nullptr;

		bool m_rotate_camera = false;
		bool m_reset_cursor = false;
		cv::Point2f m_cursor_last_pos;

		void createCamCoord();
		void createVolume();
		void createWCoord();

		void drawWireframe(VertexBuffer*);
		void drawVoxels();

		void update();
		void draw();

		std::map<int, int> m_key_lookup;
		bool isKeyRepeat(int);
		bool isKeyPressed(int);

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
			return *m_scene_camera;
		}
	};
} /* namespace team45 */