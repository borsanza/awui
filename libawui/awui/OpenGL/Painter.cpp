/*
 * awui/OpenGL/Painter.cpp
 *
 * Copyright (C) 2026 Borja Sánchez Zamorano
 */

#include "Painter.h"

#include <awui/Drawing/Color.h>
#include <awui/OpenGL/Shaders.h>

#include <cmath>
#include <cstddef>
#include <cstring>

using namespace awui::OpenGL;
using awui::Drawing::Color;

namespace {
	// El mismo código para OpenGL 3.3 y OpenGL ES 3.0 (Shaders::BuildProgram pone la línea #version)
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

bool Painter::Initialize() {
	if (m_initialized || m_failed)
		return m_initialized;

	m_failed = true; // Hasta que todo vaya bien: si falla, no se vuelve a intentar en cada dibujo

	m_program = Shaders::BuildProgram(VertexShader, FragmentShader, "Painter");
	if (!m_program)
		return false;

	m_projectionLocation = Shaders::GetUniformLocation(m_program, "u_projection");
	m_modeLocation = Shaders::GetUniformLocation(m_program, "u_mode");
	m_textureLocation = Shaders::GetUniformLocation(m_program, "u_texture");

	// Formato de los vértices: posición, coordenada de textura y color (bytes, de 0 a 1 en el shader)
	Shaders::GenVertexArrays(1, &m_vertexArray);
	Shaders::GenBuffers(1, &m_vertexBuffer);
	Shaders::BindVertexArray(m_vertexArray);
	Shaders::BindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	Shaders::EnableVertexAttribArray(0);
	Shaders::VertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, x));
	Shaders::EnableVertexAttribArray(1);
	Shaders::VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *) offsetof(Vertex, u));
	Shaders::EnableVertexAttribArray(2);
	Shaders::VertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (const void *) offsetof(Vertex, r));

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

	// La proyección con el desplazamiento ya aplicado
	float projection[16];
	memcpy(projection, m_projection, sizeof(projection));
	projection[12] += m_projection[0] * m_offsetX;
	projection[13] += m_projection[5] * m_offsetY;

	Shaders::UseProgram(m_program);
	Shaders::UniformMatrix4fv(m_projectionLocation, 1, GL_FALSE, projection);
	Shaders::Uniform1i(m_modeLocation, mode);
	Shaders::Uniform1i(m_textureLocation, 0);
	Shaders::ActiveTexture(GL_TEXTURE0);
	if (texture)
		glBindTexture(GL_TEXTURE_2D, texture);

	// En 2D no hay caras traseras (los triángulos de una línea salen en un sentido u otro según su dirección) ni
	// profundidad
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	if (blend == Blend::Premultiplied)
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	else
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shaders::BindVertexArray(m_vertexArray);
	Shaders::BindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	Shaders::BufferData(GL_ARRAY_BUFFER, count * sizeof(Vertex), vertices, GL_STREAM_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, count);
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
