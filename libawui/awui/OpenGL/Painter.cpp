/*
 * awui/OpenGL/Painter.cpp
 *
 * Copyright (C) 2026 Borja Sánchez Zamorano
 */

#include "Painter.h"

#include <awui/Console.h>
#include <awui/Drawing/Color.h>
#include <awui/String.h>

#include <SDL.h>
#include <SDL_opengl.h>

#include <cmath>
#include <cstddef>
#include <cstring>

using namespace awui::OpenGL;
using awui::Drawing::Color;

namespace {
	// Funciones de OpenGL 2.0 en adelante. Se cargan con SDL al inicializar: en Windows opengl32 solo exporta las de
	// 1.1, y en OpenGL ES (y con EGL) no hay GLEW
	PFNGLCREATESHADERPROC p_glCreateShader;
	PFNGLSHADERSOURCEPROC p_glShaderSource;
	PFNGLCOMPILESHADERPROC p_glCompileShader;
	PFNGLGETSHADERIVPROC p_glGetShaderiv;
	PFNGLGETSHADERINFOLOGPROC p_glGetShaderInfoLog;
	PFNGLDELETESHADERPROC p_glDeleteShader;
	PFNGLCREATEPROGRAMPROC p_glCreateProgram;
	PFNGLATTACHSHADERPROC p_glAttachShader;
	PFNGLBINDATTRIBLOCATIONPROC p_glBindAttribLocation;
	PFNGLLINKPROGRAMPROC p_glLinkProgram;
	PFNGLGETPROGRAMIVPROC p_glGetProgramiv;
	PFNGLGETPROGRAMINFOLOGPROC p_glGetProgramInfoLog;
	PFNGLUSEPROGRAMPROC p_glUseProgram;
	PFNGLGETUNIFORMLOCATIONPROC p_glGetUniformLocation;
	PFNGLUNIFORMMATRIX4FVPROC p_glUniformMatrix4fv;
	PFNGLUNIFORM1IPROC p_glUniform1i;
	PFNGLGENBUFFERSPROC p_glGenBuffers;
	PFNGLBINDBUFFERPROC p_glBindBuffer;
	PFNGLBUFFERDATAPROC p_glBufferData;
	PFNGLGENVERTEXARRAYSPROC p_glGenVertexArrays;
	PFNGLBINDVERTEXARRAYPROC p_glBindVertexArray;
	PFNGLVERTEXATTRIBPOINTERPROC p_glVertexAttribPointer;
	PFNGLENABLEVERTEXATTRIBARRAYPROC p_glEnableVertexAttribArray;
	PFNGLACTIVETEXTUREPROC p_glActiveTexture;

	template <typename T>
	bool Load(T &function, const char *name) {
		function = (T) SDL_GL_GetProcAddress(name);
		if (!function)
			awui::Console::Error->WriteLine(awui::String("Painter: falta la función de OpenGL ") + name);
		return function != nullptr;
	}

	// El mismo código para OpenGL 3.3 y OpenGL ES 3.0: solo cambia la primera línea (y la precisión en ES)
	const char *VertexShader = R"(
uniform mat4 u_projection;
layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec4 a_color;
out vec2 v_texCoord;
out vec4 v_color;

void main() {
	v_texCoord = a_texCoord;
	v_color = a_color;
	gl_Position = u_projection * vec4(a_position, 0.0, 1.0);
}
)";

	// u_mode: 0 solo color, 1 textura RGBA, 2 textura BGRA (se reordena aquí), 3 línea suavizada (v_texCoord.x:
	// medio grosor; v_texCoord.y: distancia al centro de la línea, en píxeles)
	const char *FragmentShader = R"(
uniform sampler2D u_texture;
uniform int u_mode;
in vec2 v_texCoord;
in vec4 v_color;
out vec4 fragColor;

void main() {
	vec4 color = v_color;
	if (u_mode == 3) {
		color.a *= clamp(v_texCoord.x + 0.5 - abs(v_texCoord.y), 0.0, 1.0);
	} else if (u_mode != 0) {
		vec4 texel = texture(u_texture, v_texCoord);
		if (u_mode == 2)
			texel = texel.bgra;
		color *= texel;
	}
	fragColor = color;
}
)";

	Painter::Vertex MakeVertex(float x, float y, float u, float v, const Color &color) {
		return {x, y, u, v, color.GetR(), color.GetG(), color.GetB(), color.GetA()};
	}
} // namespace

