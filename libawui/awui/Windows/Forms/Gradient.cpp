/**
 * awui/Windows/Forms/Station/Gradient.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "Gradient.h"

#include <SDL2/SDL_opengl.h>

using namespace awui::OpenGL;
using namespace awui::Windows::Forms::Station;

Gradient::Gradient() {
	this->SetTabStop(false);
}

Gradient::~Gradient() {
}

int Gradient::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Gradient)
		return 1;

	return Control::IsClass(objectClass);
}

void Gradient::SetColor1(const Drawing::Color color) {
	this->_color1 = color;
}

void Gradient::SetColor2(const Drawing::Color color) {
	this->_color2 = color;
}

void Gradient::SetColor3(const Drawing::Color color) {
	this->_color3 = color;
}

void Gradient::SetColor4(const Drawing::Color color) {
	this->_color4 = color;
}

void Gradient::OnPaint(GL* gl) {
	glBegin(GL_QUADS);
	glColor4f(this->_color1.GetR() / 255.0f, this->_color1.GetG() / 255.0f, this->_color1.GetB() / 255.0f, this->_color1.GetA() / 255.0f);
	glVertex3f( 0.0f, 0.0f, 0.0f );
	glColor4f(this->_color2.GetR() / 255.0f, this->_color2.GetG() / 255.0f, this->_color2.GetB() / 255.0f, this->_color2.GetA() / 255.0f);
	glVertex3f( this->GetWidth(), 0.0f, 0.0f );
	glColor4f(this->_color3.GetR() / 255.0f, this->_color3.GetG() / 255.0f, this->_color3.GetB() / 255.0f, this->_color3.GetA() / 255.0f);
	glVertex3f( this->GetWidth(), this->GetHeight(), 0.0f );
	glColor4f(this->_color4.GetR() / 255.0f, this->_color4.GetG() / 255.0f, this->_color4.GetB() / 255.0f, this->_color4.GetA() / 255.0f);
	glVertex3f( 0.0f, this->GetHeight(), 0.0f );
	glEnd();
}
