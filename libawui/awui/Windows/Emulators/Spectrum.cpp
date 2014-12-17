/*
 * awui/Windows/Emulators/Spectrum.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Spectrum.h"

#include <awui/Drawing/Image.h>
#include <awui/DateTime.h>
#include <awui/Emulation/Spectrum/CPU.h>
#include <awui/Emulation/Spectrum/SoundSDL.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/OpenGL/GL.h>
#include <awui/Windows/Emulators/DebuggerSMS.h>

using namespace awui;
using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;
using namespace awui::Emulation::Spectrum;

Spectrum::Spectrum() {
	this->SetSize(1, 1);
	this->_image = new Drawing::Image(1, 1);
	this->_cpu = new CPU();
	this->SetTabStop(true);
	this->_multiply = 1;
	this->_canChangeControl = true;
	this->_pause = false;

	this->_first = -1;
	this->_last = -1;
	this->_actual = -1;
	this->_lastTick = 0;

	for (int i = 0; i < TOTALSAVED; i++)
		this->_savedData[i] = (uint8_t *) calloc (CPUInst::GetSaveSize(), sizeof(uint8_t));
}

Spectrum::~Spectrum() {
	for (int i = 0; i < TOTALSAVED; i++)
		free(this->_savedData[i]);

	delete this->_cpu;
	delete this->_image;
}

int Spectrum::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Spectrum)
		return 1;

	return Control::IsClass(objectClass);
}

void Spectrum::LoadRom(const String file) {
	this->SetName(file);
	this->_cpu->LoadRom(file);
	this->_first = 0;
	this->_last = 0;
	this->_actual = 0;
	this->_lastTick = DateTime::GetNow().GetTicks();
	this->_cpu->SaveState(this->_savedData[this->_actual]);
}

void Spectrum::CheckLimits() {
}

void Spectrum::OnTick() {
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

void Spectrum::RunOpcode() {
	this->_cpu->RunOpcode();
}

CPU * Spectrum::GetCPU() {
	return this->_cpu;
}


// Interface:
// 256x192: 1800 = 6144 bytes = (32 x 8) x 192 bits

// 4000-57FF Spectrum bitmap
// 5800-5AFF Spectrum attributes
// 7000 attribute lookup: 256 bytes.  64 colors of (paper, ink)
// 7100 pixel stretch, 16 bytes.

void Spectrum::OnPaint(GL* gl) {
//	¿Lo rellenamos con el registro 7?
//	c = screen->GetBackColor();
//	r = color[c & 0x3];
//	g = color[(c >> 2) & 0x3];
//	b = color[(c >> 4) & 0x3];
//	this->SetBackColor(Color::FromArgb(255, r, g, b));
	int width = 320;
	int height = 240;
	int pos;

	if ((width != this->_image->GetWidth()) || (height != this->_image->GetHeight())) {
		delete this->_image;
		this->_image = new Drawing::Image(width, height);
		this->SetSize(width * this->_multiply, height * this->_multiply);
	}

	for (uint16_t y = 0; y < 240; y++) {
		for (int x = 0; x < 32; x++) {
			this->_image->SetPixel(x, y, 255, 255, 255);
			this->_image->SetPixel(x + 288, y, 255, 255, 255);
		}
	}

	for (uint16_t y = 0; y < 24; y++) {
		for (int x = 0; x < 320; x++) {
			this->_image->SetPixel(x, y, 255, 255, 255);
			this->_image->SetPixel(x, y + 216, 255, 255, 255);
		}
	}

	for (uint16_t y = 0; y < 192; y++) {
		for (int x = 0; x < 256; x++) {
			uint8_t newY = (y & 0xC0) | ((y & 0x38) >> 3) | ((y & 0x7) << 3);
			pos = 0x4000 + (x >> 3) + (newY * 32);
			uint8_t v = this->_cpu->ReadMemory(pos);
			int bit = 8 - (x & 0x7);
			v = ((v & (1 << bit)) != 0) ? 0 : 255;
			// printf("%.4X: %dx%d\n", pos, x, y);
			this->_image->SetPixel(x + 32, y + 24, v, v, v);
		}
	}

	this->_image->Update();

	GL::DrawImageGL(this->_image, 0, 0, this->GetWidth(), this->GetHeight());
}

bool Spectrum::OnKeyPress(Keys::Enum key) {
	if (key == Keys::Key_B) {
/*
		VDP * screen = this->_cpu->GetVDP();
		screen->SetShowBorder(!screen->GetShowBorder());
		screen->Clear();;
*/
	}

	if (key == Keys::Key_BACKSPACE)
		this->_cpu->Reset();

	return true;
}

void Spectrum::SetMultiply(int multiply) {
	this->_multiply = multiply;
}

uint32_t Spectrum::GetCRC32() {
	return this->_cpu->GetCRC32();
}

uint8_t Spectrum::GetPad(int which) const {
	if (this->_canChangeControl)
		return 0xFF;

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

bool Spectrum::OnRemoteKeyPress(int which, RemoteButtons::Enum button) {
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

	if (this->_canChangeControl)
		return Control::OnRemoteKeyPress(which, button);

	return true;
}

bool Spectrum::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	if ((button & RemoteButtons::Pause) || (button & RemoteButtons::Ok) || (button & RemoteButtons::Play))
		this->_canChangeControl = false;

	if (button & RemoteButtons::Menu)
		this->_canChangeControl = true;

	uint8_t pad1 = this->GetPad(0);
	uint8_t pad2 = this->GetPad(1);
	this->_cpu->SetPad1(pad1);
	this->_cpu->SetPad2(pad2);
	bool paused = (((pad1 & 0x40) == 0) | ((pad2 & 0x40) == 0));
	if (!paused)
		this->_pause = false;

	if (this->_canChangeControl)
		return Control::OnRemoteKeyUp(which, button);

	return true;
}

void Spectrum::SetSoundEnabled(bool mode) {
	SoundSDL::Instance()->SetPlayingSound(this->_cpu->GetSound());
}
