/*
 * awui/Emulation/MasterSystem/CPU.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "CPU.h"

#include <awui/Emulation/MasterSystem/Rom.h>

using namespace awui::Emulation::MasterSystem;

CPU::CPU() {
	this->_rom = new Rom(4096);
	this->_pc = 0;
}

CPU::~CPU() {
	delete this->_rom;
}

void CPU::LoadRom(const String file) {
	this->_rom->LoadRom(file);
}

void CPU::OnTick() {
	this->RunOpcode();
}

void CPU::RunOpcode() {
}