/**
 * awui/Windows/Forms/Station/MenuButton.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "MenuButton.h"

#include <awui/Windows/Emulators/Chip8.h>
#include <awui/Windows/Emulators/MasterSystem.h>
#include <awui/Windows/Emulators/Spectrum.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/MouseEventArgs.h>
#include <awui/Windows/Forms/Station/StationUI.h>
#include <SDL2/SDL_opengl.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;
using namespace awui::Windows::Forms::Station;

MenuButton::MenuButton(StationUI * station) {
	this->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	this->_station = station;
	this->SetTabStop(true);

	this->GetControls()->Add(&this->_label);
}

MenuButton::~MenuButton() {
	this->GetControls()->Remove(&this->_label);
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

#define OFFSET 0.5f

void MenuButton::OnPaint(GL* gl) {
	if (this->_node->_directory) {
		glLineWidth(2.5f);

		float x = this->GetWidth() - 22.0f;
		float y = (this->GetHeight() >> 1) - 0.5f;

		if (Form::GetControlSelected() == this) {
			this->SetForeColor(Color::FromArgb(255, 255, 255));
			glColor3ub(255, 255, 255);
		} else {
			this->SetForeColor(Color::FromArgb(199, 199, 199));
			glColor3ub(199, 199, 199);
		}

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
	this->_label.SetText(str);
}

void MenuButton::SetForeColor(const Drawing::Color color) {
	if (color != this->GetForeColor()) {
		Control::SetForeColor(color);
		this->_label.SetForeColor(this->GetForeColor());
	}
}

const awui::String MenuButton::GetText() {
	return this->_label.GetText();
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

void MenuButton::OnResize() {
	this->_label.SetLocation(23, 0);

	if (this->_node->_directory)
		this->_label.SetSize(this->GetWidth() - (50 + this->_label.GetLeft()), this->GetHeight());
	else
		this->_label.SetSize(this->GetWidth() - (0 + this->_label.GetLeft()), this->GetHeight());
}

/******************************** LabelButton *********************************/

LabelButton::LabelButton() {
	this->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	this->SetTextAlign(ContentAlignment::MiddleLeft);
	this->SetTabStop(false);
}

LabelButton::~LabelButton() {
}

void LabelButton::OnMouseDown(MouseEventArgs * e) {
	this->GetParent()->OnMouseDown(e);
}
