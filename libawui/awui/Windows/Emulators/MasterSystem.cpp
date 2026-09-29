/**
 * awui/Windows/Emulators/MasterSystem.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "MasterSystem.h"

#include <awui/DateTime.h>
#include <awui/Drawing/Image.h>
#include <awui/Emulation/MasterSystem/Motherboard.h>
#include <awui/Emulation/MasterSystem/SoundSDL.h>
#include <awui/Emulation/MasterSystem/VDP.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Emulators/DebuggerSMS.h>
#include <awui/Windows/Forms/JoystickAxisMotionEventArgs.h>
#include <awui/Windows/Forms/JoystickButtonEventArgs.h>
#include <awui/Windows/Forms/JoystickButtons.h>

using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;
using namespace awui::Emulation::MasterSystem;

const int DEADZONE = 8192;

MasterSystem::MasterSystem() {
	m_class = Classes::MasterSystem;
	m_keys1 = 0xFF;
	m_keys2 = 0xFF;
	m_joys1 = 0xFF;
	m_joys2 = 0xFF;
	m_axis1 = 0xFF;
	m_axis2 = 0xFF;
	m_pause = false;
	m_invertButtons = false;

	SetSize(1, 1);
	m_image = new Drawing::Image(1, 1);
	m_cpu = new Motherboard();

	m_first = -1;
	m_last = -1;
	m_actual = -1;
	m_lastTick = 0;
	m_debugger = NULL;

	for (int i = 0; i < TOTALSAVED; i++)
		m_savedData[i] = (uint8_t *) calloc(Motherboard::GetSaveSize(), sizeof(uint8_t));
}

MasterSystem::~MasterSystem() {
	for (int i = 0; i < TOTALSAVED; i++)
		free(m_savedData[i]);

	delete m_cpu;
	delete m_image;
}

bool MasterSystem::IsClass(Classes objectClass) const {
	return (objectClass == Classes::MasterSystem) || ArcadeContainer::IsClass(objectClass);
}

void MasterSystem::LoadRom(const String file) {
	SetName(file);
	m_cpu->LoadRom(file);
	m_first = 0;
	m_last = 0;
	m_actual = 0;
	m_lastTick = DateTime::GetNow().GetTicks();
	m_cpu->SaveState(m_savedData[m_actual]);
}

void MasterSystem::OnTick(float deltaSeconds) {
	long long now = DateTime::GetNow().GetTicks();

	if ((now - m_lastTick) > 10000000) {
		m_lastTick = now;
		m_actual++;

		// Guardar tras rebobinar empieza otra línea de tiempo: los estados posteriores ya no sirven
		m_last = m_actual;

		// Buffer circular: el hueco que se va a pisar era el estado más antiguo
		if (m_actual - m_first >= TOTALSAVED) {
			m_first = m_actual - TOTALSAVED + 1;
		}

		m_cpu->SaveState(m_savedData[m_actual % TOTALSAVED]);
	}

	m_cpu->OnTick(deltaSeconds);
}

void MasterSystem::RunOpcode() {
	m_cpu->RunOpcode();
}

Motherboard *MasterSystem::GetCPU() {
	return m_cpu;
}

void MasterSystem::OnPaint(GL *gl) {
	uint16_t c;
	uint8_t r, g, b;
	VDP *screen = m_cpu->GetVDP();

	//	¿Lo rellenamos con el registro 7?
	//	c = screen->GetBackColor();
	//	r = color[c & 0x3];
	//	g = color[(c >> 2) & 0x3];
	//	b = color[(c >> 4) & 0x3];
	//	SetBackColor(Color::FromArgb(255, r, g, b));

	int width = screen->GetVisualWidth();
	int height = screen->GetVisualHeight();

	if ((width != m_image->GetWidth()) || (height != m_image->GetHeight())) {
		delete m_image;
		m_image = new Drawing::Image(width, height);
	}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// 12 bits (0x0BGR): cada componente de 4 bits pasa a 8 (x17: 0..255)
			c = screen->GetPixel(x, y);
			r = (c & 0xF) * 17;
			g = ((c >> 4) & 0xF) * 17;
			b = ((c >> 8) & 0xF) * 17;
			m_image->SetPixel(x, y, r, g, b);
		}
	}

	m_image->Update();

	float w = width;
	float h = height;
	float ratio = w / h;
	w = GetWidth();
	h = GetWidth() / ratio;

	if (h > GetHeight()) {
		h = GetHeight();
		w = GetHeight() * ratio;
	}

	int ratio2 = w / width;
	if (ratio2 >= 1) {
		w = width * ratio2;
		h = height * ratio2;
	}

	GL::DrawImageGL(m_image, int(GetWidth() - w) >> 1, int(GetHeight() - h) >> 1, w, h);
}

bool MasterSystem::OnKeyPress(Keys::Enum key) {
	bool ret = false;
	uint8_t button1 = 0;
	uint8_t button2 = 0;

	switch (key) {
		case Keys::Key_W:
			button1 = 0x01;
			break;
		case Keys::Key_S:
			button1 = 0x02;
			break;
		case Keys::Key_A:
			button1 = 0x04;
			break;
		case Keys::Key_D:
			button1 = 0x08;
			break;
		case Keys::Key_G:
		case Keys::Key_J:
			button1 = 0x10;
			break;
		case Keys::Key_H:
			button1 = 0x20;
			break;
		case Keys::Key_UP:
			button2 = 0x01;
			break;
		case Keys::Key_DOWN:
			button2 = 0x02;
			break;
		case Keys::Key_LEFT:
			button2 = 0x04;
			break;
		case Keys::Key_RIGHT:
			button2 = 0x08;
			break;
		case Keys::Key_KP1:
		case Keys::Key_KP3:
			button2 = 0x10;
			break;
		case Keys::Key_KP2:
			button2 = 0x20;
			break;
		case Keys::Key_SPACE:
			Pause(true);
			ret = true;
			break;
		case Keys::Key_BACKSPACE:
			m_cpu->Reset();
			ret = true;
			break;
		case Keys::Key_F:
			if (m_debugger) {
				m_debugger->SetShow(!m_debugger->GetShow());
				ret = true;
			}
			break;
		case Keys::Key_B: {
			VDP *screen = m_cpu->GetVDP();
			screen->SetShowBorder(!screen->GetShowBorder());
			screen->Clear();
			ret = true;
		} break;
		case Keys::Key_Q:
			TimeReverse();
			RefreshPads();
			ret = true;
			break;
		case Keys::Key_E:
			TimeForward();
			RefreshPads();
			ret = true;
			break;
		case Keys::Key_1:
			SoundSDL::ToggleChannel(0);
			ret = true;
			break;
		case Keys::Key_2:
			SoundSDL::ToggleChannel(1);
			ret = true;
			break;
		case Keys::Key_3:
			SoundSDL::ToggleChannel(2);
			ret = true;
			break;
		case Keys::Key_4:
			SoundSDL::ToggleChannel(3);
			ret = true;
			break;
	}

	if (button1) {
		m_keys1 &= ~button1;
		RefreshPads();
		ret = true;
	}

	if (button2) {
		m_keys2 &= ~button2;
		RefreshPads();
		ret = true;
	}

	return true;
}

bool MasterSystem::OnKeyUp(Keys::Enum key) {
	bool ret = false;
	uint8_t button1 = 0;
	uint8_t button2 = 0;

	switch (key) {
		case Keys::Key_W:
			button1 = 0x01;
			break;
		case Keys::Key_S:
			button1 = 0x02;
			break;
		case Keys::Key_A:
			button1 = 0x04;
			break;
		case Keys::Key_D:
			button1 = 0x08;
			break;
		case Keys::Key_G:
		case Keys::Key_J:
			button1 = 0x10;
			break;
		case Keys::Key_H:
			button1 = 0x20;
			break;
		case Keys::Key_UP:
			button2 = 0x01;
			break;
		case Keys::Key_DOWN:
			button2 = 0x02;
			break;
		case Keys::Key_LEFT:
			button2 = 0x04;
			break;
		case Keys::Key_RIGHT:
			button2 = 0x08;
			break;
		case Keys::Key_KP1:
		case Keys::Key_KP3:
			button2 = 0x10;
			break;
		case Keys::Key_KP2:
			button2 = 0x20;
			break;
		case Keys::Key_SPACE:
			Pause(false);
			ret = true;
			break;
		case Keys::Key_BACKSPACE:
			m_cpu->Reset();
			ret = true;
			break;
	}

	if (button1) {
		m_keys1 |= button1;
		RefreshPads();
		ret = true;
	}

	if (button2) {
		m_keys2 |= button2;
		RefreshPads();
		ret = true;
	}

	return ret;
}

bool MasterSystem::RefreshButtons(JoystickButtonEventArgs *e) {
	bool ret = false;

	uint32_t buttons = e->GetButtons();
	// printf("Buttons: %x\n", buttons);
	uint8_t masterButtons = ((buttons & JoystickButtons::JOYSTICK_BUTTON_DPAD_UP) ? 0x01 : 0) |
							((buttons & JoystickButtons::JOYSTICK_BUTTON_DPAD_DOWN) ? 0x02 : 0) |
							((buttons & JoystickButtons::JOYSTICK_BUTTON_DPAD_RIGHT) ? 0x08 : 0) |
							((buttons & JoystickButtons::JOYSTICK_BUTTON_DPAD_LEFT) ? 0x04 : 0) |
							((buttons & JoystickButtons::JOYSTICK_BUTTON_Y) ? (m_invertButtons ? 0x20 : 0x10) : 0) |
							((buttons & JoystickButtons::JOYSTICK_BUTTON_A) ? (m_invertButtons ? 0x20 : 0x10) : 0) |
							((buttons & JoystickButtons::JOYSTICK_BUTTON_B) ? (m_invertButtons ? 0x10 : 0x20) : 0) |
							((buttons & JoystickButtons::JOYSTICK_BUTTON_X) ? (m_invertButtons ? 0x10 : 0x20) : 0) |
							((buttons & JoystickButtons::JOYSTICK_BUTTON_START) ? 0x40 : 0);

	switch (e->GetWhich()) {
		case 0:
			m_joys1 = ~masterButtons;
			// printf("RefreshButtons: %x\n", m_joys1);
			ret = true;
			break;
		case 1:
			m_joys2 = ~masterButtons;
			ret = true;
			break;
	}

	if (ret) {
		RefreshPads();
	}

	return ret;
}

bool MasterSystem::OnJoystickButtonDown(JoystickButtonEventArgs *e) {
	if (e->GetButton() & JoystickButtons::JOYSTICK_BUTTON_LEFTSHOULDER) {
		TimeReverse();
		return true;
	}

	if (e->GetButton() & JoystickButtons::JOYSTICK_BUTTON_RIGHTSHOULDER) {
		TimeForward();
		return true;
	}

	if (e->GetButton() & JoystickButtons::JOYSTICK_BUTTON_START) {
		Pause(true);
		return true;
	}

	if (e->GetButton() & JoystickButtons::JOYSTICK_BUTTON_BACK) {
		m_invertButtons = !m_invertButtons;
		return true;
	}

	return RefreshButtons(e);
}

bool MasterSystem::OnJoystickButtonUp(JoystickButtonEventArgs *e) {
	if (e->GetButton() & JoystickButtons::JOYSTICK_BUTTON_START) {
		Pause(false);
		return true;
	}

	return RefreshButtons(e);
}

bool MasterSystem::OnJoystickAxisMotion(JoystickAxisMotionEventArgs *e) {
	bool ret = false;

	uint8_t masterButtons = ((e->GetAxisY() < -DEADZONE) ? 0x01 : 0) |
							((e->GetAxisY() > DEADZONE) ? 0x02 : 0) |
							((e->GetAxisX() < -DEADZONE) ? 0x04 : 0) |
							((e->GetAxisX() > DEADZONE) ? 0x08 : 0);

	switch (e->GetWhich()) {
		case 0:
			m_axis1 = ~masterButtons;
			// printf("RefreshButtons: %x\n", m_joys1);
			ret = true;
			break;
		case 1:
			m_axis2 = ~masterButtons;
			ret = true;
			break;
	}

	if (ret) {
		RefreshPads();
	}

	return ret;
}

uint32_t MasterSystem::GetCRC32() {
	return m_cpu->GetCRC32();
}

void MasterSystem::TimeReverse() {
	m_lastTick = DateTime::GetNow().GetTicks();
	m_actual--;
	if (m_actual < m_first)
		m_actual = m_first;
	m_cpu->LoadState(m_savedData[m_actual % TOTALSAVED]);
}

void MasterSystem::TimeForward() {
	m_lastTick = DateTime::GetNow().GetTicks();
	m_actual++;
	if (m_actual > m_last)
		m_actual = m_last;
	m_cpu->LoadState(m_savedData[m_actual % TOTALSAVED]);
}

void awui::Windows::Emulators::MasterSystem::Pause(bool mode) {
	if (mode) {
		if (!m_pause) {
			m_pause = true;
			m_cpu->SetPauseButton(true);
		}
	} else {
		m_pause = false;
		m_cpu->SetPauseButton(false);
	}
}

void MasterSystem::SetSoundEnabled(bool mode) {
	SoundSDL::Instance().SetPlayingSound(mode ? m_cpu->GetSound() : 0);
}

void MasterSystem::RefreshPads() {
	m_cpu->SetPad1(0xFF & (~(~m_keys1 | ~m_joys1 | ~m_axis1)));
	m_cpu->SetPad2(0xFF & (~(~m_keys2 | ~m_joys2 | ~m_axis2)));
}
