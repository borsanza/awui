/*
 * awui/Windows/Emulators/Spectrum.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Spectrum.h"

#include <awui/Drawing/Image.h>
#include <awui/DateTime.h>
#include <awui/Emulation/Spectrum/Motherboard.h>
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
	this->_cpu = new Motherboard();
	this->SetTabStop(true);
	this->_multiply = 1;
	this->_canChangeControl = true;
	this->_pause = false;

	this->_first = -1;
	this->_last = -1;
	this->_actual = -1;
	this->_lastTick = 0;

	this->_colors[0]  = 0x000000;
	this->_colors[1]  = 0x0000BF;
	this->_colors[2]  = 0xBF0000;
	this->_colors[3]  = 0xBF00BF;
	this->_colors[4]  = 0x00BF00;
	this->_colors[5]  = 0x00BFBF;
	this->_colors[6]  = 0xBFBF00;
	this->_colors[7]  = 0xBFBFBF;
	this->_colors[8]  = 0x000000;
	this->_colors[9]  = 0x0000FF;
	this->_colors[10] = 0xFF0000;
	this->_colors[11] = 0xFF00FF;
	this->_colors[12] = 0x00FF00;
	this->_colors[13] = 0x00FFFF;
	this->_colors[14] = 0xFFFF00;
	this->_colors[15] = 0xFFFFFF;

	for (int i = 0; i < TOTALSAVED; i++)
		this->_savedData[i] = (uint8_t *) calloc (Motherboard::GetSaveSize(), sizeof(uint8_t));
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

Motherboard * Spectrum::GetCPU() {
	return this->_cpu;
}

// Interface:
// 256x192: 1800 = 6144 bytes = (32 x 8) x 192 bits
// 300 = 768 bytes = 32 x 24 x 8 bits
// 4000-57FF Spectrum bitmap
// 5800-5AFF Spectrum attributes
// 7000 attribute lookup: 256 bytes.  64 colors of (paper, ink)
// 7100 pixel stretch, 16 bytes.

void Spectrum::OnPaint(GL* gl) {
	int width = 320;
	int height = 240;
	int pos;

	if ((width != this->_image->GetWidth()) || (height != this->_image->GetHeight())) {
		delete this->_image;
		this->_image = new Drawing::Image(width, height);
		this->SetSize(width * this->_multiply, height * this->_multiply);
	}

	uint32_t color = this->_colors[this->_cpu->GetBackgroundColor()];

	for (uint16_t y = 0; y < 240; y++) {
		for (int x = 0; x < 32; x++) {
			this->_image->SetPixel(x, y, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color >> 0) & 0xFF);
			this->_image->SetPixel(x + 288, y, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color >> 0) & 0xFF);
		}
	}

	for (uint16_t y = 0; y < 24; y++) {
		for (int x = 0; x < 320; x++) {
			this->_image->SetPixel(x, y, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color >> 0) & 0xFF);
			this->_image->SetPixel(x, y + 216, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color >> 0) & 0xFF);
		}
	}

	for (uint16_t y = 0; y < 192; y++) {
		for (int x = 0; x < 256; x++) {
			uint8_t newY = (y & 0xC0) | ((y & 0x38) >> 3) | ((y & 0x7) << 3);
			pos = 0x4000 + (x >> 3) + (newY * 32);
			uint8_t v = this->_cpu->ReadMemory(pos);
			int bit = 7 - (x & 0x7);
			bool active = ((v & (1 << bit)) != 0) ? true : false;

			uint8_t reg = this->_cpu->ReadMemory(0x5800 + (x >> 3) + ((y >> 3) * 32));
			if (active) {
				color = this->_colors[((reg & 0x40) >> 3) |  (reg & 0x07)];
			} else {
				color = this->_colors[(reg & 0x78) >> 3];
			}

			if (color == 0xBF00BF)
				printf("%.2X\n", reg);

			this->_image->SetPixel(x + 32, y + 24, (color >> 16) & 0xFF, (color >> 8) & 0xFF, (color >> 0) & 0xFF);
		}
	}

	this->_image->Update();

	GL::DrawImageGL(this->_image, 0, 0, this->GetWidth(), this->GetHeight());
}

void Spectrum::SetMultiply(int multiply) {
	this->_multiply = multiply;
}

bool Spectrum::OnKeyPress(Keys::Enum key) {
//	if (key == Keys::Key_BACKSPACE) this->_cpu->Reset();

	switch (key) {
		case Keys::Key_Z: this->_cpu->OnKeyPress(0, 0x02); break;
		case Keys::Key_X: this->_cpu->OnKeyPress(0, 0x04); break;
		case Keys::Key_C: this->_cpu->OnKeyPress(0, 0x08); break;
		case Keys::Key_V: this->_cpu->OnKeyPress(0, 0x10); break;
		case Keys::Key_A: this->_cpu->OnKeyPress(1, 0x01); break;
		case Keys::Key_S: this->_cpu->OnKeyPress(1, 0x02); break;
		case Keys::Key_D: this->_cpu->OnKeyPress(1, 0x04); break;
		case Keys::Key_F: this->_cpu->OnKeyPress(1, 0x08); break;
		case Keys::Key_G: this->_cpu->OnKeyPress(1, 0x10); break;
		case Keys::Key_Q: this->_cpu->OnKeyPress(2, 0x01); break;
		case Keys::Key_W: this->_cpu->OnKeyPress(2, 0x02); break;
		case Keys::Key_E: this->_cpu->OnKeyPress(2, 0x04); break;
		case Keys::Key_R: this->_cpu->OnKeyPress(2, 0x08); break;
		case Keys::Key_T: this->_cpu->OnKeyPress(2, 0x10); break;
		case Keys::Key_1: this->_cpu->OnKeyPress(3, 0x01); break;
		case Keys::Key_2: this->_cpu->OnKeyPress(3, 0x02); break;
		case Keys::Key_3: this->_cpu->OnKeyPress(3, 0x04); break;
		case Keys::Key_4: this->_cpu->OnKeyPress(3, 0x08); break;
		case Keys::Key_5: this->_cpu->OnKeyPress(3, 0x10); break;
		case Keys::Key_0: this->_cpu->OnKeyPress(4, 0x01); break;
		case Keys::Key_9: this->_cpu->OnKeyPress(4, 0x02); break;
		case Keys::Key_8: this->_cpu->OnKeyPress(4, 0x04); break;
		case Keys::Key_7: this->_cpu->OnKeyPress(4, 0x08); break;
		case Keys::Key_6: this->_cpu->OnKeyPress(4, 0x10); break;
		case Keys::Key_P: this->_cpu->OnKeyPress(5, 0x01); break;
		case Keys::Key_O: this->_cpu->OnKeyPress(5, 0x02); break;
		case Keys::Key_I: this->_cpu->OnKeyPress(5, 0x04); break;
		case Keys::Key_U: this->_cpu->OnKeyPress(5, 0x08); break;
		case Keys::Key_Y: this->_cpu->OnKeyPress(5, 0x10); break;
		case Keys::Key_L: this->_cpu->OnKeyPress(6, 0x02); break;
		case Keys::Key_K: this->_cpu->OnKeyPress(6, 0x04); break;
		case Keys::Key_J: this->_cpu->OnKeyPress(6, 0x08); break;
		case Keys::Key_H: this->_cpu->OnKeyPress(6, 0x10); break;
		case Keys::Key_M: this->_cpu->OnKeyPress(7, 0x04); break;
		case Keys::Key_N: this->_cpu->OnKeyPress(7, 0x08); break;
		case Keys::Key_B: this->_cpu->OnKeyPress(7, 0x10); break;
		case Keys::Key_BACKSPACE:
			this->_cpu->OnKeyPress(0, 0x01);
			this->_cpu->OnKeyPress(4, 0x01);
			break;
		default: break;
	}

	return true;
}

bool Spectrum::OnKeyUp(Keys::Enum key) {
	switch (key) {
		case Keys::Key_Z: this->_cpu->OnKeyUp(0, 0x02); break;
		case Keys::Key_X: this->_cpu->OnKeyUp(0, 0x04); break;
		case Keys::Key_C: this->_cpu->OnKeyUp(0, 0x08); break;
		case Keys::Key_V: this->_cpu->OnKeyUp(0, 0x10); break;
		case Keys::Key_A: this->_cpu->OnKeyUp(1, 0x01); break;
		case Keys::Key_S: this->_cpu->OnKeyUp(1, 0x02); break;
		case Keys::Key_D: this->_cpu->OnKeyUp(1, 0x04); break;
		case Keys::Key_F: this->_cpu->OnKeyUp(1, 0x08); break;
		case Keys::Key_G: this->_cpu->OnKeyUp(1, 0x10); break;
		case Keys::Key_Q: this->_cpu->OnKeyUp(2, 0x01); break;
		case Keys::Key_W: this->_cpu->OnKeyUp(2, 0x02); break;
		case Keys::Key_E: this->_cpu->OnKeyUp(2, 0x04); break;
		case Keys::Key_R: this->_cpu->OnKeyUp(2, 0x08); break;
		case Keys::Key_T: this->_cpu->OnKeyUp(2, 0x10); break;
		case Keys::Key_1: this->_cpu->OnKeyUp(3, 0x01); break;
		case Keys::Key_2: this->_cpu->OnKeyUp(3, 0x02); break;
		case Keys::Key_3: this->_cpu->OnKeyUp(3, 0x04); break;
		case Keys::Key_4: this->_cpu->OnKeyUp(3, 0x08); break;
		case Keys::Key_5: this->_cpu->OnKeyUp(3, 0x10); break;
		case Keys::Key_0: this->_cpu->OnKeyUp(4, 0x01); break;
		case Keys::Key_9: this->_cpu->OnKeyUp(4, 0x02); break;
		case Keys::Key_8: this->_cpu->OnKeyUp(4, 0x04); break;
		case Keys::Key_7: this->_cpu->OnKeyUp(4, 0x08); break;
		case Keys::Key_6: this->_cpu->OnKeyUp(4, 0x10); break;
		case Keys::Key_P: this->_cpu->OnKeyUp(5, 0x01); break;
		case Keys::Key_O: this->_cpu->OnKeyUp(5, 0x02); break;
		case Keys::Key_I: this->_cpu->OnKeyUp(5, 0x04); break;
		case Keys::Key_U: this->_cpu->OnKeyUp(5, 0x08); break;
		case Keys::Key_Y: this->_cpu->OnKeyUp(5, 0x10); break;
		case Keys::Key_L: this->_cpu->OnKeyUp(6, 0x02); break;
		case Keys::Key_K: this->_cpu->OnKeyUp(6, 0x04); break;
		case Keys::Key_J: this->_cpu->OnKeyUp(6, 0x08); break;
		case Keys::Key_H: this->_cpu->OnKeyUp(6, 0x10); break;
		case Keys::Key_M: this->_cpu->OnKeyUp(7, 0x04); break;
		case Keys::Key_N: this->_cpu->OnKeyUp(7, 0x08); break;
		case Keys::Key_B: this->_cpu->OnKeyUp(7, 0x10); break;
		case Keys::Key_BACKSPACE:
			this->_cpu->OnKeyUp(0, 0x01);
			this->_cpu->OnKeyUp(4, 0x01);
			break;
		default: break;
	}

	return true;
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

	switch (button) {
		case RemoteButtons::Ok: this->_cpu->OnKeyPress(6, 0x01); break;
		default: break;
	}

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

//	uint8_t pad1 = this->GetPad(0);
//	uint8_t pad2 = this->GetPad(1);
//	this->_cpu->SetPad1(pad1);
//	this->_cpu->SetPad2(pad2);

	if (ret)
		return ret;

	if (this->_canChangeControl)
		return Control::OnRemoteKeyPress(which, button);

	return true;
}

bool Spectrum::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	switch (button) {
		case RemoteButtons::Ok: this->_cpu->OnKeyUp(6, 0x01); break;
		default: break;
	}

	if ((button & RemoteButtons::Pause) || (button & RemoteButtons::Ok) || (button & RemoteButtons::Play))
		this->_canChangeControl = false;

	if (button & RemoteButtons::Menu)
		this->_canChangeControl = true;

	uint8_t pad1 = this->GetPad(0);
	uint8_t pad2 = this->GetPad(1);
//	this->_cpu->SetPad1(pad1);
//	this->_cpu->SetPad2(pad2);
	bool paused = (((pad1 & 0x40) == 0) | ((pad2 & 0x40) == 0));
	if (!paused)
		this->_pause = false;

	if (this->_canChangeControl)
		return Control::OnRemoteKeyUp(which, button);

	return true;
}
