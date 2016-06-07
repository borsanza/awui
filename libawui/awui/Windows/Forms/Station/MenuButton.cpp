/**
 * awui/Windows/Forms/Station/MenuButton.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "MenuButton.h"

#include <awui/Windows/Forms/Station/StationUI.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/MouseEventArgs.h>
#include <awui/Windows/Emulators/Chip8.h>
#include <awui/Windows/Emulators/MasterSystem.h>
#include <awui/Windows/Emulators/Spectrum.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;
using namespace awui::Windows::Forms;
using namespace awui::Windows::Forms::Station;

MenuButton::MenuButton(StationUI * station) {
	this->_station = station;
	this->SetSize(75, 23);
	// this->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	this->SetTabStop(true);

	this->_label.SetTextAlign(ContentAlignment::MiddleLeft);
}

MenuButton::~MenuButton() {
}

int MenuButton::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::MenuButton)
		return 1;

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

void MenuButton::OnMouseMove(MouseEventArgs* e) {
	// Form::SetControlSelected(this);
}

void MenuButton::OnPaint(GL* gl) {
	if (this->_node->_directory) {
		this->SetForeColor(Color::FromArgb(138, 226, 52));
	} else {
		if (Form::GetControlSelected() == this)
			this->SetForeColor(Color::FromArgb(255, 255, 255));
		else
			this->SetForeColor(Color::FromArgb(199, 199, 199));
	}

	this->_label.Draw(23, 0, this->GetWidth() - 32, this->GetHeight());
}

void MenuButton::SetText(const String str) {
	this->_label.SetText(str);
}

const awui::String MenuButton::GetText() {
	return this->_label.GetText();
}

void MenuButton::SetForeColor(const Drawing::Color color) {
	if (color != this->GetForeColor()) {
		Control::SetForeColor(color);
		this->_label.SetForeColor(this->GetForeColor());
	}
}

void MenuButton::SetFont(const Drawing::Font font) {
	Control::SetFont(font);
	this->_label.SetFont(font);
}

int MenuButton::GetLabelWidth() const {
	return this->_label.GetLabelWidth();
}

bool MenuButton::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	switch (button) {
		case RemoteButtons::Ok:
			this->_station->SelectChild(this->_node);
			break;
		case RemoteButtons::Menu:
			this->_station->SelectParent();
			break;
		default:
			break;
	}

	return 1;
}

void MenuButton::SetNodeFile(NodeFile * node) {
	this->_node = node;
}

void MenuButton::CheckArcade() {
	if (!this->_node->_directory) {
		if (this->_node->_arcade == NULL) {
			switch (this->_node->_emulator) {
				case 2:
				case 3: {
					MasterSystem * sms = new MasterSystem();
					sms->LoadRom(this->_node->_path);
					this->_node->_arcade = sms;
					break;
				}
				case 4: {
					Spectrum * szx = new Spectrum();
					szx->LoadRom(this->_node->_path);
					this->_node->_arcade = szx;
					break;
				}
				case 1: {
					Chip8 * ch8 = new Chip8();
					ch8->LoadRom(this->_node->_path);
					this->_node->_arcade = ch8;
					break;
				}
				default:
					break;
			}
		}
	}

	if (this->_node->_arcade)
		this->_node->_arcade->SetStationUI(this->_station);

	this->_station->SetArcade(this->_node->_arcade);
}
