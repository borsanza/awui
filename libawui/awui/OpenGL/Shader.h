#pragma once

#include <string>

typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace awui::OpenGL {
	class Shader {
	  private:
		GLuint m_gProgramID;

		void printShaderLog(GLuint shader);

	  public:
		Shader();
		~Shader() = default;

		GLuint LoadShaderFromFile(std::string path, GLenum shaderType);
	};
} // namespace awui::OpenGL
