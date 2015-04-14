/*
 * awui/Emulation/Spectrum/ULA.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "ULA.h"

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
	this->d._width = 256;
	this->d._height = 192;

	this->d._line = 0;
	this->d._col = 0;
	this->d._interrupt = false;
	this->d._backcolor = 0;
	this->d._blinkCount = 0;
	this->d._blink = false;

	this->Reset();
}

ULA::~ULA() {
}

void ULA::Reset() {
	this->Clear();
	memset(this->d._vram, 0, 16384 * sizeof(uint8_t));
}

void ULA::Clear() {
	memset(this->d._data, 0, SPECTRUM_WIDTH * SPECTRUM_HEIGHT * sizeof(uint8_t));
}

uint8_t * ULA::GetVram() {
	return this->d._vram;
}

uint16_t ULA::GetLine() const {
	return this->d._line;
}

uint16_t ULA::GetColumn() const {
	return this->d._col;
}

uint16_t ULA::GetWidth() const {
	return this->d._width;
}

uint16_t ULA::GetHeight() const {
	return this->d._height;
}

void ULA::SetHeight(uint16_t height) {
	if (height != this->d._height)
		this->d._height = height;
}

uint16_t ULA::GetTotalWidth() const {
	return SPECTRUM_WIDTH;
}

uint16_t ULA::GetTotalHeight() const {
	return SPECTRUM_HEIGHT;
}

uint8_t ULA::GetPixel(uint16_t x, uint16_t y) const {
	return this->d._data[(y * this->GetTotalWidth()) + x];
}

bool ULA::IsVSYNC(uint16_t line) const {
	return 240 == line;
}

void ULA::CalcNextPixel(uint16_t * col, uint16_t * line, bool * hsync, bool * vsync) {
	*hsync = false;
	*vsync = false;

	(*col)++;
	if (*col == this->GetTotalWidth()) {
		*col = 0;
		(*line)++;
		if (*line == this->GetTotalHeight())
			*line = 0;
	}

	if (*col == 304) {
		// Esto es una chapuza, tengo que revisar al milimetro todo el tema de interrupciones de hsync y vsync
		if (*line == this->GetTotalHeight() - 48)
			this->d._interrupt = true;
	}

	// 256 Active Display +
	// 15 Right Border +
	// 8 Right Blank <- Suponemos que cambia al final del Right Blank
	// 26 HSync
	if (*col == 312) { // 279
		*hsync = true;
		*vsync = this->IsVSYNC(*line);
	}
}

void ULA::OnTickBorder() {
	int x = 0;
	int y = 0;
	bool draw = false;
	if ((this->d._col >= this->GetWidth()) || (this->d._line >= this->GetHeight())) {
		if (this->d._col >= (this->GetTotalWidth() - SPECTRUM_BORDER_WIDTH)) {
			draw = true;
			x = this->d._col - (this->GetTotalWidth() - SPECTRUM_BORDER_WIDTH);
		} else {
			if (this->d._col < this->GetWidth() + SPECTRUM_BORDER_WIDTH) {
				draw = true;
				x = SPECTRUM_BORDER_WIDTH + this->d._col;
			}
		}

		if (this->d._line >= (this->GetTotalHeight() - SPECTRUM_BORDER_HEIGHT)) {
			draw = true;
			y = this->d._line - (this->GetTotalHeight() - SPECTRUM_BORDER_HEIGHT);
		} else {
			if (this->d._line < (this->GetHeight() + SPECTRUM_BORDER_HEIGHT)) {
				draw = true;
				y = SPECTRUM_BORDER_HEIGHT + this->d._line;
			}
		}
	}

	if (draw)
		this->d._data[x + (y * this->GetTotalWidth())] = this->d._backcolor;
}

bool ULA::OnTick(uint32_t counter) {
	bool ret = false;
	bool hsync, vsync;

	this->CalcNextPixel(&this->d._col, &this->d._line, &hsync, &vsync);
	if (vsync)
		ret = true;

	if ((this->d._col == 0) && (this->d._line == 0)) {
		if (++(this->d._blinkCount) >= 16) {
			this->d._blinkCount = 0;
			this->d._blink = !this->d._blink;
		}
	}

	if ((this->d._col < this->d._width) && (this->d._line < this->d._height)) {
		uint16_t col = this->d._col;
		uint16_t line = this->d._line;
		uint8_t newY = (line & 0xC0) | ((line & 0x38) >> 3) | ((line & 0x7) << 3);
		uint8_t v = this->d._vram[(col >> 3) + (newY * 32)];
		int bit = 7 - (col & 0x7);
		bool active = ((v & (1 << bit)) != 0) ? true : false;

		uint8_t reg = this->d._vram[0x1800 + (col >> 3) + ((line >> 3) * 32)];

		if (reg & 0x80 && this->d._blink)
			active = !active;

		uint8_t color;
		if (active) {
			color = ((reg & 0x40) >> 3) |  (reg & 0x07);
		} else {
			color = (reg & 0x78) >> 3;
		}

		uint32_t pos = col + SPECTRUM_BORDER_WIDTH + ((line + SPECTRUM_BORDER_HEIGHT) * this->GetTotalWidth());
		this->d._data[pos] = color;
	} else {
		this->OnTickBorder();
	}

	return ret;
}

bool ULA::GetInterrupt() {
	if (this->d._interrupt) {
		this->d._interrupt = false;
		return true;
	}

	return false;
}

int ULA::GetSaveSize() {
	return sizeof(ULA::saveData);
}

void ULA::LoadState(uint8_t * data) {
	memcpy (&this->d, data, sizeof(ULA::saveData));
}

void ULA::SaveState(uint8_t * data) {
	memcpy (data, &this->d, sizeof(ULA::saveData));
}
