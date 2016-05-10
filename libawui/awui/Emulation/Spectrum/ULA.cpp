/**
 * awui/Emulation/Spectrum/ULA.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "ULA.h"

#include <awui/Drawing/Image.h>
#include <awui/Emulation/Spectrum/Motherboard.h>
#include <assert.h>
#include <string.h>

using namespace awui::Emulation::Spectrum;

/*
16384 Ciclos - Borde superior


*** Ciclos ***
 24:  48 pixeles borde izquierdo
128: 256 pixeles
 24:  48 pixeles borde derecho
 48:     sincronismo horizontal
-------------------
224 ciclos

48
192
48
------------
288 Lineas



349440 Ciclos = 312 lineas * 224 ciclos * 50 frames



 60 lineas
192 lineas
 60 lineas
----------
312 lineas


resolucion 352x288
352 = 48 + 256 + 48
288 = 48 + 192 + 48




Por ahi leo que un televisor tiene 720x540 de resolucion
Tienen en cuenta que un pixel es 2x2
Asi que la resolucion real de un televisor es 360x270
*/

ULA::ULA() {
	this->d._line = 0;
	this->d._col = 0;
	this->d._backcolor = 0;
	this->d._blinkCount = 0;
	this->d._blink = false;

	this->_image = new Drawing::Image(SPECTRUM_VIDEO_WIDTH_VISUAL, SPECTRUM_VIDEO_HEIGHT_VISUAL);

	this->Reset();
}

ULA::~ULA() {
	delete this->_image;
}

void ULA::Reset() {
	memset(this->d._vram, 0, 16384 * sizeof(uint8_t));
	// Me gusta no vaciar la memoria de video, se parece mas al reset que se producia en el spectrum
	// cuando se pulsaba el boton, queda mas realista
	// memset(this->d._data, 0, SPECTRUM_VIDEO_WIDTH_TOTAL * SPECTRUM_VIDEO_HEIGHT_TOTAL * sizeof(uint8_t));
	this->_image->Clear();
}

/* VSYNC
0	191	192	Video Display
192	247	56	Bottom Border
248	263	16	Vertical Sync
264	311	48	Top Border

312 Lines
*/

/* HSYNC
0 (0)	127 (255)	Video
128 (256)	151 (303)	Right Border
152 (304)	165 (331)	HSync
152 (304)	199 (399)	Blank
200 (400)	223 (447)	Left Border

448 Pixeles = 224 States
48+256+48
*/

void ULA::CalcNextPixel(uint16_t * col, uint16_t * line, bool * hsync, bool * vsync) {
	*hsync = false;
	*vsync = false;

	*col = (*col + 1) % SPECTRUM_VIDEO_WIDTH_TOTAL;

	if (*col == (SPECTRUM_VIDEO_WIDTH + SPECTRUM_VIDEO_WIDTH_RIGHT + (SPECTRUM_VIDEO_WIDTH_SYNC >> 1)))
		*line = (*line + 1) % SPECTRUM_VIDEO_HEIGHT_TOTAL;

	if (*col == (SPECTRUM_VIDEO_WIDTH + SPECTRUM_VIDEO_WIDTH_RIGHT)) {
		this->d._lastbackcolor = this->d._backcolor;
		*hsync = true;
	}

	if ((*col == 0) && (*line == (SPECTRUM_VIDEO_HEIGHT + SPECTRUM_VIDEO_HEIGHT_BOTTOM)))
		*vsync = true;
}

