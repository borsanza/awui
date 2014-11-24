/*
 * awui/Emulation/MasterSystem/Ports.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Ports.h"

using namespace awui::Emulation::MasterSystem;

#include <assert.h>
#include <stdio.h>
#include <awui/Emulation/MasterSystem/CPU.h>
#include <awui/Emulation/MasterSystem/VDP.h>

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

Ports::Ports() {
	this->_cpu = NULL;

	// 0: Japonesa
	// 1: Japonesa
	// 2: Japonesa
	// 3: Japonesa
	this->_region = 2;
	this->_getRegion = false;
}

Ports::~Ports() {
}

void Ports::SetCPU(CPU * cpu) {
	this->_cpu = cpu;
}

void Ports::WriteByte(uint8_t port, uint8_t value) {
//	printf("Write Port: %.2X    Value: %.2X\n", port, value);

	if (port >= 0x40 && port <= 0xBF) {
		this->_cpu->GetVDP()->WriteByte(port, value);
		return;
	}

	// SDSC
	if (port == 0xFD) {
		printf("%c", value);
		fflush(stdout);
		return;
	}

	if (port == 0x3F) {
//		printf("Region value: %.4X\n", value);
		if (value == 0xF5) {
//			printf("Region: True\n");
			this->_getRegion = true;
		}

		if (value == 0xF0) {
//			printf("Region: False\n");
			this->_getRegion = false;
		}

		return;
	}

//	assert(false);
}

uint8_t Ports::ReadByte(uint8_t port) const {
//	printf("Read Port: %.2X\n", port);

	if (port >= 0x40 && port <= 0xBF)
		return this->_cpu->GetVDP()->ReadByte(port);

	if (port == 0xC0 || port == 0xDC)
		return ((this->_cpu->GetPad2() << 6) | ((this->_cpu->GetPad1() & 0x3F)));

	if (port == 0xC1 || port == 0xDD) {
		if (this->_getRegion) {
//			printf("Region: %d\n", this->_region);
			return ((this->_region & 0x3) << 6) | 0x30 | ((this->_cpu->GetPad2() & 0x3F) >> 2);
		}

		return 0xF0 | ((this->_cpu->GetPad2() & 0x3F) >> 2);
	}

//	assert(false);
	return 0;
}
