/**
 * awui/Windows/Forms/MenuButton.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "MenuButton.h"

#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/MouseEventArgs.h>
#include <SDL2/SDL_opengl.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;
using namespace awui::Windows::Forms::Station;

MenuButton::MenuButton() {
	this->SetSize(75, 23);
	this->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	this->SetTabStop(true);

	this->label.SetTextAlign(ContentAlignment::MiddleLeft);
}

MenuButton::~MenuButton() {
}

int MenuButton::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Button)
		return 1;

	return Control::IsClass(objectClass);
}

void MenuButton::OnMouseLeave() {
}

void MenuButton::OnMouseDown(MouseEventArgs* e) {
	Form::SetControlSelected(this);
}

void MenuButton::OnMouseMove(MouseEventArgs* e) {
	Form::SetControlSelected(this);
}

void MenuButton::OnPaint(GL* gl) {
	this->label.Draw(16, 0, this->GetWidth() - 32, this->GetHeight());
}

void MenuButton::SetText(const String str) {
	this->label.SetText(str);
}

const awui::String MenuButton::GetText() {
	return this->label.GetText();
}

void MenuButton::SetForeColor(const Drawing::Color color) {
	Control::SetForeColor(color);
	this->label.SetForeColor(this->GetForeColor());
}

void MenuButton::SetFont(const Drawing::Font * font) {
	Control::SetFont(font);
	this->label.SetFont(font);
}

int MenuButton::GetLabelWidth() const {
	return this->label.GetLabelWidth();
}