bool ULA::OnTick(uint32_t counter) {
	bool vsync;
	bool hsync;

	this->CalcNextPixel(&this->d._col, &this->d._line, &hsync, &vsync);

	uint16_t col = this->d._col;
	uint16_t line = this->d._line;
	uint32_t pos = col + (line * SPECTRUM_VIDEO_WIDTH_TOTAL);

	if ((pos == 0) && (++(this->d._blinkCount) >= 16)) {
		this->d._blinkCount = 0;
		this->d._blink = !this->d._blink;
	}

	if (col == (SPECTRUM_VIDEO_WIDTH + SPECTRUM_VIDEO_WIDTH_RIGHT))
		if (line == (SPECTRUM_VIDEO_HEIGHT + SPECTRUM_VIDEO_HEIGHT_BOTTOM))
			this->DrawImage();

	if ((col < SPECTRUM_VIDEO_WIDTH) && (line < SPECTRUM_VIDEO_HEIGHT)) {
		uint8_t newY = (line & 0xC0) | ((line & 0x38) >> 3) | ((line & 0x7) << 3);
		uint8_t v = this->d._vram[(col >> 3) + (newY * 32)];
		int bit = 7 - (col & 0x7);
		bool active = ((v & (1 << bit)) != 0) ? true : false;

		uint8_t reg = this->d._vram[0x1800 + (col >> 3) + ((line >> 3) * 32)];

		if (reg & 0x80 && this->d._blink)
			active = !active;

		uint8_t color;
		if (active)
			color = ((reg & 0x40) >> 3) |  (reg & 0x07);
		else
			color = (reg & 0x78) >> 3;

		this->d._data[pos] = color;
	} else {
/*
		Es mucho mas rapido asignarlo simplemente
		uint16_t vsync_begin = SPECTRUM_VIDEO_WIDTH + SPECTRUM_VIDEO_WIDTH_RIGHT;
		uint16_t vsync_end = vsync_begin + SPECTRUM_VIDEO_WIDTH_SYNC;
		uint16_t hsync_begin = SPECTRUM_VIDEO_HEIGHT + SPECTRUM_VIDEO_HEIGHT_BOTTOM;
		uint16_t hsync_end = hsync_begin + SPECTRUM_VIDEO_HEIGHT_SYNC;
		bool border = !(((col >= vsync_begin) && (col < vsync_end)) || ((line >= hsync_begin) && (line < hsync_end)));
		if (border)
*/
		this->d._data[pos] = this->d._lastbackcolor;
	}

	return vsync;
}

int ULA::GetSaveSize() {
	return sizeof(ULA::saveData);
}

void ULA::LoadState(uint8_t * data) {
	memcpy(&this->d, data, sizeof(ULA::saveData));
}

void ULA::SaveState(uint8_t * data) {
	memcpy(data, &this->d, sizeof(ULA::saveData));
}

void ULA::DrawImage() {
	uint8_t c;
	uint32_t color;
	uint16_t width = SPECTRUM_VIDEO_WIDTH + SPECTRUM_VIDEO_WIDTH_RIGHT;
	uint16_t height = SPECTRUM_VIDEO_HEIGHT + SPECTRUM_VIDEO_HEIGHT_BOTTOM;

	// VIDEO + Border derecho + Borde Inferior
	for (int x = 0; x < width; x++)
		for (int y = 0; y < height; y++) {
			c = this->d._data[(y * SPECTRUM_VIDEO_WIDTH_TOTAL) + x];
			color = _colors[c];
			this->_image->SetPixel(x + SPECTRUM_VIDEO_WIDTH_LEFT, y + SPECTRUM_VIDEO_HEIGHT_TOP, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
		}

	// Borde superior izquierdo
	for (int x = 0; x < SPECTRUM_VIDEO_WIDTH_LEFT; x++)
		for (int y = 0; y < SPECTRUM_VIDEO_HEIGHT_TOP; y++) {
			int newX = SPECTRUM_VIDEO_WIDTH_TOTAL - SPECTRUM_VIDEO_WIDTH_LEFT + x;
			int newY = SPECTRUM_VIDEO_HEIGHT_TOTAL - SPECTRUM_VIDEO_HEIGHT_TOP + y;
			c = this->d._data[(newY * SPECTRUM_VIDEO_WIDTH_TOTAL) + newX];
			color = _colors[c];
			this->_image->SetPixel(x, y, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
		}

	// Borde superior
	for (int x = 0; x < width; x++)
		for (int y = 0; y < SPECTRUM_VIDEO_HEIGHT_TOP; y++) {
			int newY = SPECTRUM_VIDEO_HEIGHT_TOTAL - SPECTRUM_VIDEO_HEIGHT_TOP + y;
			c = this->d._data[(newY * SPECTRUM_VIDEO_WIDTH_TOTAL) + x];
			color = _colors[c];
			this->_image->SetPixel(x + SPECTRUM_VIDEO_WIDTH_LEFT, y, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
		}

	// Borde Izquierdo
	for (int x = 0; x < SPECTRUM_VIDEO_WIDTH_LEFT; x++)
		for (int y = 0; y < height; y++) {
			int newX = SPECTRUM_VIDEO_WIDTH_TOTAL - SPECTRUM_VIDEO_WIDTH_LEFT + x;
			c = this->d._data[(y * SPECTRUM_VIDEO_WIDTH_TOTAL) + newX];
			color = _colors[c];
			this->_image->SetPixel(x, y + SPECTRUM_VIDEO_HEIGHT_TOP, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
		}

	this->_image->Update();
}
