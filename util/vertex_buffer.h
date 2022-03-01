#pragma once

namespace team45
{
	// Provides methods for uploading vertex data
	class VertexBuffer
	{
	public:
		VertexBuffer();
		~VertexBuffer();
		void Create(const std::vector<Vertex>& vertices);
		void Create(const std::vector<Vertex>& vertices, const std::vector<uint>& indices);
		void Draw(GLenum mode = GL_TRIANGLES) const;
		void Bind() const;
		void Unbind() const;		
		void SetName(std::string const& name) { m_Name = name; }
		std::string const& GetName() const { return m_Name; }
	private:
		std::string m_Name;
		mutable bool m_IsBind;
		int m_nVertices;
		int m_nIndices;
		GLuint m_VAO;
		GLuint m_VBO;
		GLuint m_EBO;
	};
}