Painter::Painter() {
	m_initialized = false;
	m_failed = false;
	m_es = false;
	m_legacy = false;
	m_program = 0;
	m_vertexArray = 0;
	m_vertexBuffer = 0;
	m_projectionLocation = -1;
	m_modeLocation = -1;
	m_textureLocation = -1;
	m_offsetX = 0.0f;
	m_offsetY = 0.0f;
	SetOrtho(0.0f, 1.0f, 1.0f, 0.0f);
}

Painter &Painter::Instance() {
	static Painter painter;
	return painter;
}

GLuint Painter::CompileShader(unsigned int type, const char *source) {
	// La cabecera según el contexto: GLSL 3.30 de escritorio o GLSL ES 3.00
	// Precisión alta en ES (3.0 la garantiza también en el shader de fragmentos): con la media, las posiciones en una
	// pantalla de más de mil píxeles se van un píxel
	const char *header = m_es ? "#version 300 es\nprecision highp float;\n" : "#version 330 core\n";
	const char *sources[] = {header, source};

	GLuint shader = p_glCreateShader(type);
	p_glShaderSource(shader, 2, sources, nullptr);
	p_glCompileShader(shader);

	GLint compiled = GL_FALSE;
	p_glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (compiled != GL_TRUE) {
		char log[1024] = "";
		p_glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
		Console::Error->WriteLine(String("Painter: no compila el shader: ") + log);
		p_glDeleteShader(shader);
		return 0;
	}

	return shader;
}

bool Painter::Initialize() {
	if (m_initialized || m_failed)
		return m_initialized;

	m_failed = true; // Hasta que todo vaya bien: si falla, no se vuelve a intentar en cada dibujo

	bool loaded = Load(p_glCreateShader, "glCreateShader") & Load(p_glShaderSource, "glShaderSource") &
				  Load(p_glCompileShader, "glCompileShader") & Load(p_glGetShaderiv, "glGetShaderiv") &
				  Load(p_glGetShaderInfoLog, "glGetShaderInfoLog") & Load(p_glDeleteShader, "glDeleteShader") &
				  Load(p_glCreateProgram, "glCreateProgram") & Load(p_glAttachShader, "glAttachShader") &
				  Load(p_glBindAttribLocation, "glBindAttribLocation") & Load(p_glLinkProgram, "glLinkProgram") &
				  Load(p_glGetProgramiv, "glGetProgramiv") & Load(p_glGetProgramInfoLog, "glGetProgramInfoLog") &
				  Load(p_glUseProgram, "glUseProgram") & Load(p_glGetUniformLocation, "glGetUniformLocation") &
				  Load(p_glUniformMatrix4fv, "glUniformMatrix4fv") & Load(p_glUniform1i, "glUniform1i") &
				  Load(p_glGenBuffers, "glGenBuffers") & Load(p_glBindBuffer, "glBindBuffer") &
				  Load(p_glBufferData, "glBufferData") & Load(p_glGenVertexArrays, "glGenVertexArrays") &
				  Load(p_glBindVertexArray, "glBindVertexArray") & Load(p_glVertexAttribPointer, "glVertexAttribPointer") &
				  Load(p_glEnableVertexAttribArray, "glEnableVertexAttribArray") & Load(p_glActiveTexture, "glActiveTexture");
	if (!loaded)
		return false;

	const char *version = (const char *) glGetString(GL_VERSION);
	m_es = version && strstr(version, "OpenGL ES");

	// Contexto de compatibilidad: puede haber pintado antiguo alrededor (ver Draw)
	m_legacy = false;
	if (!m_es) {
		GLint profile = 0;
		glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
		m_legacy = (profile & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) != 0;
	}

	GLuint vertex = CompileShader(GL_VERTEX_SHADER, VertexShader);
	GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, FragmentShader);
	if (!vertex || !fragment)
		return false;

	m_program = p_glCreateProgram();
	p_glAttachShader(m_program, vertex);
	p_glAttachShader(m_program, fragment);
	p_glLinkProgram(m_program);
	p_glDeleteShader(vertex);
	p_glDeleteShader(fragment);

	GLint linked = GL_FALSE;
	p_glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
	if (linked != GL_TRUE) {
		char log[1024] = "";
		p_glGetProgramInfoLog(m_program, sizeof(log), nullptr, log);
		Console::Error->WriteLine(String("Painter: no enlaza el programa: ") + log);
		return false;
	}

	m_projectionLocation = p_glGetUniformLocation(m_program, "u_projection");
	m_modeLocation = p_glGetUniformLocation(m_program, "u_mode");
	m_textureLocation = p_glGetUniformLocation(m_program, "u_texture");

	// Formato de los vértices: posición, coordenada de textura y color (bytes, de 0 a 1 en el shader)
	GLint previousArray = 0;
	GLint previousBuffer = 0;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousArray);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousBuffer);

	p_glGenVertexArrays(1, &m_vertexArray);
	p_glGenBuffers(1, &m_vertexBuffer);
	p_glBindVertexArray(m_vertexArray);
	p_glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	p_glEnableVertexAttribArray(0);
	p_glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, x));
	p_glEnableVertexAttribArray(1);
	p_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, u));
	p_glEnableVertexAttribArray(2);
	p_glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const void *) offsetof(Vertex, r));

	p_glBindVertexArray(previousArray);
	p_glBindBuffer(GL_ARRAY_BUFFER, previousBuffer);

	m_initialized = true;
	m_failed = false;
	return true;
}

