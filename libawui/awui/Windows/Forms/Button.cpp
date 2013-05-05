// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Button.h"

#include <awui/Math.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Drawing/Graphics.h>
#include <awui/Drawing/Image.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/MouseEventArgs.h>
#include <awui/Windows/Forms/TextRenderer.h>
#include <SDL_opengl.h>

#include <iostream>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;

Button::Button() {
	this->label.SetDock(DockStyle::Fill);
	this->label.SetTextAlign(ContentAlignment::MiddleCenter);

	this->SetSize(75,23);
	this->SetBackColor(Color::FromArgb(0, 0, 0));
	this->testx = 0;
	this->testy = 0;
	this->show = 0;
}

Button::~Button() {
}

int Button::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Button)
		return 1;

	return Control::IsClass(objectClass);
}

void Button::OnMouseLeave() {
	this->testx = -1;
	this->testy = -1;
	this->show = 0;
}

void Button::OnMouseDown(MouseEventArgs* e) {
	this->show = 1;
	this->testx = e->GetX();
	this->testy = e->GetY();
}

void Button::OnMouseMove(MouseEventArgs* e) {
	this->show = 1;
	this->testx = e->GetX();
	this->testy = e->GetY();
}

void Button::OnPaint(GL* gl) {
	if (this->show) {
		glColor3f(0.3f, 0.3f, 1.0f);
/*
		glBegin(GL_LINES);
		glVertex2f(0, 0);
		glVertex2f(this->testx, this->testy);
		glVertex2f(this->GetWidth() - 1, 0);
		glVertex2f(this->testx, this->testy);
		glVertex2f(0, this->GetHeight() - 1);
		glVertex2f(this->testx, this->testy);
		glVertex2f(this->GetWidth() - 1, this->GetHeight() - 1);
		glVertex2f(this->testx, this->testy);
		glEnd();
*/
		glBegin(GL_LINES);
		glVertex2f(0, this->GetHeight() - 1);
		glVertex2f(this->GetWidth() - 1, this->GetHeight() - 1);
		glVertex2f(this->GetWidth() - 1, 0);
		glVertex2f(this->GetWidth() - 1, this->GetHeight() - 1);
		glVertex2f(0, 0);
		glVertex2f(this->GetWidth() - 1, 0);
		glVertex2f(0, 0);
		glVertex2f(0, this->GetHeight() - 1);
		glEnd();
	}

	this->label.SetSize(this->GetWidth(), this->GetHeight());
	this->label.OnPaint(gl);
}

void Button::SetText(const String str) {
	this->label.SetText(str);
}

const awui::String Button::GetText() {
	return this->label.GetText();
}

void Button::SetForeColor(const Drawing::Color color) {
	Control::SetForeColor(color);
	this->label.SetForeColor(this->GetForeColor());
}

void Button::SetFont(const Drawing::Font * font) {
	Control::SetFont(font);
	this->label.SetFont(font);
}
