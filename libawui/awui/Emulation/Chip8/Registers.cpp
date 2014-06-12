/*
 * awui/Emulation/Chip8/Registers.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Registers.h"

#include <stdlib.h>

using namespace awui::Emulation::Chip8;

Registers::Registers(unsigned char n) {
	this->_length = n;
	this->_v = (unsigned char *) malloc (sizeof(unsigned char *) * n);
	this->Clear();
}

Registers::~Registers() {
	free(this->_v);
}

void Registers::Clear() {
	for (unsigned char i = 0; i < this->_length; i++)
		this->_v[i] = 0;

	this->_i = 0;
}

void Registers::SetV(unsigned char pos, unsigned char value) {
	this->_v[pos] = value;
}

unsigned char Registers::GetV(unsigned char pos) {
	return this->_v[pos];
}

void Registers::SetI(unsigned int value) {
	this->_i = value;
}

unsigned int Registers::GetI() {
	return this->_i;
}