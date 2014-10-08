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
	}

//	assert(false);
}

uint8_t Ports::ReadByte(uint8_t port) const {
//	printf("Read Port: %.2X\n", port);

	if (port >= 0x40 && port <= 0xBF)
		return this->_vdp->ReadByte(port);

//	assert(false);
	return 0;
}