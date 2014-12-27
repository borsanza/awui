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

ULA::ULA(Motherboard * cpu) {
	this->d._width = 256;
	this->d._height = 192;

	this->_cpu = cpu;
	this->d._line = 0;
	this->d._col = 0;
	this->d._interrupt = false;

	this->Reset();
}

ULA::~ULA() {
}

void ULA::Reset() {
	this->Clear();
	memset(this->d._vram, 0, 16384 * sizeof(uint8_t));
}

void ULA::Clear() {
	memset(this->d._data, 0, 320 * 240 * sizeof(uint8_t));
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
	return 320;
}

uint16_t ULA::GetTotalHeight() const {
	return 240;
}

uint16_t ULA::GetVisualWidth() const {
	return 320;
}

uint16_t ULA::GetVisualHeight() const {
	return 240;
}

uint8_t ULA::GetPixel(uint16_t x, uint16_t y) const {
	return this->d._data[(y * this->GetVisualWidth()) + x];
}

bool ULA::IsVSYNC(uint16_t line) const {
	return 219 == line;
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

	if (*col == 271) {
		// Esto es una chapuza, tengo que revisar al milimetro todo el tema de interrupciones de hsync y vsync
		if (*line == this->GetTotalHeight() - 43)
			this->d._interrupt = true;
	}

	// 256 Active Display +
	// 15 Right Border +
	// 8 Right Blank <- Suponemos que cambia al final del Right Blank
	// 26 HSync
	if (*col == 279) {
		*hsync = true;
		*vsync = this->IsVSYNC(*line);
	}
}

void ULA::OnTickBorder() {
	int x = 0;
	int y = 0;
	bool draw = false;
	if ((this->d._col >= this->GetWidth()) || (this->d._line >= this->GetHeight())) {
		if (this->d._col >= (this->GetTotalWidth() - 32)) {
			draw = true;
			x = this->d._col - (this->GetTotalWidth() - 32);
		} else {
			if (this->d._col < this->GetWidth() + 32) {
				draw = true;
				x = 32 + this->d._col;
			}
		}

		if (this->d._line >= (this->GetTotalHeight() - 24)) {
			draw = true;
			y = this->d._line - (this->GetTotalHeight() - 24);
		} else {
			if (this->d._line < (this->GetHeight() + 24)) {
				draw = true;
				y = 24 + this->d._line;
			}
		}
	}

	if (draw)
		this->d._data[x + (y * this->GetVisualWidth())] = this->d._backcolor;
}

bool ULA::OnTick(uint32_t counter) {
	bool ret = false;
	bool hsync, vsync;

	this->CalcNextPixel(&this->d._col, &this->d._line, &hsync, &vsync);
	if (vsync)
		ret = true;

	if ((this->d._col < this->d._width) && (this->d._line < this->d._height)) {
//		this->d._data[pos] = this->GetBackgroundPixel(sprite, col & 0x7, line & 0x7, flipx, flipy, otherPalette, priority);
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

uint8_t ULA::GetBackColor() const {
	return this->d._backcolor;
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
