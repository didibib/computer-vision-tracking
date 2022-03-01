#pragma once

namespace team45
{
	struct Vertex
	{
		glm::vec3 Position;		// 3 * float = 12 bytes
		glm::vec4 Color;		// 4 * float = 16 bytes
		glm::vec2 UV;			// 2 * float = 8  bytes
		glm::vec3 Normal;		// 3 * float = 12 bytes
	};

	// Provides methods for uploading vertex data
	class VertexBuffer
	{
	public:
		VertexBuffer();
		void Create(const std::vector<Vertex>& vertices);
		void Create(const std::vector<Vertex>& vertices, const std::vector<uint>& indices);
		void Draw(GLenum mode = GL_TRIANGLES) const;
		void Bind() const;
		void Unbind() const;
		std::string const& GetName() const { return m_name; }
	private:
		std::string m_name;
		mutable bool m_IsBind;
		int m_nVertices;
		int m_nIndices;
		GLuint m_VAO;
		GLuint m_VBO;
		GLuint m_EBO;
	};
}