void Painter::SetOrtho(float left, float right, float bottom, float top) {
	// Como glOrtho con near -1 y far 1, por columnas
	memset(m_projection, 0, sizeof(m_projection));
	m_projection[0] = 2.0f / (right - left);
	m_projection[5] = 2.0f / (top - bottom);
	m_projection[10] = -1.0f;
	m_projection[12] = -(right + left) / (right - left);
	m_projection[13] = -(top + bottom) / (top - bottom);
	m_projection[15] = 1.0f;
}

void Painter::SetOffset(float x, float y) {
	m_offsetX = x;
	m_offsetY = y;
}

void Painter::DrawTriangles(const Vertex *vertices, int count, GLuint texture, TextureFormat format, Blend blend) {
	Draw(vertices, count, texture ? ((format == TextureFormat::BGRA) ? 2 : 1) : 0, texture, blend);
}

void Painter::DrawLines(const std::vector<Vertex> &vertices) {
	Draw(vertices.data(), (int) vertices.size(), 3, 0, Blend::Normal);
}

void Painter::Draw(const Vertex *vertices, int count, int mode, GLuint texture, Blend blend) {
	if ((count < 3) || !Initialize())
		return;

	// Con el contexto de compatibilidad convive con el pintado antiguo (el 3D de gameOfBlocks): se guarda lo que se
	// cambia para dejarlo como estaba. En un contexto moderno solo pinta él, y consultar el estado en cada dibujo
	// sería tiempo perdido
	GLint previousProgram = 0;
	GLint previousArray = 0;
	GLint previousBuffer = 0;
	GLint previousTexture = 0;
	GLint previousSource = 0;
	GLint previousDestination = 0;
	GLboolean previousBlend = GL_FALSE;
	GLboolean previousDepth = GL_FALSE;
	GLboolean previousCull = GL_FALSE;
	if (m_legacy) {
		glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram);
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousArray);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousBuffer);
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousTexture);
		glGetIntegerv(GL_BLEND_SRC_RGB, &previousSource);
		glGetIntegerv(GL_BLEND_DST_RGB, &previousDestination);
		previousBlend = glIsEnabled(GL_BLEND);
		previousDepth = glIsEnabled(GL_DEPTH_TEST);
		previousCull = glIsEnabled(GL_CULL_FACE);
	}

	// La proyección con el desplazamiento ya aplicado
	float projection[16];
	memcpy(projection, m_projection, sizeof(projection));
	projection[12] += m_projection[0] * m_offsetX;
	projection[13] += m_projection[5] * m_offsetY;

	p_glUseProgram(m_program);
	p_glUniformMatrix4fv(m_projectionLocation, 1, GL_FALSE, projection);
	p_glUniform1i(m_modeLocation, mode);
	p_glUniform1i(m_textureLocation, 0);
	p_glActiveTexture(GL_TEXTURE0);
	if (texture)
		glBindTexture(GL_TEXTURE_2D, texture);

	// En 2D no hay caras traseras: los triángulos de una línea salen en un sentido u otro según su dirección
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	if (blend == Blend::Premultiplied)
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	else
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	p_glBindVertexArray(m_vertexArray);
	p_glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	p_glBufferData(GL_ARRAY_BUFFER, count * sizeof(Vertex), vertices, GL_STREAM_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, count);

	if (!m_legacy)
		return;

	p_glBindVertexArray(previousArray);
	p_glBindBuffer(GL_ARRAY_BUFFER, previousBuffer);
	p_glUseProgram(previousProgram);
	glBindTexture(GL_TEXTURE_2D, previousTexture);
	glBlendFunc(previousSource, previousDestination);
	if (!previousBlend)
		glDisable(GL_BLEND);
	if (previousDepth)
		glEnable(GL_DEPTH_TEST);
	if (previousCull)
		glEnable(GL_CULL_FACE);
}

