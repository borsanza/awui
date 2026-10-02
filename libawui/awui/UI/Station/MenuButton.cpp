/**
 * awui/UI/Station/MenuButton.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "MenuButton.h"

#include <SDL_opengl.h>
#include <awui/OpenGL/Painter.h>
#include <awui/UI/Emulators/Chip8.h>
#include <awui/UI/Emulators/MasterSystem.h>
#include <awui/UI/Emulators/Spectrum.h>
#include <awui/UI/Form.h>
#include <awui/UI/Events/MouseEventArgs.h>
#include <awui/UI/Station/StationUI.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::UI::Emulators;
using namespace awui::UI::Station;
using namespace awui::UI::Input;
using namespace awui::UI::Events;

#define OFFSET 0.5f

MenuButton::MenuButton(StationUI *station) {
	m_node = NULL;
	SetBackColor(Color::Transparent);
	m_station = station;
	SetFocusable(true);
	SetFont(Font("Liberation Sans", 28, FontStyle::Bold));
	SetDock(DockStyle::None);

	AddWidget(&m_label, WidgetOwnership::Borrowed);
}

MenuButton::~MenuButton() {
}

void MenuButton::OnMouseDown(MouseEventArgs *e) {
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

void MenuButton::OnPaint(GL *gl) {
	Form *form = GetForm();
	if (form && (form->GetChildFocused() == this)) {
		SetForeColor(Color::White);
	} else {
		SetForeColor(Color::FromArgb(199, 199, 199));
	}

	if (m_node->m_directory) {
		float x = GetWidth() - 22.0f;
		float y = (GetHeight() / 2.0f) - 0.5f;

		// Flecha ">" de 2,5 píxeles de grosor, con los bordes suavizados
		std::vector<Painter::Vertex> lines;
		Painter::AddPolyline(lines, {x - 10.0f + OFFSET, y - 10.0f + OFFSET, x + OFFSET, y + OFFSET, x - 10.0f + OFFSET, y + 10.0f + OFFSET}, 2.5f, GetForeColor());
		Painter::Instance().DrawLines(lines);
	}
}

void MenuButton::SetText(const String str) {
	m_label.SetText(str);
}

void MenuButton::SetForeColor(const Color color) {
	if (color != GetForeColor()) {
		Control::SetForeColor(color);
		m_label.SetForeColor(GetForeColor());
	}
}

const awui::String MenuButton::GetText() const {
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

void MenuButton::SetNodeFile(NodeFile *node) {
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
					MasterSystem *sms = new MasterSystem();
					sms->LoadRom(m_node->m_path);
					m_node->m_arcade = sms;
					break;
				}
				case 4: {
					Spectrum *szx = new Spectrum();
					szx->LoadRom(m_node->m_path);
					m_node->m_arcade = szx;
					break;
				}
				case 1: {
					Chip8 *ch8 = new Chip8();
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

awui::String MenuButton::ToString() const {
	String a = awui::String("awui::UI::Station::MenuButton (");
	a += GetText();
	a += awui::String(")");
	return a;
}
