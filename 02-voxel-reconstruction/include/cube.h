#pragma once

namespace team45
{
	class Shader;
	class Cube
	{
	public:
		Cube();
		void draw(const Shader& shader);
		void setPos(float& x, float& y, float& z) { m_position = glm::vec3(x, y, z); };
		const glm::vec3& getPos() { return m_position; }

	private:
		VertexBuffer m_vb;
		glm::vec3 m_position;
		glm::vec3 m_worldUp;
		glm::mat4 m_model;

		static const std::vector<Vertex> GetVertices()
		{
			// Order of vertices of a face
			//  2 ___3,4
			//	 |  /|
			//	 | / |
			//   |/__|
			// 1,6    5

			static std::vector<Vertex> vertices = {
				// Front
				{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, -1.f)},
				{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(0.f, 1.f), glm::vec3(0.f, 0.f, -1.f)},
				{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 0.f, -1.f)},
				{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 0.f, -1.f)},
				{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)},
				{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, -1.f)},
				// Back																, 
				{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f,  1.f)},
				{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(0.f, 1.f), glm::vec3(0.f, 0.f,  1.f)},
				{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 0.f,  1.f)},
				{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 0.f,  1.f)},
				{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, 0.f,  1.f)},
				{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f,  1.f)},
				// Left																, 
				{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(-1.f, 0.f, 0.f)},
				{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(0.f, 1.f), glm::vec3(-1.f, 0.f, 0.f)},
				{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(-1.f, 0.f, 0.f)},
				{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(-1.f, 0.f, 0.f)},
				{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(1.f, 0.f), glm::vec3(-1.f, 0.f, 0.f)},
				{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(-1.f, 0.f, 0.f)},
				// Right
				{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(1.f, 0.f, 0.f)},
				{glm::vec3(0.5f,  0.5f,  0.5f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(0.f, 1.f), glm::vec3(1.f, 0.f, 0.f)},
				{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(1.f, 0.f, 0.f)},
				{glm::vec3(0.5f,  0.5f, -0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(1.f, 0.f, 0.f)},
				{glm::vec3(0.5f, -0.5f, -0.5f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(1.f, 0.f), glm::vec3(1.f, 0.f, 0.f)},
				{glm::vec3(0.5f, -0.5f,  0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(1.f, 0.f, 0.f)},
				// Top
				{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)},
				{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(0.f, 1.f), glm::vec3(0.f, 1.f, 0.f)},
				{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 1.f, 0.f)},
				{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 1.f, 0.f)},
				{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, 1.f, 0.f)},
				{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)},
				// Bottom
				{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)},
				{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(0.f, 1.f), glm::vec3(0.f, -1.f, 0.f)},
				{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, -1.f, 0.f)},
				{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, -1.f, 0.f)},
				{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec4(1.f, 1.f, 1.f, 1.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, -1.f, 0.f)},
				{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)},
			};
			return vertices;
		}

	};
}