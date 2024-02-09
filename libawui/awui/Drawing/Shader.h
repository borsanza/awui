#pragma once

#include <awui/Object.h>
#include <string>

typedef unsigned int GLuint;
typedef unsigned int GLenum;

namespace awui {
	namespace Drawing {
		class Shader : public Object {
			private:
				GLuint _gProgramID;

				void printShaderLog(GLuint shader);

			public:
				Shader();
				virtual ~Shader();

				virtual bool IsClass(Classes objectClass) const override;
				GLuint LoadShaderFromFile(std::string path, GLenum shaderType);
		};
	}
}
