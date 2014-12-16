/*
 * awui/Emulation/Spectrum/Ports.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Ports.h"

using namespace awui::Emulation::Spectrum;

#include <assert.h>
#include <stdio.h>
#include <awui/Emulation/Spectrum/CPUInst.h>
#include <awui/Emulation/Spectrum/Sound.h>
#include <awui/Emulation/Spectrum/VDP.h>

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
//	printf("Write Port: %.2X    Value: %.2X\n", port, value);
	if (port == 0x7f || port == 0x7e) {
		// printf("Write Port: %.2X    Value: %.2X\n", port, value);
		cpu->GetSound()->WriteByte(cpu, value);
		return;
	}

	if (port >= 0x40 && port <= 0xBF) {
		cpu->GetVDP()->WriteByte(port, value);
		return;
	}

	// SDSC
	if (port == 0xFD) {
		printf("%c", value);
		fflush(stdout);
		return;
	}

	if (port == 0x3F) {
		if (value & 0x01)
			this->_region = (this->_region & 0x02) | ((value >> 5) & 0x01);
		else
			this->_region = (this->_region & 0x02) | (DEFAULTREGION & 0x01);

		if (value & 0x04)
			this->_region = (this->_region & 0x01) | ((value >> 6) & 0x02);
		else
			this->_region = (this->_region & 0x01) | (DEFAULTREGION & 0x02);

		if (DEFAULTREGION == 3)
			this->_region = 3;

		this->_maskRegion = value;

		return;
	}

//	assert(false);
}

uint8_t Ports::ReadByte(CPUInst * cpu, uint8_t port) const {
//	printf("Read Port: %.2X\n", port);

	if (port >= 0x40 && port <= 0xBF)
		return cpu->GetVDP()->ReadByte(port);

	if (port == 0xC0 || port == 0xDC)
		return ((cpu->GetPad2() << 6) | ((cpu->GetPad1() & 0x3F)));

	if (port == 0xC1 || port == 0xDD) {
		uint8_t data =  0x30 | ((cpu->GetPad2() & 0x3F) >> 2);
		if (this->_maskRegion & 0x01)
			data |= (this->_region & 0x01) << 6;
		else
			data |= 0x40;

		if (this->_maskRegion & 0x04)
			data |= (this->_region & 0x02) << 6;
		else
			data |= 0x80;

		return data;
	}

//	assert(false);
	return 0xFF;
}
