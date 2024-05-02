#pragma once

#include <string>

typedef unsigned int GLuint;

namespace awui::GOB::Engine {
	class Shader {
	  private:
		GLuint m_program;

	  public:
		Shader(const char *vertexPath, const char *fragmentPath);
		~Shader();

		GLuint GetProgram() { return m_program; }

		void Use();
		void Unuse();

	  private:
		void CheckCompileErrors(GLuint shader, std::string type);
	};
} // namespace awui::GOB::Engine