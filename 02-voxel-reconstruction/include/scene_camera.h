#pragma once

namespace team45
{
	// Some usefull info about the camera matrices
	// https://gamedev.stackexchange.com/questions/178643/the-view-matrix-finally-explained
	enum class Direction { Forward, Right, Up, Backward, Left, Down };

	class SceneCamera
	{
	public:
		SceneCamera() = delete;
		SceneCamera(const float& fov, const int& viewWidth, const int& viewHeight, const float& nearPlane, const float& farPlane);

		void Move(const Direction& dir, const float& deltaTime);
		void Cursor(float xOffset, float yOffset);
		void OnWindowResize(const int& width, const int& height);

		const glm::vec3& GetPos();
		void SetPos(glm::vec3 const& pos);
		void SetPos(float const& x, float const& y, float const& z);

		const glm::mat4& GetProjMatrix();
		// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
		glm::mat4 GetViewMatrix();
		const glm::vec3& GetForward();
	private:
		void UpdateCameraVectors();
	private:
		float m_Fov;
		float m_Near;
		float m_Far;
		int m_ViewWidth;
		int m_ViewHeight;

		float m_Yaw;
		float m_Pitch;

		float m_MoveSpeed = 1000;
		float m_MouseSensivity = 0.8f;
	private:
		glm::vec3 m_Position;
		glm::vec3 m_Forward;
		glm::vec3 m_Up;
		glm::vec3 m_Right;
		glm::vec3 m_WorldUp;
		glm::mat4 m_Projection;
	};
}