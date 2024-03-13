/**
 * awui/Windows/Forms/Station/Gradient.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "Gradient.h"

#include <awui/Math.h>
#include <SDL_opengl.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms::Station;

Gradient::Gradient() {
	m_class = Classes::Gradient;
}

bool Gradient::IsClass(Classes objectClass) const {
	return (objectClass == Classes::Gradient) || Control::IsClass(objectClass);
}

void Gradient::SetColor(int pos, const Drawing::ColorF color) {
	this->m_color[pos] = this->m_colorGo[pos] = color;
}

void Gradient::SetColorGo(int pos, const Drawing::ColorF color) {
	this->m_colorGo[pos] = color;
}

void Gradient::OnPaint(GL* gl) {
	ColorF * c;

	glBegin(GL_QUADS);

	c = &this->m_color[0];
	glColor4ub(c->GetR(), c->GetG(), c->GetB(), c->GetA());
	glVertex3f(0.0f, 0.0f, 0.0f);

	c = &this->m_color[1];
	glColor4ub(c->GetR(), c->GetG(), c->GetB(), c->GetA());
	glVertex3f(this->GetWidth(), 0.0f, 0.0f);

	c = &this->m_color[2];
	glColor4ub(c->GetR(), c->GetG(), c->GetB(), c->GetA());
	glVertex3f(this->GetWidth(), this->GetHeight(), 0.0f);

	c = &this->m_color[3];
	glColor4ub(c->GetR(), c->GetG(), c->GetB(), c->GetA());
	glVertex3f(0.0f, this->GetHeight(), 0.0f);

	glEnd();
}

awui::Drawing::ColorF Gradient::InterpolateColor(Drawing::ColorF * c1, Drawing::ColorF * c2, float percent) {
	return ColorF::FromArgb(
		Math::Interpolate(c1->GetA(), c2->GetA(), percent),
		Math::Interpolate(c1->GetR(), c2->GetR(), percent),
		Math::Interpolate(c1->GetG(), c2->GetG(), percent),
		Math::Interpolate(c1->GetB(), c2->GetB(), percent));
}

void Gradient::OnTick(float deltaSeconds) {
	for (int i = 0; i < 4; i++)
		this->m_color[i] = this->InterpolateColor(&this->m_color[i], &this->m_colorGo[i], 0.02f);
}
