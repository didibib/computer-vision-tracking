#pragma once

namespace team45
{
	class Shader;
	class Cube
	{
	public:
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