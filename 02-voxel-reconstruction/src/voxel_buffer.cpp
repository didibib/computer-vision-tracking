#include "cvpch.h"
#include "voxel_buffer.h"
#include "cube.h"

namespace team45
{
	VoxelBuffer::VoxelBuffer() :
		m_nVertices(0), m_VAO(0), m_VBO(0), m_Name("")
	{
	}

	VoxelBuffer::~VoxelBuffer()
	{
		glDeleteVertexArrays(1, &m_VAO);
	}

	void VoxelBuffer::Create(int voxel_amount)
	{
		auto vertices = Cube::GetVertices();

		m_nVertices = vertices.size();

		glGenBuffers(1, &m_IVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_IVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Voxel) * voxel_amount, NULL, GL_STREAM_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Generate buffers
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		// Bind VAO
		glBindVertexArray(m_VAO);
		// Bind VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_nVertices, &vertices[0], GL_STATIC_DRAW);

		// Set attributes
		// Vertex (location = 0)
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
		// Color (location = 1)
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, m_IVBO);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Voxel), (void*)offsetof(Voxel, color));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribDivisor(1, 1);
		// Offset (location = 2)
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, m_IVBO);
		glVertexAttribIPointer(2, 3, GL_INT, sizeof(Voxel), (void*)offsetof(Voxel, position));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glVertexAttribDivisor(2, 1);

		// Unbind buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored
		glBindVertexArray(0);
	}

	void VoxelBuffer::Draw(const std::vector<Voxel>& voxels) const
	{
		if (voxels.empty()) return;
		glBindBuffer(GL_ARRAY_BUFFER, m_IVBO);
		void* buffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		memcpy(buffer, voxels.data(), sizeof(voxels));
		glUnmapBuffer(GL_ARRAY_BUFFER);

		glBindVertexArray(m_VAO);

		glDrawArraysInstanced(GL_TRIANGLES, 0, m_nVertices, voxels.size());

		glBindVertexArray(0);
	}
}