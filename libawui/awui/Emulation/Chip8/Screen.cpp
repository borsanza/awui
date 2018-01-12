/**
 * awui/Emulation/Chip8/Screen.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Screen.h"

#include <awui/String.h>

#include <stdlib.h>

using namespace awui::Emulation::Chip8;

Screen::Screen(uint16_t width, uint16_t height) {
	this->_width = width;
	this->_height = height;
	this->_data = (uint32_t *) malloc (sizeof(uint32_t *) * width * height);
	this->Clear();
}

Screen::~Screen() {
	free(this->_data);
}

void Screen::Clear() {
	uint16_t length = this->_width * this->_height;
	for (uint16_t i = 0; i < length; i++)
		this->_data[i] = 0;
}

bool Screen::SetPixelXOR(uint16_t x, uint16_t y, bool value) {
	x = x % this->_width;

	bool r = false;
	uint16_t offset = (y * this->_width) + x;

	bool oldValue = this->_data[offset];
	bool newValue = oldValue ^ value;

	if (oldValue != newValue) {
		if (newValue == 0)
			r = true;
		if (offset < (this->_width * this->_height))
			this->_data[offset] = newValue;
	}

	return r;
}

void Screen::SetPixel(uint16_t x, uint16_t y, uint32_t value) {
	if ((x >= this->_width) || (y >= this->_height))
		return;

	uint16_t offset = (y * this->_width) + x;


	uint8_t a = (value >> 24) & 0xFF;
	if (a == 255) {
		this->_data[offset] = value;
	} else {
		uint32_t oldvalue = this->_data[offset];
		float p = a / 255.0f;
		int16_t ro = (oldvalue >> 16) & 0xFF;
		int16_t go = (oldvalue >> 8) & 0xFF;
		int16_t bo = oldvalue & 0xFF;

		int16_t r = (value >> 16) & 0xFF;
		int16_t g = (value >> 8) & 0xFF;
		int16_t b = value & 0xFF;
		r = ((uint8_t) (ro + ((r - ro) * p))) & 0xFF;
		g = ((uint8_t) (go + ((g - go) * p))) & 0xFF;
		b = ((uint8_t) (bo + ((b - bo) * p))) & 0xFF;

		if (offset < (this->_width * this->_height))
			this->_data[offset] = 0xFF000000 | r << 16 | g << 8 | b;
	}
}

uint32_t Screen::GetPixel(uint16_t x, uint16_t y) {
	if (this->_data)
		return this->_data[(y * this->_width) + x];

	return 0;
}

uint16_t Screen::GetWidth() {
	return this->_width;
}

uint16_t Screen::GetHeight() {
	return this->_height;
}

void Screen::ScrollLeft(uint8_t columns) {
	for (uint8_t scroll = 0; scroll < columns; scroll++) {
		for (uint16_t i = 0; i < this->_height; i++) {
			uint32_t aux = this->_data[(i * this->_width)];
			for (uint16_t j = 0; j < this->_width - 1; j++)
				this->_data[(i * this->_width) + j] = this->_data[(i * this->_width) + (j + 1)];
			this->_data[(i * this->_width) + (this->_width - 1)] = aux;
		}
	}
}

void Screen::ScrollRight(uint8_t columns) {
	for (uint8_t scroll = 0; scroll < columns; scroll++) {
		for (uint16_t i = 0; i < this->_height; i++) {
			uint32_t aux = this->_data[(i * this->_width) + this->_width - 1];
			for (uint16_t j = this->_width - 1; j >= 1; j--)
				this->_data[(i * this->_width) + j] = this->_data[(i * this->_width) + (j - 1)];
			this->_data[(i * this->_width)] = aux;
		}
	}
}

void Screen::ScrollUp(uint8_t lines) {
	for (uint8_t scroll = 0; scroll < lines; scroll++) {
		for (uint16_t j = 0; j < this->_width; j++) {
			for (uint16_t i = 0; i < this->_height - 1; i++)
				this->_data[(i * this->_width) + j] = this->_data[((i + 1) * this->_width) + j];
			this->_data[((this->_height - 1) * this->_width) + j] = 0;
		}
	}
}

void Screen::ScrollDown(uint8_t lines) {
	for (uint8_t scroll = 0; scroll < lines; scroll++) {
		for (uint16_t j = 0; j < this->_width; j++) {
			for (uint16_t i = this->_height - 1; i >= 1; i--)
				this->_data[(i * this->_width) + j] = this->_data[((i - 1) * this->_width) + j];
			this->_data[j] = 0;
		}
	}
}
