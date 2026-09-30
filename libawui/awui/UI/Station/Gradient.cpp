/**
 * awui/UI/Station/Gradient.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "Gradient.h"

#include <math.h>

#include <SDL_opengl.h>
#include <awui/Math.h>

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

// GL_CCW
void Gradient::OnPaint(GL *gl) {
	ColorF *c;

	glBegin(GL_QUADS);

	c = &m_color[3];
	glColor4ub(c->GetR(), c->GetG(), c->GetB(), c->GetA());
	glVertex3f(0.0f, GetHeight(), 0.0f); // Left Bottom

	c = &m_color[2];
	glColor4ub(c->GetR(), c->GetG(), c->GetB(), c->GetA());
	glVertex3f(GetWidth(), GetHeight(), 0.0f); // Right Bottom

	c = &m_color[1];
	glColor4ub(c->GetR(), c->GetG(), c->GetB(), c->GetA());
	glVertex3f(GetWidth(), 0.0f, 0.0f); // Right Top

	c = &m_color[0];
	glColor4ub(c->GetR(), c->GetG(), c->GetB(), c->GetA());
	glVertex3f(0.0f, 0.0f, 0.0f); // Left Top

	glEnd();
}

ColorF Gradient::InterpolateColor(ColorF *c1, ColorF *c2, float percent) {
	return ColorF::FromArgb(Math::Interpolate(c1->GetA(), c2->GetA(), percent), Math::Interpolate(c1->GetR(), c2->GetR(), percent), Math::Interpolate(c1->GetG(), c2->GetG(), percent), Math::Interpolate(c1->GetB(), c2->GetB(), percent));
}

void Gradient::OnTick(float deltaSeconds) {
	// Un 2% del camino por frame a 60 Hz, sea cual sea la tasa de frames: 0.98 de lo que falta cada 1/60 s
	float percent = 1.0f - powf(0.98f, deltaSeconds * 60.0f);
	for (int i = 0; i < 4; i++) {
		m_color[i] = InterpolateColor(&m_color[i], &m_colorGo[i], percent);
	}
}
