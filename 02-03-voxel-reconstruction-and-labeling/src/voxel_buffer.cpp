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

		// Generate buffers
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_IVBO);
		// Bind VAO
		glBindVertexArray(m_VAO);
		// Bind VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_nVertices, &vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Set attributes
		// Vertex (location = 0)
		/*glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
		*/// Color (location = 1)

		// Bind instance VBO
		glBindBuffer(GL_ARRAY_BUFFER, m_IVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VoxelGPU) * voxel_amount, NULL, GL_STREAM_DRAW);

		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Voxel), (void*)offsetof(Voxel, color));
		//// Offset (location = 2)
		//glEnableVertexAttribArray(2);
		//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Voxel), (void*)offsetof(Voxel, position));

		// Unbind buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored
		glBindVertexArray(0);
	}

	void VoxelBuffer::Draw(const std::vector<VoxelGPU>& voxels) const
	{
		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
		glVertexAttribDivisor(0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_IVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(VoxelGPU) * voxels.size(), voxels.data(), GL_STREAM_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelGPU), (void*)offsetof(VoxelGPU, color));
		glVertexAttribDivisor(1, 1);

		std::size_t vec4Size = sizeof(glm::vec4);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VoxelGPU), (void*)offsetof(VoxelGPU, model));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(VoxelGPU), (void*)(offsetof(VoxelGPU, model) + (1 * vec4Size)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VoxelGPU), (void*)(offsetof(VoxelGPU, model) + (2 * vec4Size)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(VoxelGPU), (void*)(offsetof(VoxelGPU, model) + (3 * vec4Size)));

		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);

		glDrawArraysInstanced(GL_TRIANGLES, 0, m_nVertices, voxels.size());

		glBindVertexArray(0);
	}
}