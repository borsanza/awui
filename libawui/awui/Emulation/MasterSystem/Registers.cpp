/*
 * awui/Emulation/MasterSystem/Registers.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Registers.h"

using namespace awui::Emulation::MasterSystem;

Registers::Registers() {
	this->Clear();
}

Registers::~Registers() {
}

void Registers::Clear() {
	this->_sp = 0;
}

void Registers::SetSP(uint16_t value) {
	this->_sp = value;
}

uint16_t Registers::GetSP() {
	return this->_sp;
}