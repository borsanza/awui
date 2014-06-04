/*
 * awui/Emulation/Chip8/Processor.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Processor.h"

#include <awui/Emulation/Chip8/Screen.h>
#include <awui/Emulation/Chip8/Memory.h>
#include <awui/Emulation/Chip8/Registers.h>
#include <awui/String.h>

using namespace awui::Emulation::Chip8;

Processor::Processor() {
	this->_screen = new Screen(64, 32);
	this->_memory = new Memory(4096);
	this->_registers = new Registers(16);
}

Processor::~Processor() {
	delete this->_screen;
	delete this->_memory;
	delete this->_registers;
}

void Processor::LoadRom(const String file) {
	this->_memory->LoadRom(file);
}