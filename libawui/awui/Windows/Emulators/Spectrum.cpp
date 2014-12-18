/*
 * awui/Windows/Emulators/Spectrum.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Spectrum.h"

#include <awui/Drawing/Image.h>
#include <awui/DateTime.h>
#include <awui/Emulation/Spectrum/Mainboard.h>
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
	this->_cpu = new Mainboard();
	this->SetTabStop(true);
	this->_multiply = 1;
	this->_canChangeControl = true;
	this->_pause = false;

	this->_first = -1;
	this->_last = -1;
	this->_actual = -1;
	this->_lastTick = 0;

	this->_colors[0]  = 0x000000;
	this->_colors[1]  = 0x00007F;
	this->_colors[2]  = 0x7F0000;
	this->_colors[3]  = 0x7F007F;
	this->_colors[4]  = 0x007F00;
	this->_colors[5]  = 0x007F7F;
	this->_colors[6]  = 0x7F7F00;
	this->_colors[7]  = 0x7F7F7F;
	this->_colors[8]  = 0x000000;
	this->_colors[9]  = 0x0000FF;
	this->_colors[10] = 0xFF0000;
	this->_colors[11] = 0xFF00FF;
	this->_colors[12] = 0x00FF00;
	this->_colors[13] = 0x00FFFF;
	this->_colors[14] = 0xFFFF00;
	this->_colors[15] = 0xFFFFFF;
}

Spectrum::~Spectrum() {
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
	this->_cpu->OnTick();
}

Mainboard * Spectrum::GetCPU() {
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

			uint8_t color = this->_cpu->ReadMemory(0x5800 + (x >> 3) + ((newY >> 3) * 32));

			this->_image->SetPixel(x + 32, y + 24, v, v, v);
		}
	}

	this->_image->Update();

	GL::DrawImageGL(this->_image, 0, 0, this->GetWidth(), this->GetHeight());
}

void Spectrum::SetMultiply(int multiply) {
	this->_multiply = multiply;
}
