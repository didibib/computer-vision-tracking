#include "cvpch.h"
#include "vertex_buffer.h"
#include "cube.h"
#include "shader.h"

namespace team45
{
	Cube::Cube() :
		m_position(0.f, 0.f, 0.f), m_worldUp(0.f, 1.f, 0.f), m_model(1.f)
	{
		m_vb.Create(GetVertices());
	}

	void Cube::draw(const Shader& shader)
	{
		shader.SetMat4("u_Model", m_model);

		m_vb.Bind();
		m_vb.Draw();
		m_vb.Unbind();
	}
}
