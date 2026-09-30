/*
 * awui/Emulation/Chip8/Registers.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Registers.h"

#include <stdlib.h>

using namespace awui::Emulation::Chip8;

Registers::Registers(uint8_t n) {
	m_i = 0;
	m_length = n;
	m_v = (uint8_t *) malloc(sizeof(uint8_t *) * n);
	Clear();
}

Registers::~Registers() {
	free(m_v);
}

void Registers::Clear() {
	for (uint8_t i = 0; i < m_length; i++)
		m_v[i] = 0;

	m_i = 0;
}

void Registers::SetV(uint8_t pos, uint8_t value) {
	m_v[pos] = value;
}

uint8_t Registers::GetV(uint8_t pos) {
	return m_v[pos];
}

void Registers::SetI(uint32_t value) {
	m_i = value;
}

uint32_t Registers::GetI() {
	return m_i;
}
