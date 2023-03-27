#include "cvpch.h"
#include "scene_camera.h"

namespace team45
{
	SceneCamera::SceneCamera()
		: m_Yaw(-90), m_Pitch(0), 
		m_Forward(0.f, -1.f, 0.f), m_Up(0.f, 0.f, 1.f), m_Right(1.f, 0.f, 0.f), m_WorldUp(0.f, 0.f, 1.f)
	{
	}
	void SceneCamera::Init(const float& fov, const int& viewWidth, const int& viewHeight, const float& nearPlane, const float& farPlane)
		// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector 
		// pointing to the right so we initially rotate a bit to the left.
	{
		m_Fov = fov;
		m_ViewWidth = viewWidth;
		m_ViewHeight = viewHeight;
		m_Near = nearPlane;
		m_Far = farPlane;

		m_Projection = glm::perspective(glm::radians(m_Fov), (float)m_ViewWidth / (float)m_ViewHeight, m_Near, m_Far);
		UpdateCameraVectors();
	}

	void SceneCamera::Move(const Direction& dir, const float& deltaTime)
	{
		float velocity = m_MoveSpeed * deltaTime;
		switch (dir)
		{
		case Direction::Forward:
			m_Position += m_Forward * velocity;
			break;
		case Direction::Backward:
			m_Position -= m_Forward * velocity;
			break;
		case Direction::Right:
			m_Position += m_Right * velocity;
			break;
		case Direction::Left:
			m_Position -= m_Right * velocity;
			break;
		case Direction::Up:
			m_Position += m_Up * velocity;
			break;
		case Direction::Down:
			m_Position -= m_Up * velocity;
			break;
		default:
			break;
		}
		UpdateCameraVectors();
	}

	void SceneCamera::Cursor(float xOffset, float yOffset)
	{
		xOffset *= m_MouseSensivity;
		yOffset *= m_MouseSensivity;
		m_Yaw = glm::mod(m_Yaw + xOffset, 360.f);
		m_Pitch += yOffset;
		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		m_Pitch = glm::clamp(m_Pitch, -89.f, 89.f);
		UpdateCameraVectors();
	}

	void SceneCamera::UpdateCameraVectors()
	{
		// Calculate the new mForward vector
		glm::vec3 forward;
		forward.x = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		forward.y = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		forward.z = sin(glm::radians(m_Pitch));
		m_Forward = glm::normalize(forward);
		// Also re-calculate the Right and Up vector
		// normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		m_Right = glm::normalize(glm::cross(m_Forward, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Forward));
		m_View = glm::lookAt(m_Position, m_Position + m_Forward, m_Up);
	}

	void SceneCamera::OnWindowResize(const int& width, const int& height)
	{
		if (width > 0 && height > 0)
		{
			m_ViewWidth = width;
			m_ViewHeight = height;
			m_Projection = glm::perspective(glm::radians(m_Fov), (float)m_ViewWidth / (float)m_ViewHeight, m_Near, m_Far);
		}
	}

	const glm::vec3& SceneCamera::GetPos()
	{
		return m_Position;
	}

	void SceneCamera::SetPos(glm::vec3 const& pos)
	{
		m_Position = pos;
	}

	void SceneCamera::SetPos(float const& x, float const& y, float const& z)
	{
		m_Position.x = x;
		m_Position.y = y;
		m_Position.z = z;
		UpdateCameraVectors();
	}

	void SceneCamera::RotateAroundPoint(glm::vec3 target, float radius, float speed)
	{
		float angle = glfwGetTime() * speed;
		float camX = cos(angle) * radius - sin(angle) * radius;
		float camY = sin(angle) * radius + cos(angle) * radius;
		m_View = glm::lookAt(glm::vec3(camX, camY, m_Position.z), target, m_Up);
	}

	void SceneCamera::Reset(bool rotate_camera)
	{
		m_Forward = glm::vec3(0.f, -1.f, 0.f);
		m_Up = glm::vec3(0.f, 0.f, 1.f);
		m_Right = glm::vec3(1.f, 0.f, 0.f);
		if (!rotate_camera) UpdateCameraVectors();
	}

	void SceneCamera::Zoom(float offset)
	{
		m_Fov -= offset;
		if (m_Fov < 1.0f)
			m_Fov = 1.0f;
		if (m_Fov > 60.f)
			m_Fov = 60.0f;
	}

	const glm::mat4& SceneCamera::GetProjMatrix()
	{
		return m_Projection = glm::perspective(glm::radians(m_Fov), (float)m_ViewWidth / (float)m_ViewHeight, m_Near, m_Far);
	}

	glm::mat4 SceneCamera::GetViewMatrix()
	{
		return m_View;
	}
}