/*
 * awui/Emulation/Chip8/Registers.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Registers.h"

#include <stdlib.h>

using namespace awui::Emulation::Chip8;

Registers::Registers(uint8_t n) {
	this->_i = 0;
	this->_length = n;
	this->_v = (uint8_t *) malloc (sizeof(uint8_t *) * n);
	this->Clear();
}

Registers::~Registers() {
	free(this->_v);
}

void Registers::Clear() {
	for (uint8_t i = 0; i < this->_length; i++)
		this->_v[i] = 0;

	this->_i = 0;
}

void Registers::SetV(uint8_t pos, uint8_t value) {
	this->_v[pos] = value;
}

uint8_t Registers::GetV(uint8_t pos) {
	return this->_v[pos];
}

void Registers::SetI(uint32_t value) {
	this->_i = value;
}

uint32_t Registers::GetI() {
	return this->_i;
}
