/*
 * awui/Emulation/MasterSystem/VDP.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "VDP.h"

#include <awui/Emulation/MasterSystem/CPU.h>
#include <awui/Emulation/MasterSystem/Ram.h>
#include <assert.h>
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

VDP::VDP(CPU * cpu) {
	this->_cpu = cpu;
	this->_width = 256;
	this->_height = 192;
	this->_line = 0;
	this->_col = 0;
	this->_status = 0x1F;
	this->_address = 0;
	this->_goVram = true;
	this->_baseAddress = 0;

	this->_controlByte = -1;

	// 16Kb in Video Ram
	this->_vram = new Ram(0x4000);

	this->Reset();
}

VDP::~VDP() {
	delete this->_vram;
}

void VDP::Reset() {
	uint8_t values[] = {0x36, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0x00, 0x00, 0x00, 0xFF};
	for (uint8_t i = 0; i<=10; i++)
		this->_registers[i] = values[i];

	this->UpdateAllRegisters();
}

uint16_t VDP::GetWidth() {
	return this->_width;
}

uint16_t VDP::GetHeight() {
	return this->_height;
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

void VDP::UpdateAllRegisters() {
	// Register #0

	// Register #1
	this->_visible = this->_registers[0] & 0x40;

	if (this->_registers[0] & 0x04) {
		this->_height = 192;
		if (this->_registers[0] & 0x02) {
			if (this->_registers[1] & 0x10)
				this->_height = 224;
			else
				if (this->_registers[1] & 0x08)
					this->_height = 240;
		}
	}

	if (this->_registers[1] & 0x01) {
		if (this->_registers[0] & 0x04)
			this->_spriteSize = SPRITE_8x16;
		else
			this->_spriteSize = SPRITE_16x16;
	} else
		this->_spriteSize = SPRITE_8x8;

	// Register #2
	uint8_t code = (this->_registers[2] >> 1) & 0x7;
	if ((this->_height == 224) || (this->_height == 240)) {
		code = code >> 1;
		this->_baseAddress = (code * 0x1000) + 0x0700;
	} else {
		this->_baseAddress = code * 0x0800;
	}
}


void VDP::WriteControlByte(uint8_t value) {
	if (this->_controlByte == -1) {
		this->_controlByte = value;
//		this->_cpu->SetAddressBus((this->_cpu->GetAddressBus() & 0xFF00) | value);
		this->_address = (this->_address & 0x3F00) | value;
//		printf("VDP Address: %.4X\n", this->_cpu->GetAddressBus() & 0x03FF);

		return;
	}

	uint8_t state = value >> 6;

	this->_address = ((value & 0x3F) << 8) | this->_controlByte;
//	this->_cpu->SetAddressBus((this->_cpu->GetAddressBus() & 0xC000) | this->_address);

//	 printf("VDP Address: %.4X\n", _address);

	switch (state) {
		case 0:
			this->_goVram = true;
			break;
		case 1:
			this->_goVram = true;
			break;
		case 2:
			{
				uint8_t pos = value & 0xF;
				if (pos < 11) {
					this->_registers[pos] = this->_controlByte;
					// printf("VReg[%X] = %.2X\n", value & 0xF, this->_controlByte);
					this->UpdateAllRegisters();
				}
			}
			break;
		case 3:
			this->_goVram = false;
			break;
	}

	this->_controlByte = -1;
}

void VDP::WriteDataByte(uint8_t value) {
	if (this->_goVram) {
		this->_vram->WriteByte(this->_address, value);
		// printf("VRam");
	} else {
		this->_cram[this->_address] = value;
		// printf("CRam");
	}

	// printf("[%.4X] = %.2X\n", this->_address, value);

	this->_address = (this->_address + 1) & 0x3FFF;
//	this->_cpu->SetAddressBus(this->_cpu->GetAddressBus() + 1);
}

void VDP::WriteByte(uint8_t port, uint8_t value) {
	bool even = ((port & 1) == 0);

	if (port >= 0x40 && port <= 0x7F) {
		if (even) {
		} else {
		}
	}

	if (port >= 0x80 && port <= 0xBF) {
		if (even) {
			this->WriteDataByte(value);
		} else {
			this->WriteControlByte(value);
		}
	}
}

uint8_t VDP::ReadByte(uint8_t port) {
	bool even = ((port & 1) == 0);

	if (port >= 0x40 && port <= 0x7F) {
		if (even) {
		} else {
		}
	}

	if (port >= 0x80 && port <= 0xBF) {
		if (even) {
		} else {
			return this->GetStatus();
		}
	}

	assert(0);
	return 0;
}