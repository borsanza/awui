#pragma once

#include <awui/Object.h>
#include <string>

typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace awui::OpenGL {
	class Shader : public Object {
	  private:
		GLuint m_gProgramID;

		void printShaderLog(GLuint shader);

	  public:
		Shader();
		virtual ~Shader() = default;

		GLuint LoadShaderFromFile(std::string path, GLenum shaderType);
	};
} // namespace awui::OpenGL
