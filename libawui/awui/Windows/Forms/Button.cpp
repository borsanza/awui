/*
 * awui/Windows/Forms/Button.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Button.h"

#include <awui/Drawing/Font.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Listeners/IButtonListener.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms;
using namespace awui::Windows::Forms::Listeners;

Button::Button() {
	m_class = Classes::Button;
	m_label.SetDock(DockStyle::Fill);
	m_label.SetTextAlign(ContentAlignment::MiddleCenter);

	SetSize(75, 23);
	SetBackColor(Color::FromArgb(0, 0, 0, 0));
	SetFocusable(true);
}

Button::~Button() {
	RemoveAllListeners();
}

bool Button::IsClass(Classes objectClass) const {
	return (objectClass == Classes::Button) || Control::IsClass(objectClass);
}

void Button::OnMouseLeave() {
}

void Button::OnMouseDown(MouseEventArgs *e) {
	if (IsFocusable()) {
		SetFocus();
	}
}

void Button::OnMouseMove(MouseEventArgs *e) {
	if (IsFocusable()) {
		SetFocus();
	}
}

void Button::OnMouseUp(MouseEventArgs *e) {
	Click();
}

bool Button::OnKeyPress(Keys::Enum key) {
	bool ret = false;
	switch (key) {
		case Keys::Key_ENTER:
		case Keys::Key_SPACE:
			Click();
			ret = true;
			break;

		default:
			break;
	}

	return ret;
}

void Button::OnPaint(GL *gl) {
	m_label.SetSize(GetWidth(), GetHeight());
	m_label.OnPaint(gl);
}

void Button::SetText(const String str) {
	m_label.SetText(str);
}

const awui::String Button::GetText() {
	return m_label.GetText();
}

void Button::SetForeColor(const Drawing::Color color) {
	Control::SetForeColor(color);
	m_label.SetForeColor(GetForeColor());
}

void Button::SetFont(const Drawing::Font font) {
	Control::SetFont(font);
	m_label.SetFont(font);
}

int Button::GetLabelWidth() const {
	return m_label.GetLabelWidth();
}

void Button::Click() {
	for (auto *listener : m_listeners) {
		listener->OnClick(this);
	}
}

void Button::AddOnClickListener(IButtonListener *listener) {
	m_listeners.push_back(listener);
}

void Button::RemoveOnClickListener(IButtonListener *listener) {
	m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), listener), m_listeners.end());
}

void Button::RemoveAllListeners() {
	m_listeners.clear();
}
