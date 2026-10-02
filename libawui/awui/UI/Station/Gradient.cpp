/**
 * awui/UI/Station/Gradient.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "Gradient.h"


#include <SDL_opengl.h>
#include <awui/Math.h>
#include <awui/OpenGL/Painter.h>

using namespace awui::Drawing;
using namespace awui;
using namespace awui::OpenGL;
using namespace awui::UI::Station;

Gradient::Gradient() {
}

void Gradient::SetColor(int pos, const ColorF color) {
	m_color[pos] = m_colorGo[pos] = color;
}

void Gradient::SetColorGo(int pos, const ColorF color) {
	m_colorGo[pos] = color;
}

// Un color en cada esquina (0: arriba izquierda, 1: arriba derecha, 2: abajo derecha, 3: abajo izquierda), que van de
// 0 a 255. Los dos triángulos parten el rectángulo igual que lo hacía GL_QUADS
void Gradient::OnPaint(GL *gl) {
	auto vertex = [this](float x, float y, int corner) {
		const ColorF &c = m_color[corner];
		auto toByte = [](float value) { return (uint8_t) Math::Clamp(value, 0.0f, 255.0f); };
		return Painter::Vertex{x, y, 0.0f, 0.0f, toByte(c.GetR()), toByte(c.GetG()), toByte(c.GetB()), toByte(c.GetA())};
	};

	Painter::Vertex leftBottom = vertex(0.0f, GetHeight(), 3);
	Painter::Vertex rightBottom = vertex(GetWidth(), GetHeight(), 2);
	Painter::Vertex rightTop = vertex(GetWidth(), 0.0f, 1);
	Painter::Vertex leftTop = vertex(0.0f, 0.0f, 0);
	Painter::Vertex vertices[] = {leftBottom, rightBottom, rightTop, leftBottom, rightTop, leftTop};
	Painter::Instance().DrawTriangles(vertices, 6);
}

ColorF Gradient::InterpolateColor(ColorF *c1, ColorF *c2, float percent) {
	return ColorF::FromArgb(Math::Interpolate(c1->GetA(), c2->GetA(), percent), Math::Interpolate(c1->GetR(), c2->GetR(), percent), Math::Interpolate(c1->GetG(), c2->GetG(), percent), Math::Interpolate(c1->GetB(), c2->GetB(), percent));
}

void Gradient::OnTick(float deltaSeconds) {
	// Un 2% del camino por frame a 60 Hz, sea cual sea la tasa de frames
	float percent = Math::SmoothFactor(0.02f, deltaSeconds);
	for (int i = 0; i < 4; i++) {
		m_color[i] = InterpolateColor(&m_color[i], &m_colorGo[i], percent);
	}
}
