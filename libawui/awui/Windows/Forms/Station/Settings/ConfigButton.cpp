/**
 * awui/Windows/Forms/Station/MenuButton.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "ConfigButton.h"

#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Drawing/Font.h>
#include <SDL_opengl.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Forms::Station::Settings;

#define OFFSET 0.5f

ConfigButton::ConfigButton() {
	SetBackColor(Color::FromArgb(0, 0, 0, 0));
	SetTabStop(true);
	SetFont(Font("Liberation Sans", 28, FontStyle::Bold));
	SetDock(DockStyle::None);

	GetControls()->Add(&m_label);

	// SetBackColor(Color::FromArgb(0, 255, 0));
}

ConfigButton::~ConfigButton() {
	GetControls()->Remove(&m_label);
}

bool ConfigButton::IsClass(Classes objectClass) const {
	if (objectClass == Classes::ConfigButton) {
		return true;
	}

	return Control::IsClass(objectClass);
}

void ConfigButton::OnPaint(GL* gl) {
	if (Form::GetControlSelected() == this)
		SetForeColor(Color::FromArgb(255, 255, 255));
	else
		SetForeColor(Color::FromArgb(199, 199, 199));

	if (m_group) {
		glLineWidth(2.5f);

		float x = GetWidth() - 22.0f;
		float y = (GetHeight() / 2.0f) - 0.5f;

		if (Form::GetControlSelected() == this)
			glColor3ub(255, 255, 255);
		else
			glColor3ub(199, 199, 199);

		glEnable( GL_LINE_SMOOTH );
		glBegin(GL_LINE_STRIP);
		glVertex2f(x - 10.0f + OFFSET, y - 10.0f + OFFSET);
		glVertex2f(x + OFFSET, y + OFFSET);
		glVertex2f(x - 10.0f + OFFSET, y + 10.0f + OFFSET);
		glEnd();
		glDisable( GL_LINE_SMOOTH );
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

const awui::String ConfigButton::GetText() {
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
