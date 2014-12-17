/*
 * awui/Emulation/Spectrum/Ports.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Ports.h"

using namespace awui::Emulation::Spectrum;

#include <awui/Emulation/Spectrum/CPUInst.h>

/*
 *|-----------------------------------|
 *| Port |   Input   |     Output     |
 *|------|-----------|----------------|
 *| 0x3E |           | Memory control |
 *| 0x3F |      I/O port control      |
 *| 0x7E | V counter |                |
 *| 0x7F | H counter |       PSG      |
 *| 0xBE |           |   VDP (data)   |
 *| 0xBF |           |  VDP (control) |
 *| 0xC0 |         Controllers        |
 *| 0xC1 |         Controllers        |
 *| 0xDC |         Controllers        |
 *| 0xDD |         Controllers        |
 *|-----------------------------------|
 */

// 0: USA-EUR (default)
// 1: USA-EUR
// 2: USA-EUR
// 3: Japonesa

#define DEFAULTREGION 0

Ports::Ports() {
	this->_region = DEFAULTREGION;
	this->_getRegion = false;
	this->_maskRegion = 0x00;
}

void Ports::WriteByte(CPUInst * cpu, uint8_t port, uint8_t value) {
}

uint8_t Ports::ReadByte(CPUInst * cpu, uint8_t port) const {
	return 0xFF;
}
