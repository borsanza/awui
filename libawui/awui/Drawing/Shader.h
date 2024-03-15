#pragma once

#include <awui/Object.h>
#include <string>

typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace awui::Drawing {
	class Shader : public Object {
	  private:
		GLuint m_gProgramID;

		void printShaderLog(GLuint shader);

	  public:
		Shader();
		virtual ~Shader() = default;

		virtual bool IsClass(Classes objectClass) const override;
		GLuint LoadShaderFromFile(std::string path, GLenum shaderType);
	};
} // namespace awui::Drawing
