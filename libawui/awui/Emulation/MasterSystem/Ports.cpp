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

Ports::Ports(VDP * vdp) {
	this->_vdp = vdp;
}

Ports::~Ports() {
}

void Ports::WriteByte(uint8_t port, uint8_t value) {
	printf("Write Port: %.2X    Value: %.2X   Not developed\n", port, value);
//	assert(false);
}

uint8_t Ports::ReadByte(uint8_t port) const {
	// Port BF and BD are the same
	if ((port == 0xBF) || (port == 0xBD))
		return this->_vdp->GetStatus();

	printf("Read Port: %.2X    Not developed\n", port);
	assert(false);
	return 0;
}