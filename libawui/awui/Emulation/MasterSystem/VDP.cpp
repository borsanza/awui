/*
 * awui/Emulation/MasterSystem/VDP.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "VDP.h"

#include <awui/Emulation/MasterSystem/CPU.h>
#include <awui/Emulation/MasterSystem/Ram.h>
#include <awui/Emulation/MasterSystem/Registers.h>
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
	this->_data = NULL;
	this->_width = 256;
	this->_height = 192;
	this->ResetVideo();

	this->_cpu = cpu;
	this->_line = 0;
	this->_col = 0;
	this->_lastLine = 0;
	this->_lastCol = 0;

	this->_status = 0x1F;
	this->_address = 0;
	this->_goVram = true;
	this->_baseAddress = 0;
	this->_ntsc = true;

	this->_controlByte = -1;

	// 16Kb in Video Ram
	this->_vram = new Ram(0x4000);

	// Rellenando array de valores para el vertical
	for (int i = 0; i < 262; i++) {
		this->NTSCx192[i] = i <= 0xDA ? i : i - 0xDA + 0xD5 - 1; // 00-DA, D5-FF
		this->NTSCx224[i] = i <= 0xEA ? i : i - 0xEA + 0xE5 - 1; // 00-EA, E5-FF
		this->NTSCx240[i] = i <= 0xFF ? i : i - 0xFF + 0x00 - 1; // 00-FF, 00-06
	}

	for (int i = 0; i < 313; i++) {
		this->PALx192[i] = i <= 0xF2 ? i : i - 0xF2 + 0xBA - 1; // 00-F2, BA-FF
		this->PALx224[i] = i <= 0xFF ? i : ((i <= 0x100 + 0x02)? i - 0xFF - 1 : i - 0xFF - 0x02 + 0xCA - 2); // 00-FF, 00-02, CA-FF
		this->PALx240[i] = i <= 0xFF ? i : ((i <= 0x100 + 0x0A)? i - 0xFF - 1 : i - 0xFF - 0x0A + 0xD2 - 2); // 00-FF, 00-0A, D2-FF
	}

	for (int i = 0; i < 256; i++) this->HORSYNC[i]       =    0 + ((i / 255.0f) * 0x7F); // 256 : 00-7F : Active display
	for (int i = 0; i < 15; i++)  this->HORSYNC[i + 256] = 0x80 + ((i / 14.0f) * 0x7);   //  15 : 80-87 : Right border
	for (int i = 0; i < 8; i++)   this->HORSYNC[i + 271] = 0x87 + ((i / 7.0f) * 0x4);    //   8 : 87-8B : Right blanking
	for (int i = 0; i < 26; i++)  this->HORSYNC[i + 279] = 0x8B + ((i / 25.0f) * 0x62);  //  26 : 8B-ED : Horizontal sync
	for (int i = 0; i < 2; i++)   this->HORSYNC[i + 305] = 0xED + ((i / 1.0f) * 0x1);    //   2 : ED-EE : Left blanking
	for (int i = 0; i < 14; i++)  this->HORSYNC[i + 307] = 0xEE + ((i / 13.0f) * 0x7);   //  14 : EE-F5 : Color burst
	for (int i = 0; i < 8; i++)   this->HORSYNC[i + 321] = 0xF5 + ((i / 7.0f) * 0x4);    //   8 : F5-F9 : Left blanking
	for (int i = 0; i < 13; i++)  this->HORSYNC[i + 329] = 0xF9 + ((i / 12.0f) * 0x6);   //  13 : F9-FF : Left border

	this->Reset();
}

VDP::~VDP() {
	delete this->_vram;
	free(this->_data);
}

void VDP::ResetVideo() {
	if (this->_data)
		free(this->_data);

	//this->_data = (uint32_t *) malloc (this->_width * this->_height * sizeof(uint32_t));
	this->_data = (uint32_t *) calloc (this->_width * this->_height, sizeof(uint32_t));
}

void VDP::SetNTSC() {
	this->_ntsc = true;
}

void VDP::SetPAL() {
	this->_ntsc = false;
}

bool VDP::GetNTSC() {
	return this->_ntsc;
}

bool VDP::GetPAL() {
	return !this->_ntsc;
}

void VDP::Reset() {
	uint8_t values[] = {0x36, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0x00, 0x00, 0x00, 0xFF};
	for (uint8_t i = 0; i<=10; i++)
		this->_registers[i] = values[i];

	for (uint8_t i = 0; i<32; i++)
		this->_cram[i] = 0;

	this->UpdateAllRegisters();
}

uint16_t VDP::GetWidth() {
	return this->_width;
}

uint16_t VDP::GetHeight() {
	return this->_height;
}

void VDP::SetHeight(uint16_t height) {
	if (height != this->_height) {
		this->_height = height;
		this->ResetVideo();
	}
}

uint16_t VDP::GetTotalWidth() {
	return 342;
}

uint16_t VDP::GetTotalHeight() {
	return (this->_ntsc ? 262 : 313);
}

uint32_t VDP::GetPixel(uint16_t x, uint16_t y) {
	return this->_data[(y * this->_width) + x];
}

bool VDP::OnTick(uint32_t counter) {
/*
	{
//		int8_t r = (counter % 2) == 0?0xFF : 0x00;
//		int8_t g = (counter % 2) == 0?0xFF : 0x00;
//		int8_t b = (counter % 2) == 0?0xFF : 0x00;
		counter = (this->_col * 8) / this->_width;
		counter += 8;
		// min = 8
		// max = 15
		uint8_t b = (this->_cram[counter % 32] >> 4) & 0x3;
		uint8_t g = (this->_cram[counter % 32] >> 2) & 0x3;
		uint8_t r = this->_cram[counter % 32] & 0x3;
		r = r * 85;
		g = g * 85;
		b = b * 85;
		this->_data[this->_col + (this->_line * this->_width)] = 0xFF000000 | r << 16 | g << 8 | b;
	}
*/
	bool ret = false;
	this->_col++;
	if (this->_col >= this->GetTotalWidth()) {
		this->_col = 0;
		this->_line++;
		if (this->_line >= this->GetTotalHeight()) {
			this->_line = 0;
			this->_status |= 0x80;
			ret = true;
		}
	}

	if (this->_col % 32 == 0) {
		while ((this->_col != this->_lastCol) && (this->_line != this->_lastLine)) {

//			if (this->_lastCol % 32 == 0)
			{
/*
				int8_t r = (counter % 2) == 0?0xFF : 0x00;
				int8_t g = (counter % 2) == 0?0xFF : 0x00;
				int8_t b = (counter % 2) == 0?0xFF : 0x00;
*/
				counter = (this->_lastCol * 8) / this->_width;
				counter += 8;

				uint8_t b = (this->_cram[counter % 32] >> 4) & 0x3;
				uint8_t g = (this->_cram[counter % 32] >> 2) & 0x3;
				uint8_t r = this->_cram[counter % 32] & 0x3;
				r = r * 85;
				g = g * 85;
				b = b * 85;

				this->_data[this->_lastCol + (this->_lastLine * this->_width)] = 0xFF000000 | r << 16 | g << 8 | b;
			}

			this->_lastCol++;
			if (this->_lastCol >= this->GetTotalWidth()) {
				this->_lastCol = 0;
				this->_lastLine++;
				if (this->_lastLine >= this->GetTotalHeight())
					this->_lastLine = 0;
			}
		}
	}

