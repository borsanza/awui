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
#include <awui/Windows/Forms/JoystickAxisMotionEventArgs.h>
#include <awui/Windows/Forms/JoystickButtons.h>
#include <awui/Windows/Forms/JoystickButtonEventArgs.h>
#include <awui/Windows/Emulators/DebuggerSMS.h>

using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;
using namespace awui::Emulation::MasterSystem;

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

	m_first = -1;
	m_last = -1;
	m_actual = -1;
	m_lastTick = 0;
	m_debugger = NULL;

	for (int i = 0; i < TOTALSAVED; i++)
		m_savedData[i] = (uint8_t *) calloc (Motherboard::GetSaveSize(), sizeof(uint8_t));
}

MasterSystem::~MasterSystem() {
	for (int i = 0; i < TOTALSAVED; i++)
		free(m_savedData[i]);

	delete m_cpu;
	delete m_image;
}

bool MasterSystem::IsClass(Classes objectClass) const {
	if (objectClass == Classes::MasterSystem) {
		return true;
	}

	return ArcadeContainer::IsClass(objectClass);
}

void MasterSystem::LoadRom(const String file) {
	SetName(file);
	m_cpu->LoadRom(file);
	m_first = 0;
	m_last = 0;
	m_actual = 0;
	m_lastTick = DateTime::GetNow().GetTicks();
	m_cpu->SaveState(m_savedData[m_actual]);

	uint32_t crc = GetCRC32();
	switch (crc) {
		case 0xd87316f6: // #gscept Intro by blindio (PD)
		case 0xa581402e: // 64 Color Palette Test Program by Charles MacDonald V1.00 (PD)
		case 0x6d165047: // 64 Color Palette Test Program by Charles MacDonald Vbeta (PD)
		case 0xb3e5986e: // AntiISDA Warrior by Ventzislav Tzvetkov (V1.02) (PD)
		case 0xb4dfb825: // Bank Panic (E) [p1][!]
		case 0xa957b2f8: // Battleships by Killer Bean 2 V1.00 (PD)
		case 0x1712e9f1: // Battleships by Killer Bean 2 V1.10 (PD)
		case 0x6994770d: // Blockhead by Proppy & Tet v20040714 (PD)
		case 0xaed594a2: // Blockhead by Proppy & Tet v20040807 (PD)
		case 0xa11b791c: // Blockhead by Proppy & Tet v20041016 (PD)
		case 0x79a1d911: // Blockhead by Proppy & Tet vWIP1 (PD)
		case 0x2734a9b4: // Blockhead by Proppy & Tet vWIP2 (PD)
		case 0xb80a74a1: // Bock's Birthday 2007 by Maxim (PD)
		case 0xe041f33e: // Bock's Birthday 2011 by Charles MacDonald (PD)
		case 0xc9ee4a8d: // Bock's Birthday 2011 II by Maxim (PD)
		case 0x943ac89d: // Burgers of Hanoi by smccd v1.00 (PD)
		case 0xeead16c7: // Canyon Racer V0.01alpha by Haroldo O. Pinheiro (PD)
		case 0x7d2280a5: // Cavelon by Timo Brueggmann V2 (PD)
		case 0x9d19b99c: // Chicken Demo (PD)
		case 0xbfee193c: // Code 38 - Volume I by Furrtek & L.i.N. (PD)
		case 0x7253c3ec: // Color & Switch Test (U)
		case 0x5c19e8a5: // Color Check
		case 0x9d549e08: // Comical Machine Gun Joe (J) [!]
		case 0x8f0e826a: // CWP_20110508
		case 0xfcefd481: // Damiana by Heliophobe (PD)
		case 0x7cc2a20b: // DCEvolution.net Intro by Ventzislav Tzvetkov, Maze & Apathy V1.00 (PD)
		case 0xc4ca6878: // Digger Chan by Aypok (PD)
		case 0xdddb3dd8: // Draw Poker by Mike Beaver (PD)
		case 0xc297198f: // F-16 Fighter (UE) [!]
		case 0x7ce06fce: // F-16 Fighting Falcon (J) [!]
		case 0x5833e1f2: // Fri For Fransk! by Marc Klemp V1.03 (PD)
		case 0x27b87fad: // Fullscreen Palette Test Program
		case 0x96ca6902: // Ghouls 'n Ghosts Demo (U)
		case 0x5ab72491: // Great Soccer (T) [!]
		case 0xc256921a: // Hang-On (J) [!]
		case 0xed169d59: // Headbreak For a Scroller by Furrtek & L.i.N. (PD)
		case 0x30165ac2: // Hello, World Test 1 (PD)
		case 0x191c3113: // Interactive Sprite Test (PD)
		case 0x6887900e: // Interrupt Test (PD)
		case 0x52d3ce1f: // JJ's SMS OS - Keyboard Test (PD)
		case 0x783c99ea: // Joust V0.05 Alpha (PD)
		case 0x1074e6d4: // King Kong by drHirudo V1.0 (PD)
		case 0x7e15a103: // KunKun & KokoKun by Bock (PD)
		case 0xa48ba2cb: // LD19 Practice Game by thematrixeatsyou (PD)
		case 0xeb71247b: // Lethal Weapon 3 Music (Prototype)
		case 0x08a72ed4: // Libc Test (Older) (PD)
		case 0x3e10b246: // Libc Test (PD)
		case 0xc264580f: // Line Interrupt Test #1 by Charles MacDonald (PD)
		case 0x5bbb64da: // Maze3D V2008-01-14 (PD)
		case 0x261587b8: // Not Only Words by Maxim (V1.00) (PD)
		case 0x580293ee: // Only Words by Mike Gordon (v0.1) (PD)
		case 0x592b8297: // Pause Test by FluBBa (PD)
		case 0xe54af03b: // Platform Game Test by Haroldo O. Pinheiro V0.1 (PD)
		case 0x1da46851: // Platform Game Test by Haroldo O. Pinheiro V0.2 (PD)
		case 0xe1c4a4c3: // Platform Game Test by Haroldo O. Pinheiro V0.3 (PD)
		case 0xbfa31c6f: // PSG Sample Test by Haroldo O. Pinheiro (2006) (PD)
		case 0xef3d4c50: // Pongmaster by Haroldo O. Pinheiro (PD) [a1]
		case 0x3879ac1b: // Pongmaster by Haroldo O. Pinheiro (PD)
		case 0x0652b785: // Pretty Photo by Maxim v1.00 (PD)
		case 0x30af0233: // Promocao Especial M.System III Compact (B) [!]
		case 0xd58e94b4: // RehaShMS V3 by mic_ (NTSC) (PD)
		case 0xfb9f8f63: // RehaShMS V3 by mic_ (PAL) (PD)
		case 0xa82411e0: // RehaShMS V4 by mic_ (NTSC) (PD)
		case 0xead53688: // RehaShMS V4 by mic_ (PAL) (PD)
		case 0x04657906: // RehaShMS V6 by mic_ (NTSC) (PD)
		case 0x6428124a: // RehaShMS V6 by mic_ (PAL) (PD)
		case 0xf61874c9: // Robbyie by Ventzislav Tzvetkov v0.999 (PD)
		case 0xefba05fc: // Robot Finds Kitten by thematrixeatsyou v1.00 (PD)
		case 0x7cbd4432: // SALY V1.00 by Marc Klemp (PD)
		case 0x16249e19: // Satellite 7 (J) [!]
		case 0x6841ce1d: // Scroll Test Program by Charles MacDonald V1.00 (PD)
		case 0x0bb99157: // Sega Genesis 6-Button Controller v2 by Charles MacDonald & pooraussie (PD)
		case 0x5f1120c8: // Sega Mark III BG Test v0.01 by Enri (PD)
		case 0x49790a5a: // Sega Mark III Port Test by Enri v0.1 (PD
		case 0x4fcc473b: // Sega Mega Drive & Genesis 6 Button Controller Test V1.0 by Charles MacDonald (PD)
		case 0xb0131d77: // Sega Tween (3D)
		case 0xf97c47a0: // Sega Tween (Normal)
		case 0x935dca5f: // Simple BIOS
		case 0x5f40660d: // Simple Sprite Demo Release 2 by Nicolas Warren (PD)
		case 0x350f7497: // SMS APA Demo V0.01 by Haroldo O. Pinheiro (PD)
		case 0xad31c5ff: // SMS Boot Loader
		case 0x7694ed1c: // SMS Cycle Counter Demo by Maxim (PD)
		case 0xa8ef4c54: // SMS Dump 1.00 WIP by Omar Cornut (PD)
		case 0x1a15dfcc: // SMS Prototype M404 BIOS
		case 0xbd274327: // SMS Sprite Test (PD)
		case 0x2f7d2cea: // SMS VDP Test by FluBBa v1.31 (PD)
		case 0xdf0bd4b8: // SMS VDP Test by FluBBa v2008-02-22 (PD)
		case 0x0e463253: // SMS VDP Test by FluBBa v2008-03-27 (PD)
		case 0x370d2403: // SMS_demo_muy_simple_ASCII
		case 0x15181f9f: // SMS_demo_scroll
		case 0xc9c8363f: // SMS_demo_scroll_sprite
		case 0x357f000b: // SMSC Text Demo V2 by Super Majik Spiral Crew (PD)
		case 0xea766665: // SMSPalette-SMS-1.00
		case 0x297efb87: // SMSPower's 7th Anniversary Intro V1.00 by Nicolas Warren (PD)
		case 0x1b1efc66: // Sound Test by Nicolas Warren (PD)
		case 0xd6a8dd98: // Sprite and Priority Test Program by Charles MacDonald (PD)
		case 0xffcf390e: // Sprite Multiplex Demo (03012005) by Charles MacDonald (PD)
		case 0x22525d51: // Super Magic Drive BIOS V4.1a [!]
		case 0x409c9860: // supermariocloudscoryarcangel
		case 0x8cd057a3: // Test Old (PD)
		case 0x47b6499e: // Test (PD)
		case 0x7f930360: // Testsnd (PD)
		case 0x983e17e4: // Tetracycline Release 2 by Nicolas Warren (PD)
		case 0xa0bf829b: // Tetracycline Release 4 by Nicolas Warren (PD)
		case 0x949348fb: // Tower of Sokoban by Killer Bean 2 (PD)
		case 0x58b99750: // TransBot (Prototype)
		case 0xdfe369fa: // TRG by Sebastian v1.20_2011-05-02 (PD)
		case 0x4e9cc98f: // Ultima III V2006-05-13prealpha (PD)
		case 0x0b177200: // Video Poker v0.7 by Sket (PD)
		case 0x49007fd6: // Video Poker v0.8 by Sket (PD)
		case 0xad300bd9: // VIK 01 by L.i.N. (PD)
		case 0x315917d4: // Woody Pop - Shinjinrui no Block Kuzushi (J) [!]
		case 0x4b065dcc: // ZEXALL Docs v0.15 by Maxim & Eric Quinn (PD)
		case 0x05f471de: // ZEXALL V0.12SDSC by Maxim & Eric Quinn (PD)
		case 0x38e4b272: // ZEXALL V0.12VDP by Maxim & Eric Quinn (PD)
		case 0x5e9448dc: // ZEXALL V0.13SDSC by Maxim & Eric Quinn (PD)
		case 0x39078689: // ZEXALL V0.13VDP by Maxim & Eric Quinn (PD)
		case 0xcbe40c4c: // ZEXALL V0.15SDSC by Maxim & Eric Quinn (PD)
		case 0x7ce4ceda: // ZEXALL V0.15VDP by Maxim & Eric Quinn (PD)
		case 0x7e38a5db: // ZEXALL V0.15.1SDSC by BorSanZa (PD)
		case 0xc938674d: // ZEXALL V0.15.1VDP by BorSanZa (PD)
		case 0xa463ddfa: // Zoom Effect #1_1 by Charles MacDonald (PD)
		case 0xb97e110a: // Zoom Effect #1_2 by Charles MacDonald (PD)
		case 0x980fdc4b: // Zoom Effect #1_3 by Charles MacDonald (PD)
			GetCPU()->SetMapper(awui::Emulation::MasterSystem::MAPPER_NONE);
			break;
	}
}

