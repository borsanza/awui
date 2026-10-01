/**
 * awui/Emulation/Chip8/Screen.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Screen.h"

#include <cstring>

#include <awui/String.h>

#include <stdlib.h>

using namespace awui::Emulation::Chip8;

Screen::Screen(uint16_t width, uint16_t height) {
	m_width = width;
	m_height = height;
	m_data = (uint32_t *) malloc(sizeof(uint32_t) * width * height);
	Clear();
}

Screen::~Screen() {
	free(m_data);
}

void Screen::Clear() {
	uint16_t length = m_width * m_height;
	for (uint16_t i = 0; i < length; i++)
		m_data[i] = 0;
}

void Screen::CopyFrom(const Screen &other) {
	if ((other.m_width == m_width) && (other.m_height == m_height))
		memcpy(m_data, other.m_data, sizeof(uint32_t) * m_width * m_height);
}

// La pantalla es circular: lo que se sale por un borde aparece por el contrario, en horizontal y en vertical (el COSMAC
// VIP y SuperChip recortaban; aquí se da la vuelta, como XO-CHIP y los juegos hechos para emuladores, como Minimal
// game). Devuelve si se ha borrado un píxel encendido (colisión)
bool Screen::SetPixelXOR(uint16_t x, uint16_t y, bool value) {
	if (!value)
		return false;

	x %= m_width;
	y %= m_height;

	uint32_t offset = (y * m_width) + x;
	bool oldValue = m_data[offset];
	m_data[offset] = !oldValue;
	return oldValue;
}

void Screen::SetPixel(uint16_t x, uint16_t y, uint32_t value) {
	if ((x >= m_width) || (y >= m_height))
		return;

	uint32_t offset = (y * m_width) + x;

	uint8_t a = (value >> 24) & 0xFF;
	if (a == 255) {
		m_data[offset] = value;
	} else {
		uint32_t oldvalue = m_data[offset];
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

		m_data[offset] = 0xFF000000 | r << 16 | g << 8 | b;
	}
}

uint32_t Screen::GetPixel(uint16_t x, uint16_t y) {
	if ((x >= m_width) || (y >= m_height))
		return 0;

	return m_data[(y * m_width) + x];
}

uint16_t Screen::GetWidth() const {
	return m_width;
}

uint16_t Screen::GetHeight() const {
	return m_height;
}

void Screen::ScrollLeft(uint8_t columns) {
	for (uint8_t scroll = 0; scroll < columns; scroll++) {
		for (uint16_t i = 0; i < m_height; i++) {
			uint32_t aux = m_data[(i * m_width)];
			for (uint16_t j = 0; j < m_width - 1; j++)
				m_data[(i * m_width) + j] = m_data[(i * m_width) + (j + 1)];
			m_data[(i * m_width) + (m_width - 1)] = aux;
		}
	}
}

void Screen::ScrollRight(uint8_t columns) {
	for (uint8_t scroll = 0; scroll < columns; scroll++) {
		for (uint16_t i = 0; i < m_height; i++) {
			uint32_t aux = m_data[(i * m_width) + m_width - 1];
			for (uint16_t j = m_width - 1; j >= 1; j--)
				m_data[(i * m_width) + j] = m_data[(i * m_width) + (j - 1)];
			m_data[(i * m_width)] = aux;
		}
	}
}

void Screen::ScrollUp(uint8_t lines) {
	for (uint8_t scroll = 0; scroll < lines; scroll++) {
		for (uint16_t j = 0; j < m_width; j++) {
			for (uint16_t i = 0; i < m_height - 1; i++)
				m_data[(i * m_width) + j] = m_data[((i + 1) * m_width) + j];
			m_data[((m_height - 1) * m_width) + j] = 0;
		}
	}
}

void Screen::ScrollDown(uint8_t lines) {
	for (uint8_t scroll = 0; scroll < lines; scroll++) {
		for (uint16_t j = 0; j < m_width; j++) {
			for (uint16_t i = m_height - 1; i >= 1; i--)
				m_data[(i * m_width) + j] = m_data[((i - 1) * m_width) + j];
			m_data[j] = 0;
		}
	}
}
