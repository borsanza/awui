/**
 * awui/Drawing/Shader.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

// TODO: Estoy experimentando con shaders, pero es un desastre aun

#include "Shader.h"

#include <fstream>
#include <GL/glew.h>

using namespace awui::Drawing;

Shader::Shader() {
	glewInit();
	this->_gProgramID = glCreateProgram();
	GLuint fragmentShader = this->LoadShaderFromFile("shader.glfs", GL_FRAGMENT_SHADER);
	glAttachShader(this->_gProgramID, fragmentShader);
	glLinkProgram(this->_gProgramID);
	glUseProgram(this->_gProgramID);
	//int texcoord_index = glGetAttribLocation(this->_gProgramID, "in_coord");
}

Shader::~Shader() {
}

int Shader::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Shader)
		return 1;

	return Object::IsClass(objectClass);
}

void Shader::printShaderLog(GLuint shader) {
	if (glIsShader(shader)) {
		int infoLogLength = 0;
		int maxLength = infoLogLength;

		glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );

		char* infoLog = new char[ maxLength ];

		glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
		if (infoLogLength > 0)
			printf("%s\n", infoLog);

		delete[] infoLog;
	} else
		printf( "Name %d is not a shader\n", (int) shader );
}

GLuint Shader::LoadShaderFromFile( std::string path, GLenum shaderType) {
	GLuint shaderID = 0;
	std::string shaderString;
	std::ifstream sourceFile( path.c_str() );

	if (sourceFile) {
		shaderString.assign((std::istreambuf_iterator< char >(sourceFile)), std::istreambuf_iterator< char >());
		shaderID = glCreateShader(shaderType);

		const GLchar* shaderSource = shaderString.c_str();
		glShaderSource(shaderID, 1, (const GLchar**)&shaderSource, NULL);

		glCompileShader(shaderID);

		GLint shaderCompiled = GL_FALSE;
		glGetShaderiv( shaderID, GL_COMPILE_STATUS, &shaderCompiled );
		if (shaderCompiled != GL_TRUE) {
			printf("Unable to compile shader %d!\n\nSource:\n%s\n", (int) shaderID, shaderSource);
			printShaderLog(shaderID);
			glDeleteShader(shaderID);
			shaderID = 0;
		}
	}
	else
		printf( "Unable to open file %s\n", path.c_str() );

	return shaderID;
}
