#pragma once

namespace nl_uu_science_gmt
{
	class Scene3DRenderer;

	class Window
	{
		Scene3DRenderer& m_scene3d;

		static Window* m_Window;
		GLFWwindow* m_glfwWindow = nullptr;

		static void drawGrdGrid();
		static void drawCamCoord();
		static void drawVolume();
		//static void drawArcball(); was not supported on win
		static void drawVoxels();
		static void drawWCoord();
		//static void drawInfo(); was not supported on win

		static inline void perspectiveGL(GLdouble, GLdouble, GLdouble, GLdouble);

	public:
		Window(Scene3DRenderer&);
		virtual ~Window();

#ifdef _WIN32
		bool init(const char*);
		void run();
#endif

		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void mouseCallback(GLFWwindow* window, int button, int action, int mods);
		static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		static void cursorCallback(GLFWwindow* window, double xpos, double ypos);
		static void motion(int, int);
		static void reshape(int, int);
		static void reset();
		static void draw();
		static void update(int);
		static void quit();

		Scene3DRenderer& getScene3d() const
		{
			return m_scene3d;
		}
	};

} /* namespace nl_uu_science_gmt */

