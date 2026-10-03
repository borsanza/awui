/*
 * awui/OpenGL/Shaders.cpp
 *
 * Copyright (C) 2026 Borja Sánchez Zamorano
 */

#include "Shaders.h"

#include <awui/Console.h>
#include <awui/String.h>

#include <SDL.h>

#include <cstring>

namespace awui::OpenGL::Shaders {
	PFNGLCREATESHADERPROC CreateShader;
	PFNGLSHADERSOURCEPROC ShaderSource;
	PFNGLCOMPILESHADERPROC CompileShader;
	PFNGLGETSHADERIVPROC GetShaderiv;
	PFNGLGETSHADERINFOLOGPROC GetShaderInfoLog;
	PFNGLDELETESHADERPROC DeleteShader;
	PFNGLCREATEPROGRAMPROC CreateProgram;
	PFNGLATTACHSHADERPROC AttachShader;
	PFNGLLINKPROGRAMPROC LinkProgram;
	PFNGLGETPROGRAMIVPROC GetProgramiv;
	PFNGLGETPROGRAMINFOLOGPROC GetProgramInfoLog;
	PFNGLUSEPROGRAMPROC UseProgram;
	PFNGLGETUNIFORMLOCATIONPROC GetUniformLocation;
	PFNGLUNIFORMMATRIX4FVPROC UniformMatrix4fv;
	PFNGLUNIFORM1IPROC Uniform1i;
	PFNGLGENBUFFERSPROC GenBuffers;
	PFNGLBINDBUFFERPROC BindBuffer;
	PFNGLBUFFERDATAPROC BufferData;
	PFNGLGENVERTEXARRAYSPROC GenVertexArrays;
	PFNGLBINDVERTEXARRAYPROC BindVertexArray;
	PFNGLVERTEXATTRIBPOINTERPROC VertexAttribPointer;
	PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray;
	PFNGLACTIVETEXTUREPROC ActiveTexture;
} // namespace awui::OpenGL::Shaders

using namespace awui;
using namespace awui::OpenGL;

namespace {
	bool s_initialized = false;
	bool s_failed = false;
	bool s_es = false;

	template <typename T>
	bool Load(T &function, const char *name) {
		function = (T) SDL_GL_GetProcAddress(name);
		if (!function)
			Console::Error->WriteLine(String("OpenGL: falta la función ") + name);
		return function != nullptr;
	}

	GLuint Compile(GLenum type, const char *source, const char *name) {
		// La cabecera según el contexto: GLSL 3.30 de escritorio o GLSL ES 3.00
		// Precisión alta en ES (3.0 la garantiza también en el shader de fragmentos): con la media, las posiciones en
		// una pantalla de más de mil píxeles se van un píxel
		const char *header = s_es ? "#version 300 es\nprecision highp float;\n" : "#version 330 core\n";
		const char *sources[] = {header, source};

		GLuint shader = Shaders::CreateShader(type);
		Shaders::ShaderSource(shader, 2, sources, nullptr);
		Shaders::CompileShader(shader);

		GLint compiled = GL_FALSE;
		Shaders::GetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (compiled != GL_TRUE) {
			char log[1024] = "";
			Shaders::GetShaderInfoLog(shader, sizeof(log), nullptr, log);
			Console::Error->WriteLine(String(name) + ": no compila el shader: " + log);
			Shaders::DeleteShader(shader);
			return 0;
		}

		return shader;
	}
} // namespace

bool Shaders::Initialize() {
	if (s_initialized || s_failed)
		return s_initialized;

	s_failed = true; // Hasta que todo vaya bien: si falla, no se vuelve a intentar en cada dibujo

	bool loaded = Load(CreateShader, "glCreateShader") & Load(ShaderSource, "glShaderSource") &
				  Load(CompileShader, "glCompileShader") & Load(GetShaderiv, "glGetShaderiv") &
				  Load(GetShaderInfoLog, "glGetShaderInfoLog") & Load(DeleteShader, "glDeleteShader") &
				  Load(CreateProgram, "glCreateProgram") & Load(AttachShader, "glAttachShader") &
				  Load(LinkProgram, "glLinkProgram") & Load(GetProgramiv, "glGetProgramiv") &
				  Load(GetProgramInfoLog, "glGetProgramInfoLog") & Load(UseProgram, "glUseProgram") &
				  Load(GetUniformLocation, "glGetUniformLocation") & Load(UniformMatrix4fv, "glUniformMatrix4fv") &
				  Load(Uniform1i, "glUniform1i") & Load(GenBuffers, "glGenBuffers") & Load(BindBuffer, "glBindBuffer") &
				  Load(BufferData, "glBufferData") & Load(GenVertexArrays, "glGenVertexArrays") &
				  Load(BindVertexArray, "glBindVertexArray") & Load(VertexAttribPointer, "glVertexAttribPointer") &
				  Load(EnableVertexAttribArray, "glEnableVertexAttribArray") & Load(ActiveTexture, "glActiveTexture");
	if (!loaded)
		return false;

	const char *version = (const char *) glGetString(GL_VERSION);
	s_es = version && strstr(version, "OpenGL ES");

	s_initialized = true;
	s_failed = false;
	return true;
}

bool Shaders::IsES() {
	return s_es;
}

GLuint Shaders::BuildProgram(const char *vertexSource, const char *fragmentSource, const char *name) {
	if (!Initialize())
		return 0;

	GLuint vertex = Compile(GL_VERTEX_SHADER, vertexSource, name);
	GLuint fragment = Compile(GL_FRAGMENT_SHADER, fragmentSource, name);
	if (!vertex || !fragment) {
		if (vertex)
			DeleteShader(vertex);
		if (fragment)
			DeleteShader(fragment);
		return 0;
	}

	GLuint program = CreateProgram();
	AttachShader(program, vertex);
	AttachShader(program, fragment);
	LinkProgram(program);
	DeleteShader(vertex);
	DeleteShader(fragment);

	GLint linked = GL_FALSE;
	GetProgramiv(program, GL_LINK_STATUS, &linked);
	if (linked != GL_TRUE) {
		char log[1024] = "";
		GetProgramInfoLog(program, sizeof(log), nullptr, log);
		Console::Error->WriteLine(String(name) + ": no enlaza el programa: " + log);
		return 0;
	}

	return program;
}