void Painter::AddQuad(std::vector<Vertex> &vertices, float x1, float y1, float x2, float y2, float u1, float v1, float u2, float v2, const Color &color) {
	AddQuad(vertices, x1, y1, x2, y2, u1, v1, u2, v2, color, color, color, color);
}

void Painter::AddQuad(std::vector<Vertex> &vertices, float x1, float y1, float x2, float y2, float u1, float v1, float u2, float v2, const Color &topLeft, const Color &topRight, const Color &bottomLeft, const Color &bottomRight) {
	Vertex a = MakeVertex(x1, y1, u1, v1, topLeft);
	Vertex b = MakeVertex(x2, y1, u2, v1, topRight);
	Vertex c = MakeVertex(x1, y2, u1, v2, bottomLeft);
	Vertex d = MakeVertex(x2, y2, u2, v2, bottomRight);
	vertices.insert(vertices.end(), {a, c, b, b, c, d});
}

void Painter::AddLine(std::vector<Vertex> &vertices, float x1, float y1, float x2, float y2, float width, const Color &color) {
	AddSegment(vertices, x1, y1, x2, y2, width, 0.0f, 0.0f, color);
}

void Painter::AddPolyline(std::vector<Vertex> &vertices, const std::vector<float> &points, float width, const Color &color) {
	size_t count = points.size() / 2;
	for (size_t i = 0; i + 1 < count; i++) {
		float extendStart = (i > 0) ? width * 0.5f : 0.0f;
		float extendEnd = (i + 2 < count) ? width * 0.5f : 0.0f;
		AddSegment(vertices, points[i * 2], points[i * 2 + 1], points[i * 2 + 2], points[i * 2 + 3], width, extendStart, extendEnd, color);
	}
}

void Painter::AddSegment(std::vector<Vertex> &vertices, float x1, float y1, float x2, float y2, float width, float extendStart, float extendEnd, const Color &color) {
	float dx = x2 - x1;
	float dy = y2 - y1;
	float length = std::sqrt(dx * dx + dy * dy);
	if (length <= 0.0f)
		return;

	// Dirección y perpendicular. El rectángulo es un píxel más ancho por cada lado, para el suavizado
	float half = width * 0.5f;
	float extent = half + 1.0f;
	float ux = dx / length;
	float uy = dy / length;
	float nx = -uy * extent;
	float ny = ux * extent;
	float sx1 = x1 - ux * extendStart;
	float sy1 = y1 - uy * extendStart;
	float sx2 = x2 + ux * extendEnd;
	float sy2 = y2 + uy * extendEnd;

	// u: medio grosor; v: distancia al centro (el shader la interpola y saca la opacidad)
	Vertex a = MakeVertex(sx1 + nx, sy1 + ny, half, extent, color);
	Vertex b = MakeVertex(sx1 - nx, sy1 - ny, half, -extent, color);
	Vertex c = MakeVertex(sx2 + nx, sy2 + ny, half, extent, color);
	Vertex d = MakeVertex(sx2 - nx, sy2 - ny, half, -extent, color);
	vertices.insert(vertices.end(), {a, b, c, c, b, d});
}

void Painter::FillRectangle(int x1, int y1, int x2, int y2, const Color &color) {
	m_quad.clear();
	AddQuad(m_quad, (float) x1, (float) y1, x2 + 1.0f, y2 + 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, color);
	DrawTriangles(m_quad);
}

void Painter::DrawTexture(GLuint texture, TextureFormat format, Blend blend, float x, float y, float width, float height, float opacity) {
	// Con color premultiplicado, la opacidad se aplica igual a color y alfa
	uint8_t level = (uint8_t) std::lround(opacity * 255.0f);
	Color color = (blend == Blend::Premultiplied) ? Color::FromArgb(level, level, level, level) : Color::FromArgb(level, 255, 255, 255);

	m_quad.clear();
	AddQuad(m_quad, x, y, x + width, y + height, 0.0f, 0.0f, 1.0f, 1.0f, color);
	DrawTriangles(m_quad, texture, format, blend);
}
