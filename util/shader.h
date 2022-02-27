#pragma once

namespace team45
{
	class Shader
	{
	public:
		Shader();
		~Shader();
		// Specify filepath withouth file type.
		// We assume that .vert and .frag have the same name
		bool Load(const std::string& filepath);
		void Begin() const;
		void End() const;

		GLuint GetUniformLocation(const std::string& name) const;
		void SetInt(const char* name, const int& value) const;
		void SetFloat(const char* name, const float& value) const;
		void SetVec2(const char* name, const glm::vec2& vector) const;
		void SetVec2(const char* name, const float& x, const float& y) const;
		void SetVec3(const char* name, const glm::vec3& vector) const;
		void SetVec3(const char* name, const float& x, const float& y, const float& z) const;
		void SetVec4(const char* name, const glm::vec4& vector) const;
		void SetVec4(const char* name, const float& x, const float& y, const float& z, const float& w) const;
		void SetMat4(const char* name, const glm::mat4& matrix) const;

	private:
		GLuint CompileShader(GLenum shader_type, const std::string& file)  const;
		void LinkShader(GLuint& vert_id, GLuint& frag_id);

		GLuint m_ProgramId;
	};
}