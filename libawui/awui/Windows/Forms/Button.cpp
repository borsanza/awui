/*
 * awui/Windows/Forms/Button.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Button.h"

#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/MouseEventArgs.h>
#include <SDL_opengl.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;

Button::Button() {
	this->label.SetDock(DockStyle::Fill);
	this->label.SetTextAlign(ContentAlignment::MiddleCenter);

	this->SetSize(75,23);
	this->SetBackColor(Color::FromArgb(0, 0, 0));
	this->SetTabStop(true);
}

Button::~Button() {
}

int Button::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Button)
		return 1;

	return Control::IsClass(objectClass);
}

void Button::OnMouseLeave() {
}

void Button::OnMouseDown(MouseEventArgs* e) {
	Form::SetControlSelected(this);
}

void Button::OnMouseMove(MouseEventArgs* e) {
	Form::SetControlSelected(this);
}

void Button::OnPaint(GL* gl) {
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