//	if (this->_line == 110)
//		printf("                       %5d: %3dx%3d\n", counter, this->_col, this->_line);

	return ret;
}

/*
 * NTSC, 256x192   262 (3 vsync)
 * NTSC, 256x224   262 (3 vsync)
 * PAL, 256x192    313 (3 vsync)
 * PAL, 256x224    313 (3 vsync)
 * PAL, 256x240    313 (3 vsync)
 * Horizontal:     342 (26 hsync)
 */

uint8_t VDP::GetStatus(bool resetStatus) {
	uint8_t r = this->_status;

	// Clear bits 6 and 7
	if (resetStatus)
		this->_status &= 0x3F;

	return r;
}

void VDP::UpdateAllRegisters() {
	// Register #0

	// Register #1
	this->_visible = this->_registers[0] & 0x40;

	if (this->_registers[0] & 0x04) {
		uint16_t height = 192;
		if (this->_registers[0] & 0x02) {
			if (this->_registers[1] & 0x10)
				height = 224;
			else
				if (this->_registers[1] & 0x08)
					height = 240;
		}

		this->SetHeight(height);
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
//		printf("CRam[%.2X] = %.2X\n", this->_address, value);
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
	bool r = false;

	if (port >= 0x40 && port <= 0x7F) {
		r = true;
		if (even) {
			uint8_t line;
			if (this->_ntsc) {
				switch (this->_height) {
					case 192:
						line = this->NTSCx192[this->_line];
						break;
					case 224:
						line = this->NTSCx224[this->_line];
						break;
					case 240:
						line = this->NTSCx240[this->_line];
						break;
				}
			} else {
				switch (this->_height) {
					case 192:
						line = this->PALx192[this->_line];
						break;
					case 224:
						line = this->PALx224[this->_line];
						break;
					case 240:
						line = this->PALx240[this->_line];
						break;
				}
			}

			// printf("Line: %d:%d\n", line, this->_cpu->GetRegisters()->GetD());
			return line;
		} else {
			uint8_t col = this->HORSYNC[this->_col];
			// printf("Col: %d:%d\n", col, this->_cpu->GetRegisters()->GetD());
			return col;
		}
	}

	if (port >= 0x80 && port <= 0xBF) {
		r = true;
		if (even) {
		} else {
			return this->GetStatus();
		}
	}

	if (!r) {
		printf("VDP::ReadByte(port = %.2X);\n", port);
		assert(0);
	}

	return 0;
}

uint8_t * VDP::GetColors() {
	return this->_cram;
}