// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Processor.h"

#include <awui/Emulation/Chip8/Graphics.h>
#include <awui/Emulation/Chip8/Registers.h>
#include <awui/String.h>

using namespace awui::Emulation::Chip8;

Processor::Processor() {
	this->_graphics = new Graphics(64, 32);
	this->_registers = new Registers(16);
}

Processor::~Processor() {
	delete this->_graphics;
	delete this->_registers;
}

void Processor::LoadRom(const String file) {
}