void MasterSystem::OnTick() {
	long long now = DateTime::GetNow().GetTicks();
	if ((now - m_lastTick) > 10000000) {
		m_lastTick = now;
		m_actual++;
		if (m_last < m_actual)
			m_last = m_actual;
		m_cpu->SaveState(m_savedData[m_actual % TOTALSAVED]);
	}

	m_cpu->OnTick();
}

void MasterSystem::RunOpcode() {
	m_cpu->RunOpcode();
}

Motherboard * MasterSystem::GetCPU() {
	return m_cpu;
}

void MasterSystem::OnPaint(GL* gl) {
	uint8_t c, r, g, b;
	uint8_t color[4] {0, 85, 170, 255};
	VDP * screen = m_cpu->GetVDP();

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
			c = screen->GetPixel(x, y);
			r = color[c & 0x3];
			g = color[(c >> 2) & 0x3];
			b = color[(c >> 4) & 0x3];
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
			VDP * screen = m_cpu->GetVDP();
			screen->SetShowBorder(!screen->GetShowBorder());
			screen->Clear();
			ret = true;
			}
			break;
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

bool MasterSystem::RefreshButtons(JoystickButtonEventArgs* e) {
	bool ret = false;

	uint32_t buttons = e->GetButtons();
	// printf("Buttons: %x\n", buttons);
	uint8_t masterButtons = ((buttons & JoystickButtons::JOYSTICK_BUTTON_DPAD_UP)?    0x01 : 0) |
							((buttons & JoystickButtons::JOYSTICK_BUTTON_DPAD_DOWN)?  0x02 : 0) |
							((buttons & JoystickButtons::JOYSTICK_BUTTON_DPAD_RIGHT)? 0x08 : 0) |
							((buttons & JoystickButtons::JOYSTICK_BUTTON_DPAD_LEFT)?  0x04 : 0) |
							((buttons & JoystickButtons::JOYSTICK_BUTTON_Y)?          (m_invertButtons ? 0x20 : 0x10) : 0) |
							((buttons & JoystickButtons::JOYSTICK_BUTTON_A)?          (m_invertButtons ? 0x20 : 0x10) : 0) |
							((buttons & JoystickButtons::JOYSTICK_BUTTON_B)?          (m_invertButtons ? 0x10 : 0x20) : 0) |
							((buttons & JoystickButtons::JOYSTICK_BUTTON_X)?          (m_invertButtons ? 0x10 : 0x20) : 0) |
							((buttons & JoystickButtons::JOYSTICK_BUTTON_START)?      0x40 : 0);

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

bool MasterSystem::OnJoystickButtonDown(JoystickButtonEventArgs* e) {
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

bool MasterSystem::OnJoystickButtonUp(JoystickButtonEventArgs* e) {
	if (e->GetButton() & JoystickButtons::JOYSTICK_BUTTON_START) {
		Pause(false);
		return true;
	}

	return RefreshButtons(e);
}

bool MasterSystem::OnJoystickAxisMotion(JoystickAxisMotionEventArgs* e) {
	bool ret = false;

	uint8_t masterButtons = ((e->GetAxisY() < -DEADZONE)?    0x01 : 0) |
							((e->GetAxisY() >  DEADZONE)?    0x02 : 0) |
							((e->GetAxisX() < -DEADZONE)?    0x04 : 0) |
							((e->GetAxisX() >  DEADZONE)?    0x08 : 0);

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
			m_cpu->CallPaused();
		}
	} else {
		m_pause = false;
	}
}

void MasterSystem::SetSoundEnabled(bool mode) {
	SoundSDL::Instance().SetPlayingSound(mode ? m_cpu->GetSound() : 0);
}

void MasterSystem::RefreshPads() {
	m_cpu->SetPad1(0xFF & (~(~m_keys1 | ~m_joys1 | ~m_axis1)));
	m_cpu->SetPad2(0xFF & (~(~m_keys2 | ~m_joys2 | ~m_axis2)));
}
