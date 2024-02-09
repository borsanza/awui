/**
 * awui/Windows/Emulators/MasterSystem.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "MasterSystem.h"

#include <awui/Drawing/Image.h>
#include <awui/DateTime.h>
#include <awui/Emulation/MasterSystem/Motherboard.h>
#include <awui/Emulation/MasterSystem/VDP.h>
#include <awui/Emulation/MasterSystem/SoundSDL.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Emulators/DebuggerSMS.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;
using namespace awui::Emulation::MasterSystem;

MasterSystem::MasterSystem() {
	this->_invertKeys = false;
	this->SetSize(1, 1);
	this->_image = new Drawing::Image(1, 1);
	this->_cpu = new Motherboard();
	this->_pause = false;

	this->_first = -1;
	this->_last = -1;
	this->_actual = -1;
	this->_lastTick = 0;
	this->_debugger = NULL;

	for (int i = 0; i < TOTALSAVED; i++)
		this->_savedData[i] = (uint8_t *) calloc (Motherboard::GetSaveSize(), sizeof(uint8_t));
}

MasterSystem::~MasterSystem() {
	for (int i = 0; i < TOTALSAVED; i++)
		free(this->_savedData[i]);

	delete this->_cpu;
	delete this->_image;
}

bool MasterSystem::IsClass(Classes objectClass) const {
	if (objectClass == Classes::MasterSystem) {
		return true;
	}

	return ArcadeContainer::IsClass(objectClass);
}

void MasterSystem::LoadRom(const String file) {
	this->SetName(file);
	this->_cpu->LoadRom(file);
	this->_first = 0;
	this->_last = 0;
	this->_actual = 0;
	this->_lastTick = DateTime::GetNow().GetTicks();
	this->_cpu->SaveState(this->_savedData[this->_actual]);

	uint32_t crc = this->GetCRC32();
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
			this->GetCPU()->SetMapper(awui::Emulation::MasterSystem::MAPPER_NONE);
			break;
	}

	switch (crc) {
		case 0xaed9aac4: // Alex Kidd in Miracle World (UEB) (V1.1) [!]
			this->SetInvertKeys(true);
			break;
	}
}

void MasterSystem::OnTick() {
	long long now = DateTime::GetNow().GetTicks();
	if ((now - this->_lastTick) > 10000000) {
		this->_lastTick = now;
		this->_actual++;
		if (this->_last < this->_actual)
			this->_last = this->_actual;
		this->_cpu->SaveState(this->_savedData[this->_actual % TOTALSAVED]);
	}

	this->_cpu->OnTick();
}

void MasterSystem::RunOpcode() {
	this->_cpu->RunOpcode();
}

Motherboard * MasterSystem::GetCPU() {
	return this->_cpu;
}

void MasterSystem::OnPaint(GL* gl) {
	uint8_t c, r, g, b;
	uint8_t color[4] {0, 85, 170, 255};
	VDP * screen = this->_cpu->GetVDP();

//	¿Lo rellenamos con el registro 7?
//	c = screen->GetBackColor();
//	r = color[c & 0x3];
//	g = color[(c >> 2) & 0x3];
//	b = color[(c >> 4) & 0x3];
//	this->SetBackColor(Color::FromArgb(255, r, g, b));

	int width = screen->GetVisualWidth();
	int height = screen->GetVisualHeight();

	if ((width != this->_image->GetWidth()) || (height != this->_image->GetHeight())) {
		delete this->_image;
		this->_image = new Drawing::Image(width, height);
	}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			c = screen->GetPixel(x, y);
			r = color[c & 0x3];
			g = color[(c >> 2) & 0x3];
			b = color[(c >> 4) & 0x3];
			this->_image->SetPixel(x, y, r, g, b);
		}
	}

	this->_image->Update();

	float w = width;
	float h = height;
	float ratio = w / h;
	w = this->GetWidth();
	h = this->GetWidth() / ratio;

	if (h > this->GetHeight()) {
		h = this->GetHeight();
		w = this->GetHeight() * ratio;
	}

	int ratio2 = w / width;
	if (ratio2 >= 1) {
		w = width * ratio2;
		h = height * ratio2;
	}

	GL::DrawImageGL(this->_image, int(this->GetWidth() - w) >> 1, int(this->GetHeight() - h) >> 1, w, h);
}

bool MasterSystem::OnKeyPress(Keys::Enum key) {
	if (key == Keys::Key_B) {
		VDP * screen = this->_cpu->GetVDP();
		screen->SetShowBorder(!screen->GetShowBorder());
		screen->Clear();;
	}

	if ((this->_debugger) && (key == Keys::Key_D))
		this->_debugger->SetShow(!this->_debugger->GetShow());

	if (key == Keys::Key_BACKSPACE)
		this->_cpu->Reset();

	return true;
}

uint32_t MasterSystem::GetCRC32() {
	return this->_cpu->GetCRC32();
}

uint8_t MasterSystem::GetPad(int which) const {
	uint32_t buttons;
	switch (which) {
		default:
		case 0:
			buttons = Form::GetButtonsPad1();
			break;
		case 1:
			buttons = Form::GetButtonsPad2();
			break;
	}

	uint8_t pad = 0x00;

	if (!(buttons & RemoteButtons::Up))
		pad |= 0x01;

	if (!(buttons & RemoteButtons::Down))
		pad |= 0x02;

	if (!(buttons & RemoteButtons::Left))
		pad |= 0x04;

	if (!(buttons & RemoteButtons::Right))
		pad |= 0x08;

	if (!(buttons & RemoteButtons::Ok))
		pad |= _invertKeys ? 0x20 : 0x10;

	if (!(buttons & RemoteButtons::Play))
		pad |= _invertKeys ? 0x10 : 0x20;

	if (!(buttons & RemoteButtons::Pause))
		pad |= 0x40;

	return pad;
}

bool MasterSystem::OnRemoteKeyPress(int which, RemoteButtons::Enum button) {
	bool ret = false;
	if (button & RemoteButtons::Button5) {
		this->_lastTick = DateTime::GetNow().GetTicks();
		this->_actual--;
		if (this->_actual < this->_first)
			this->_actual = this->_first;
		this->_cpu->LoadState(this->_savedData[this->_actual % TOTALSAVED]);
		ret = true;
	}

	if (button & RemoteButtons::Button6) {
		this->_lastTick = DateTime::GetNow().GetTicks();
		this->_actual++;
		if (this->_actual > this->_last)
			this->_actual = this->_last;
		this->_cpu->LoadState(this->_savedData[this->_actual % TOTALSAVED]);
		ret = true;
	}

	if ((Form::GetButtonsPad1() & RemoteButtons::Button5) && (button & RemoteButtons::Button6)) {
		this->_cpu->Reset();
		ret = true;
	}

	uint8_t pad1 = this->GetPad(0);
	uint8_t pad2 = this->GetPad(1);
	this->_cpu->SetPad1(pad1);
	this->_cpu->SetPad2(pad2);

	if (ret)
		return ret;

	bool paused = (((pad1 & 0x40) == 0) | ((pad2 & 0x40) == 0));
	if (paused) {
		if (!this->_pause) {
			this->_pause = true;
			this->_cpu->CallPaused();
		}
	}

	if (this->_debugger && (button & RemoteButtons::Button4))
		this->_debugger->SetShow(!this->_debugger->GetShow());

	return true;
}

bool MasterSystem::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	if (ArcadeContainer::OnRemoteKeyUp(which, button))
		return true;

	uint8_t pad1 = this->GetPad(0);
	uint8_t pad2 = this->GetPad(1);
	this->_cpu->SetPad1(pad1);
	this->_cpu->SetPad2(pad2);
	bool paused = (((pad1 & 0x40) == 0) | ((pad2 & 0x40) == 0));
	if (!paused)
		this->_pause = false;

	return true;
}

void MasterSystem::SetSoundEnabled(bool mode) {
	SoundSDL::Instance()->SetPlayingSound(mode ? this->_cpu->GetSound() : 0);
}
