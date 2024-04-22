/*
 * awui/Windows/Forms/Button.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Button.h"

#include <awui/Drawing/Font.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Listeners/IRemoteListener.h>
#include <awui/Windows/Forms/MouseEventArgs.h>

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

void Button::OnMouseDown(MouseEventArgs *e) {
	switch (e->GetButton()) {
		case MouseButtons::Left:
			if (!IsFocused()) {
				SetFocus();
			} else {
				OnRemoteKeyUp(0, RemoteButtons::Ok);
			}
			break;
		case MouseButtons::Right:
			OnRemoteKeyUp(0, RemoteButtons::Menu);
			break;
		default:
			break;
	}
}

bool Button::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	switch (button) {
		case RemoteButtons::Ok:
			Click();
			break;

		default:
			break;
	}

	return 1;
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

void Button::SetForeColor(const Color color) {
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
	for (auto *listener : m_buttonListeners) {
		listener->OnOk(this);
	}
}

void Button::AddOnClickListener(IRemoteListener *listener) {
	m_buttonListeners.push_back(listener);
}

void Button::AddOnExitListener(IExitListener *listener) {
	m_exitListeners.push_back(listener);
}

void Button::RemoveOnClickListener(IRemoteListener *listener) {
	m_buttonListeners.erase(std::remove(m_buttonListeners.begin(), m_buttonListeners.end(), listener), m_buttonListeners.end());
}

void Button::RemoveOnExitListener(IExitListener *listener) {
	m_exitListeners.erase(std::remove(m_exitListeners.begin(), m_exitListeners.end(), listener), m_exitListeners.end());
}

void Button::RemoveAllListeners() {
	m_buttonListeners.clear();
	m_exitListeners.clear();
}
