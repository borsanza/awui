#include "Shader.h"

#include <GL/glew.h>
#include <awui/OpenGL/GL.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace awui::OpenGL;
using namespace awui::GOB::Engine;

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
	// 1. Recuperar el código fuente del archivo
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// Asegura que los objetos ifstream puedan lanzar excepciones:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		// Abre los archivos
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		// Lee el contenido del archivo buffer en los streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		// Cierra los archivos
		vShaderFile.close();
		fShaderFile.close();

		// Convierte los stream en strings
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	} catch (std::ifstream::failure &e) {
		std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
	}

	const char *vShaderCode = vertexCode.c_str();
	const char *fShaderCode = fragmentCode.c_str();

	// 2. Compilar shaders
	GLuint vertex, fragment;

	m_program = glCreateProgram();
	vertex = glCreateShader(GL_VERTEX_SHADER);
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	// const GLchar *vertexShaderSource[] = {"#version 140\nin vec2 LVertexPos2D; void main() { gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 ); }"};
	// const GLchar *fragmentShaderSource[] = {"#version 140\nout vec4 LFragment; void main() { LFragment = vec4( 1.0, 1.0, 1.0, 1.0 ); }"};

	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glShaderSource(fragment, 1, &fShaderCode, NULL);

	glCompileShader(vertex);
	CheckCompileErrors(vertex, "VERTEX");
	glCompileShader(fragment);
	CheckCompileErrors(fragment, "FRAGMENT");

	// 3. Vincular el programa de shader
	glAttachShader(m_program, vertex);
	glAttachShader(m_program, fragment);

	glBindAttribLocation(m_program, 0, "aPos");
	glBindAttribLocation(m_program, 1, "aTexCoord");

	glLinkProgram(m_program);
	CheckCompileErrors(m_program, "PROGRAM");

	// 4. Una vez vinculados, ya no se necesitan más los shaders
	glDetachShader(m_program, vertex);
	glDetachShader(m_program, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);

	CheckCompileErrors(m_program, "VALIDATE");
}

Shader::~Shader() {
	glDeleteProgram(m_program);
}

void Shader::Use() {
	// GL::CheckGLErrors("Shader::Use(1)");

	glUseProgram(m_program);
	GL::CheckGLErrors("Shader::Use::glUseProgram()");

	glUniform1i(glGetUniformLocation(m_program, "texture1"), 0);
}

void Shader::Unuse() {
	glUseProgram(0);
}

void Shader::CheckCompileErrors(GLuint shader, const std::string type) {
	GLint success;
	GLchar infoLog[1024];

	if (type == "VERTEX" || type == "FRAGMENT") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
		return;
	}

	if (type == "PROGRAM") {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
		return;
	}

	if (type == "VALIDATE") {
		glGetProgramiv(shader, GL_VALIDATE_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cerr << "ERROR::PROGRAM_VALIDATION_FAILED of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
		return;
	}
}
