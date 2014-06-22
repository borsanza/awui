/*
 * awui/Emulation/MasterSystem/VDP.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "VDP.h"

#include <stdio.h>

using namespace awui::Emulation::MasterSystem;

/*
 * bit 7: VSync flag, set at the beginning of each VSync impulse
 *     6: Line interruot flag: set when a line interrupt is generated
 *        (see the VDP section for details on line interrupts)
 *     5: Sprite collision flag: set when two sprites overlap
 *     4: ?
 *     3: ?
 *     2: ?
 *     1: ?
 *     0: ?
 */

VDP::VDP() {
	this->_width = 256;
	this->_height = 192;
	this->_line = 0;
	this->_col = 0;
	this->_status = 0x1F;
}

VDP::~VDP() {
}

bool VDP::OnTick() {
	bool r = false;
	this->_col++;
	if (this->_col >= this->_width) {
		this->_col = 0;
		this->_line++;
		if (this->_line >= this->_height) {
			this->_line = 0;
			this->_status |= 0x80;
			r = true;
		}
	}

	// printf("                       %dx%d\n", this->_col, this->_line);
	return r;
}

uint8_t VDP::GetStatus() {
	uint8_t r = this->_status;

	// Clear bits 6 and 7
	this->_status &= 0x3F;

	return r;
}