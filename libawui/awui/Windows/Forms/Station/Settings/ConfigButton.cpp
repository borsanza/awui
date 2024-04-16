/**
 * awui/Windows/Forms/Station/MenuButton.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "ConfigButton.h"

#include <SDL_opengl.h>
#include <awui/Drawing/Font.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Listeners/IRemoteListener.h>
#include <awui/Windows/Forms/MouseEventArgs.h>
#include <awui/Windows/Forms/Station/Page.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms::Listeners;
using namespace awui::Windows::Forms::Station::Settings;

#define OFFSET 0.5f

ConfigButton::ConfigButton(TypeButton typeButton) {
	m_class = Classes::ConfigButton;
	m_typeButton = typeButton;
	m_subpage = nullptr;

	SetBackColor(Color::FromArgb(0, 0, 0, 0));
	SetFont(Font("Liberation Sans", 28, FontStyle::Bold));
	SetDock(DockStyle::None);
	SetFocusable(true);

	AddWidget(&m_label);
}

ConfigButton::~ConfigButton() {
	RemoveWidget(&m_label);
}

bool ConfigButton::IsClass(Classes objectClass) const {
	return (objectClass == Classes::ConfigButton) || Control::IsClass(objectClass);
}

void ConfigButton::OnPaint(GL *gl) {
	Form *form = GetForm();
	if (form && (form->GetChildFocused() == this)) {
		SetForeColor(Color::FromArgb(255, 255, 255));
	} else {
		SetForeColor(Color::FromArgb(199, 199, 199));
	}

	if (m_group) {
		glLineWidth(2.5f);

		float x = GetWidth() - 22.0f;
		float y = (GetHeight() / 2.0f) - 0.5f;

		Form *form = GetForm();
		if (form && (form->GetChildFocused() == this)) {
			glColor3ub(255, 255, 255);
		} else {
			glColor3ub(199, 199, 199);
		}

		glEnable(GL_LINE_SMOOTH);
		glBegin(GL_LINE_STRIP);
		glVertex2f(x - 10.0f + OFFSET, y - 10.0f + OFFSET);
		glVertex2f(x + OFFSET, y + OFFSET);
		glVertex2f(x - 10.0f + OFFSET, y + 10.0f + OFFSET);
		glEnd();
		glDisable(GL_LINE_SMOOTH);
	}
}

void ConfigButton::SetText(const String str) {
	m_label.SetText(str);
}

void ConfigButton::SetForeColor(const Drawing::Color color) {
	if (color != GetForeColor()) {
		Control::SetForeColor(color);
		m_label.SetForeColor(GetForeColor());
	}
}

const awui::String ConfigButton::GetText() const {
	return m_label.GetText();
}

void ConfigButton::SetFont(const Drawing::Font font) {
	Control::SetFont(font);
	m_label.SetFont(font);
}

int ConfigButton::GetLabelWidth() const {
	return m_label.GetLabelWidth();
}

void ConfigButton::OnResize() {
	m_label.SetLocation(23, 0);
	if (m_group) {
		m_label.SetSize(GetWidth() - (50 + m_label.GetLeft()), GetHeight());
	} else {
		m_label.SetSize(GetWidth() - (0 + m_label.GetLeft()), GetHeight());
	}
}

void ConfigButton::Click() {
	for (auto *listener : m_listeners) {
		listener->OnOk(this);
	}
}

void ConfigButton::AddOnClickListener(IRemoteListener *listener) {
	m_listeners.push_back(listener);
}

void ConfigButton::RemoveOnClickListener(IRemoteListener *listener) {
	m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), listener), m_listeners.end());
}

void ConfigButton::RemoveAllListeners() {
	m_listeners.clear();
}

void ConfigButton::OnMouseDown(MouseEventArgs *e) {
	switch (e->GetButton()) {
		case MouseButtons::Left: {
			if (!IsFocused()) {
				SetFocus();
			} else {
				OnRemoteKeyUp(0, RemoteButtons::Ok);
			}
		} break;
		case MouseButtons::Right:
			OnRemoteKeyUp(0, RemoteButtons::Menu);
			break;
		default:
			break;
	}
}

awui::String ConfigButton::ToString() const {
	String a = awui::String("awui::Windows::Forms::Station::Settings::ConfigButton (");
	a += GetText();
	a += awui::String(")");
	return a;
}

bool ConfigButton::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	switch (button) {
		case RemoteButtons::Ok:
			Click();
			// m_station->SelectChild(m_node);
			break;
		case RemoteButtons::Menu:
			// m_station->SelectParent();
			break;
		default:
			break;
	}

	return 1;
}