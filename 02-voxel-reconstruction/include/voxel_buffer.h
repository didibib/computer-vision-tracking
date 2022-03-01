#pragma once

namespace team45
{
	// Provides methods for uploading vertex data
	class VoxelBuffer
	{
	public:
		VoxelBuffer();
		~VoxelBuffer();
		void Create(int voxel_amount);
		void Draw(const std::vector<Voxel>& voxels) const;
		void SetName(std::string const& name) { m_Name = name; }
		std::string const& GetName() const { return m_Name; }
	private:
		std::string m_Name;
		int m_nVertices;
		GLuint m_VAO;
		GLuint m_VBO;
		// Instanced VBO
		GLuint m_IVBO;
	};
}