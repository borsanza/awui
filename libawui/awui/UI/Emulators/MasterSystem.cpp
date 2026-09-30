/**
 * awui/UI/Emulators/MasterSystem.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "MasterSystem.h"

#include <awui/Emulation/Common/RewindBuffer.h>
#include <awui/Emulation/Common/SavePaths.h>
#include <awui/Drawing/Image.h>
#include <awui/Emulation/MasterSystem/Motherboard.h>
#include <awui/Emulation/MasterSystem/Sound.h>
#include <awui/Emulation/Common/AudioOutput.h>
#include <awui/Emulation/MasterSystem/VDP.h>
#include <awui/OpenGL/GL.h>
#include <awui/UI/Emulators/DebuggerSMS.h>
#include <awui/UI/Events/JoystickAxisMotionEventArgs.h>
#include <awui/UI/Events/JoystickButtonEventArgs.h>
#include <awui/UI/Input/JoystickButtons.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::UI::Emulators;
using namespace awui::Emulation::MasterSystem;
using namespace awui::Emulation::Common;
using namespace awui::UI;
using namespace awui::UI::Input;
using namespace awui::UI::Events;

// Memoria máxima del historial de rebobinado. Cada frame ocupa unos pocos KB (solo lo que cambia), así que da
// para varios minutos
#define REWIND_MAX_BYTES (64 * 1024 * 1024)

const int DEADZONE = 8192;

MasterSystem::MasterSystem() {
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

	m_debugger = NULL;

	m_state.resize(Motherboard::GetSaveSize());
	m_rewind = new RewindBuffer(m_state.size(), REWIND_MAX_BYTES);
	m_rewinding = false;
	m_forwarding = false;
}

MasterSystem::~MasterSystem() {
	delete m_rewind;

	delete m_cpu;
	delete m_image;
}

void MasterSystem::LoadRom(const String file) {
	SetName(file);
	m_cpu->LoadRom(file);
	m_rewind->Clear();
	m_cpu->SaveState(m_state.data());
	m_rewind->Push(m_state.data());
}

void MasterSystem::OnTick(float deltaSeconds) {
	// Rebobinando (o avanzando por lo rebobinado): cada tick carga el estado anterior (o siguiente) y emula ese
	// frame para verlo y oírlo (al revés si se retrocede). Al acabarse el historial se queda en el último
	if (m_rewinding || m_forwarding) {
		bool ok = m_rewinding ? m_rewind->Back(m_state.data()) : m_rewind->Forward(m_state.data());
		if (ok) {
			m_cpu->LoadState(m_state.data());
			m_cpu->GetSound()->SetReverse(m_rewinding);
			m_cpu->RunFrame();
		}

		return;
	}

	m_cpu->GetSound()->SetReverse(false);
	m_cpu->OnTick(deltaSeconds);

	// Guardar tras rebobinar empieza otra línea de tiempo: lo que se podía volver a avanzar se descarta
	m_cpu->SaveState(m_state.data());
	m_rewind->Push(m_state.data());
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
			SetRewinding(true);
			ret = true;
			break;
		case Keys::Key_E:
			SetForwarding(true);
			ret = true;
			break;
		case Keys::Key_1:
			awui::Emulation::MasterSystem::Sound::ToggleChannel(0);
			ret = true;
			break;
		case Keys::Key_2:
			awui::Emulation::MasterSystem::Sound::ToggleChannel(1);
			ret = true;
			break;
		case Keys::Key_3:
			awui::Emulation::MasterSystem::Sound::ToggleChannel(2);
			ret = true;
			break;
		case Keys::Key_4:
			awui::Emulation::MasterSystem::Sound::ToggleChannel(3);
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
		case Keys::Key_Q:
			SetRewinding(false);
			ret = true;
			break;
		case Keys::Key_E:
			SetForwarding(false);
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
		SetRewinding(true);
		return true;
	}

	if (e->GetButton() & JoystickButtons::JOYSTICK_BUTTON_RIGHTSHOULDER) {
		SetForwarding(true);
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
	if (e->GetButton() & JoystickButtons::JOYSTICK_BUTTON_LEFTSHOULDER) {
		SetRewinding(false);
		return true;
	}

	if (e->GetButton() & JoystickButtons::JOYSTICK_BUTTON_RIGHTSHOULDER) {
		SetForwarding(false);
		return true;
	}

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

bool MasterSystem::SaveAutoState() {
	m_cpu->SaveState(m_state.data());
	return WriteStateFile(SavePaths::GetWritePath(String::Concat(GetName(), ".autostate")), m_state.data(), (int) m_state.size());
}

bool MasterSystem::LoadAutoState() {
	if (!ReadStateFile(SavePaths::GetReadPath(String::Concat(GetName(), ".autostate")), m_state.data(), (int) m_state.size()))
		return false;

	m_cpu->LoadState(m_state.data());
	RefreshPads();

	// El historial de rebobinado era de otra partida: empieza desde aquí
	m_rewind->Clear();
	m_rewind->Push(m_state.data());
	return true;
}

void MasterSystem::SetRewinding(bool mode) {
	m_rewinding = mode;
	// Los estados cargados traen los mandos de cuando se guardaron: al soltar vuelven los que se pulsan ahora
	if (!mode)
		RefreshPads();
}

void MasterSystem::SetForwarding(bool mode) {
	m_forwarding = mode;
	if (!mode)
		RefreshPads();
}

void awui::UI::Emulators::MasterSystem::Pause(bool mode) {
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
	AudioOutput::Instance().SetPlaying(mode ? m_cpu->GetSound() : 0);
}

void MasterSystem::RefreshPads() {
	m_cpu->SetPad1(0xFF & (~(~m_keys1 | ~m_joys1 | ~m_axis1)));
	m_cpu->SetPad2(0xFF & (~(~m_keys2 | ~m_joys2 | ~m_axis2)));
}
