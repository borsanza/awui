/**
 * awui/OpenGL/GL.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "GL.h"

#include <SDL_opengl.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Image.h>
#include <awui/Math.h>
#include <awui/OpenGL/Painter.h>

#include <vector>

using namespace awui::Drawing;
using namespace awui::OpenGL;

GL::GL() {
}

void GL::SetClippingBase(awui::Drawing::Rectangle rectangle) {
	m_clippingBase = rectangle;
}

void GL::SetClipping(awui::Drawing::Rectangle rectangle) {
	m_clipping = rectangle;
}

awui::Drawing::Rectangle GL::GetClippingBase() const {
	return m_clippingBase;
}

awui::Drawing::Rectangle GL::GetClipping() const {
	return m_clipping;
}

awui::Drawing::Rectangle GL::GetClippingResult() const {
	return Rectangle::Intersect(m_clippingBase, m_clipping);
}

void GL::SetClipping() {
	awui::Drawing::Rectangle rect = GetClippingResult();

	glScissor(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
}

void GL::DrawLine(int x1, int y1, int x2, int y2, const Color &color) {
	Painter &painter = Painter::Instance();

	// Horizontal o vertical: los mismos píxeles que GL_LINES, del primero al último incluidos
	if ((x1 == x2) || (y1 == y2)) {
		painter.FillRectangle(Math::Min(x1, x2), Math::Min(y1, y2), Math::Max(x1, x2), Math::Max(y1, y2), color);
		return;
	}

	std::vector<Painter::Vertex> vertices;
	Painter::AddLine(vertices, x1 + 0.5f, y1 + 0.5f, x2 + 0.5f, y2 + 0.5f, 1.0f, color);
	painter.DrawLines(vertices);
}

// El borde de un píxel de ancho, con las esquinas incluidas
void GL::DrawRectangle(int x1, int y1, int x2, int y2, const Color &color) {
	std::vector<Painter::Vertex> vertices;
	Painter::AddQuad(vertices, x1, y1, x2 + 1.0f, y1 + 1.0f, 0, 0, 0, 0, color);
	Painter::AddQuad(vertices, x1, y2, x2 + 1.0f, y2 + 1.0f, 0, 0, 0, 0, color);
	Painter::AddQuad(vertices, x1, y1 + 1.0f, x1 + 1.0f, y2, 0, 0, 0, 0, color);
	Painter::AddQuad(vertices, x2, y1 + 1.0f, x2 + 1.0f, y2, 0, 0, 0, 0, color);
	Painter::Instance().DrawTriangles(vertices);
}

void GL::FillRectangle(int x1, int y1, int x2, int y2, const Color &color) {
	Painter::Instance().FillRectangle(x1, y1, x2, y2, color);
}

void GL::DrawImageGL(awui::Drawing::Image *image, int x, int y) {
	DrawImageGL(image, x, y, image->GetWidth(), image->GetHeight());
}

// opacity: 0 transparente, 1 opaca (fundidos). El buffer de Image es de cairo (BGRA, con el color ya multiplicado
// por el alfa)
void GL::DrawImageGL(awui::Drawing::Image *image, int x, int y, int width, int height, float opacity) {
	image->Load();
	Painter::Instance().DrawTexture(image->GetTexture(), Painter::TextureFormat::BGRA, Painter::Blend::Premultiplied, x, y, width, height, opacity);
}
