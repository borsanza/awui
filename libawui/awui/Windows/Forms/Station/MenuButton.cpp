/**
 * awui/Windows/Forms/Station/MenuButton.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "MenuButton.h"

#include <awui/Console.h>
#include <awui/Windows/Emulators/Chip8.h>
#include <awui/Windows/Emulators/MasterSystem.h>
#include <awui/Windows/Emulators/Spectrum.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/MouseEventArgs.h>
#include <awui/Windows/Forms/Station/StationUI.h>
#include <SDL_opengl.h>
#include <math.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;
using namespace awui::Windows::Forms::Station;

MenuButton::MenuButton(StationUI * station) {
	m_node = NULL;
	SetBackColor(Color::FromArgb(0, 0, 0, 0));
	m_station = station;
	SetTabStop(true);
	SetFont(Font("Liberation Sans", 28, FontStyle::Bold));
	SetDock(DockStyle::None);

	GetControls()->Add(&m_label);
}

MenuButton::~MenuButton() {
	GetControls()->Remove(&m_label);
}

bool MenuButton::IsClass(Classes objectClass) const {
	if (objectClass == Classes::MenuButton) {
		return true;
	}

	return Control::IsClass(objectClass);
}

void MenuButton::OnMouseLeave() {
}

void MenuButton::OnMouseDown(MouseEventArgs * e) {
	switch (e->GetButton()) {
		case MouseButtons::Left:
			if (Form::GetControlSelected() != this)
				Form::SetControlSelected(this);
			else
				OnRemoteKeyUp(0, RemoteButtons::Ok);
			break;
		case MouseButtons::Right:
			OnRemoteKeyUp(0, RemoteButtons::Menu);
			break;
		default:
			break;
	}
}

#define OFFSET 0.5f

void MenuButton::OnPaint(GL* gl) {
	if (Form::GetControlSelected() == this)
		SetForeColor(Color::FromArgb(255, 255, 255));
	else
		SetForeColor(Color::FromArgb(199, 199, 199));

	if (m_node->m_directory) {
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

void MenuButton::SetText(const String str) {
	m_label.SetText(str);
}

void MenuButton::SetForeColor(const Drawing::Color color) {
	if (color != GetForeColor()) {
		Control::SetForeColor(color);
		m_label.SetForeColor(GetForeColor());
	}
}

const awui::String MenuButton::GetText() {
	return m_label.GetText();
}

void MenuButton::SetFont(const Drawing::Font font) {
	Control::SetFont(font);
	m_label.SetFont(font);
}

int MenuButton::GetLabelWidth() const {
	return m_label.GetLabelWidth();
}

bool MenuButton::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	switch (button) {
		case RemoteButtons::Ok:
			m_station->SelectChild(m_node);
			break;
		case RemoteButtons::Menu:
			m_station->SelectParent();
			break;
		default:
			break;
	}

	return 1;
}

void MenuButton::SetNodeFile(NodeFile * node) {
	m_node = node;
}

void MenuButton::CheckArcade() {
	if (m_node->m_background)
		m_station->SetBackground(m_node->m_background);

	if (!m_node->m_directory) {
		if (m_node->m_arcade == NULL) {
			switch (m_node->m_emulator) {
				case 2:
				case 3: {
					MasterSystem * sms = new MasterSystem();
					sms->LoadRom(m_node->m_path);
					m_node->m_arcade = sms;
					break;
				}
				case 4: {
					Spectrum * szx = new Spectrum();
					szx->LoadRom(m_node->m_path);
					m_node->m_arcade = szx;
					break;
				}
				case 1: {
					Chip8 * ch8 = new Chip8();
					ch8->LoadRom(m_node->m_path);
					m_node->m_arcade = ch8;
					break;
				}
				default:
					break;
			}
		}
	}

	if (m_node->m_arcade)
		m_node->m_arcade->SetStationUI(m_station);

	m_station->SetArcade(m_node->m_arcade);
}

void MenuButton::OnResize() {
	m_label.SetLocation(23, 0);

	if (m_node->m_directory)
		m_label.SetSize(GetWidth() - (50 + m_label.GetLeft()), GetHeight());
	else
		m_label.SetSize(GetWidth() - (0 + m_label.GetLeft()), GetHeight());
}

/******************************** LabelButton *********************************/

LabelButton::LabelButton() {
	m_time = 0.0f;
	m_lastSelected = false;
	SetBackColor(Color::FromArgb(0, 0, 0, 0));
	SetTextAlign(ContentAlignment::MiddleLeft);
	SetTabStop(false);
}

LabelButton::~LabelButton() {
}

void LabelButton::OnMouseDown(MouseEventArgs * e) {
	GetParent()->OnMouseDown(e);
}

void LabelButton::OnTick(float deltaTime) {
	bool selected = (Form::GetControlSelected() == GetParent());

	if (m_lastSelected != selected) {
		m_lastSelected = selected;
		m_time = 0.0f;
	}

	if (selected && (GetLabelWidth() > GetWidth())) {
		if (m_time < 2.0f) {
			SetScrolled(0.0f);
		} else {
			float prev = GetScrolled();
			SetScrolled(prev - (deltaTime * 120.0f));
			float after = GetScrolled();
			if (after > prev) {
				SetScrolled(0.0f);
				m_time = 0.0f;
			}
		}

		m_time += deltaTime;
	} else {
		float scrolled = GetScrolled();
		if (scrolled != 0) {
			float dst = 0;
			float min = -(GetLabelWidth() + 80);
			if ((GetLabelWidth() >> 1) < -scrolled)
				dst = min;

			dst = Interpolate(scrolled, dst, deltaTime * 10.0f);
			if ((fabs(dst) <= 1) || ((dst - 1) <= min))
				dst = 0;

			if ((dst <= -80) && (dst >= (min + 80)))
				dst = 0;

			SetScrolled(dst);
		}

		m_time = 0.0f;
	}
}
