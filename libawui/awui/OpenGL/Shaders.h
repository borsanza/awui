#pragma once

#include <SDL_opengl.h>

namespace awui::OpenGL::Shaders {
	// Funciones de OpenGL 2.0 en adelante, compartidas por todo lo que pinta con shaders (Painter, el motor 3D). Se
	// cargan con SDL en Initialize: en Windows opengl32 solo exporta las de 1.1, y en OpenGL ES (y con EGL) no hay
	// GLEW
	extern PFNGLCREATESHADERPROC CreateShader;
	extern PFNGLSHADERSOURCEPROC ShaderSource;
	extern PFNGLCOMPILESHADERPROC CompileShader;
	extern PFNGLGETSHADERIVPROC GetShaderiv;
	extern PFNGLGETSHADERINFOLOGPROC GetShaderInfoLog;
	extern PFNGLDELETESHADERPROC DeleteShader;
	extern PFNGLCREATEPROGRAMPROC CreateProgram;
	extern PFNGLATTACHSHADERPROC AttachShader;
	extern PFNGLLINKPROGRAMPROC LinkProgram;
	extern PFNGLGETPROGRAMIVPROC GetProgramiv;
	extern PFNGLGETPROGRAMINFOLOGPROC GetProgramInfoLog;
	extern PFNGLUSEPROGRAMPROC UseProgram;
	extern PFNGLGETUNIFORMLOCATIONPROC GetUniformLocation;
	extern PFNGLUNIFORMMATRIX4FVPROC UniformMatrix4fv;
	extern PFNGLUNIFORM1IPROC Uniform1i;
	extern PFNGLGENBUFFERSPROC GenBuffers;
	extern PFNGLBINDBUFFERPROC BindBuffer;
	extern PFNGLBUFFERDATAPROC BufferData;
	extern PFNGLGENVERTEXARRAYSPROC GenVertexArrays;
	extern PFNGLBINDVERTEXARRAYPROC BindVertexArray;
	extern PFNGLVERTEXATTRIBPOINTERPROC VertexAttribPointer;
	extern PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray;
	extern PFNGLACTIVETEXTUREPROC ActiveTexture;

	// Carga las funciones (la primera vez; si falta alguna, false siempre). Necesita el contexto ya creado y activo
	bool Initialize();

	// Si el contexto es OpenGL ES (si no, de escritorio)
	bool IsES();

	// Compila y enlaza un programa. Los fuentes no llevan la línea #version: se pone la del contexto (GLSL 3.30 o
	// GLSL ES 3.00), así el mismo código vale para los dos. name sale en los errores. Devuelve 0 si falla
	GLuint BuildProgram(const char *vertexSource, const char *fragmentSource, const char *name);
} // namespace awui::OpenGL::Shaders
