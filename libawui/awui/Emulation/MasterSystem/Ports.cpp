/*
 * awui/Emulation/MasterSystem/Ports.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Ports.h"

using namespace awui::Emulation::MasterSystem;

#include <assert.h>
#include <stdio.h>
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
 *| 0xDC |         Controllers        |
 *| 0xDD |         Controllers        |
 *|-----------------------------------|
 */

Ports::Ports() {
	// 0: Japonesa
	// 1: Japonesa
	// 2: Japonesa
	// 3: Japonesa
	this->_region = 2;
	this->_getRegion = false;
}

Ports::~Ports() {
}

void Ports::SetVDP(VDP * vdp) {
	this->_vdp = vdp;
}

void Ports::WriteByte(uint8_t port, uint8_t value) {
//	printf("Write Port: %.2X    Value: %.2X\n", port, value);

	if (port >= 0x40 && port <= 0xBF) {
		this->_vdp->WriteByte(port, value);
		return;
	}

	// SDSC
	if (port == 0xFD) {
		printf("%c", value);
		fflush(stdout);
		return;
	}

	if (port == 0x3F) {
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
		return this->_vdp->ReadByte(port);

	if (port == 0xDC || port == 0xC0)
		return 0xFF;

	if (port == 0xDD || port == 0xC1) {
		if (this->_getRegion) {
//			printf("Region: %d\n", this->_region);
			return 0x1F | ((this->_region & 0x3) << 6);
		}

		return 0xFF;
	}

//	assert(false);
	return 0;
}