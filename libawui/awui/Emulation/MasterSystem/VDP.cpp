
/*
 * awui/Emulation/MasterSystem/VDP.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

// Segun documentacion
#define LEFTBORDER 13
#define RIGHTBORDER 15

// Comparado con la Master System
//#define LEFTBORDER 1
//#define RIGHTBORDER 12

#include "VDP.h"

#include <awui/Emulation/MasterSystem/CPU.h>
#include <awui/Emulation/MasterSystem/Ram.h>
#include <awui/Emulation/MasterSystem/Registers.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

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
	this->_ntsc = false;
	this->_showBorder = false;
	this->ResetVideo();

	this->_cpu = cpu;
	this->_line = 0;
	this->_col = 0;
	this->_interrupt = false;

	this->_status = 0x1F;
	this->_address = 0;
	this->_portState = 0xFF;
	this->_baseAddress = 0;

	this->_controlMode = false;

	// 16Kb in Video Ram
	this->_vram = new Ram(0x4000);

	// Rellenando array de valores para el vertical
	for (int i = 0; i < 262; i++) {
		this->NTSCx192[i] = i <= 0xDA ? i : i - 0xDA + 0xD5 - 1; // 00-DA, D5-FF
		this->NTSCx224[i] = i <= 0xEA ? i : i - 0xEA + 0xE5 - 1; // 00-EA, E5-FF
		this->NTSCx240[i] = i <= 0xFF ? i : i - 0xFF + 0x00 - 1; // 00-FF, 00-06
	}

	for (int i = 0; i < 313; i++) {
		this->PALx192[i] = (i <= 0xF2) ? i : i - 0x39; // 00-F2, BA-FF
		this->PALx224[i] = (i <= 0xFF) ? i : ((i <= 0x102)? i - 0x100 : i - 0x39); // 00-FF, 00-02, CA-FF
		this->PALx240[i] = (i <= 0xFF) ? i : ((i <= 0x10A)? i - 0x100 : i - 0x39); // 00-FF, 00-0A, D2-FF
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
	delete this->_data;
}

void VDP::Reset() {
	this->_data->Clear();
	this->_vram->Clear();

	uint8_t values[] = {0x36, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0x00, 0x00, 0x00, 0xFF};
	for (uint8_t i = 0; i<=10; i++)
		this->_registers[i] = values[i];

	for (uint8_t i = 0; i<32; i++)
		this->_cram[i] = 0;

	this->UpdateAllRegisters();
}

void VDP::Clear() {
	this->_data->Clear();
}

const uint8_t * VDP::GetColors() const {
	return this->_cram;
}

Ram * VDP::GetVram() const {
	return this->_vram;
}

bool VDP::GetShowBorder() const {
	return this->_showBorder;
}

uint16_t VDP::GetLine() const {
	return this->_line;
}

uint16_t VDP::GetColumn() const {
	return this->_col;
}

void VDP::SetShowBorder(bool mode) {
	if (this->_showBorder != mode) {
		this->_showBorder = mode;
		this->ResetVideo();
	}
}

void VDP::ResetVideo() {
	if (!this->_data)
		this->_data = new Ram(this->GetVisualWidth() * this->GetVisualHeight());
	else
		this->_data->Resize(this->GetVisualWidth() * this->GetVisualHeight());
}

void VDP::SetNTSC() {
	if (!this->_ntsc) {
		this->_ntsc = true;
		this->ResetVideo();
	}
}

void VDP::SetPAL() {
	if (this->_ntsc) {
		this->_ntsc = false;
		this->ResetVideo();
	}
}

bool VDP::GetNTSC() const {
	return this->_ntsc;
}

bool VDP::GetPAL() const {
	return !this->_ntsc;
}

uint16_t VDP::GetWidth() const {
	return this->_width;
}

uint16_t VDP::GetHeight() const {
	return this->_height;
}

void VDP::SetHeight(uint16_t height) {
	if (height != this->_height) {
		this->_height = height;
		this->ResetVideo();
	}
}

uint16_t VDP::GetTotalWidth() const {
	return 342;
}

uint16_t VDP::GetVisualWidth() const {
	if (this->_showBorder)
		return 256 + LEFTBORDER + RIGHTBORDER;

	return this->GetWidth();
}

uint16_t VDP::GetVisualHeight() const {
	if (this->_showBorder) {
		if (this->GetNTSC())
			return 243;
		else
			return 294;
	}

	return this->GetHeight();
}

uint16_t VDP::GetActiveLeft() const {
	if (this->_showBorder)
		return LEFTBORDER;

	return 0;
}

uint16_t VDP::GetBorderBottom() const {
	if (this->_showBorder)
		return this->GetVisualHeight() - (this->GetHeight() + this->GetActiveTop());

	return 0;
}

uint16_t VDP::GetActiveTop() const {
	if (this->_showBorder) {
		if (this->GetNTSC()) {
			switch (this->_height) {
				case 192: return 27;
				case 224: return 11;
				case 240: return 2;
			}
		} else {
			switch (this->_height) {
				case 192: return 54;
				case 224: return 38;
				case 240: return 30;
			}
		}
	}

	return 0;
}

uint16_t VDP::GetTotalHeight() const {
	return (this->_ntsc ? 262 : 313);
}

uint8_t VDP::GetPixel(uint16_t x, uint16_t y) const {
	return this->_data->ReadByte((y * this->GetVisualWidth()) + x);
}

bool VDP::IsVSYNC(uint16_t line) const {
	if (this->GetNTSC()) {
		switch (this->_height) {
			// 192    Active display
			// 24     Bottom border
			// 3      Bottom blanking
			// 3      Vertical blanking
			case 192: return 219 == line;

			// 224    Active display
			// 8      Bottom border
			// 3      Bottom blanking
			// 3      Vertical blanking
			case 224: return 235 == line;

			// 240    Active display
			case 240:
				// Segun documentacion no funciona en maquinas reales
				//assert(0);
				return 240 == line;
		}
	} else {
		switch (this->_height) {
			// 192    Active display
			// 48     Bottom border
			// 3      Bottom blanking
			// 3      Vertical blanking
			case 192: return 249 == line; // Confirmado con la Master System

			// 224    Active display
			// 32     Bottom border
			// 3      Bottom blanking
			// 3      Vertical blanking
			case 224: return 281 == line;

			// 240    Active display
			// 24     Bottom border
			// 3      Bottom blanking
			// 3      Vertical blanking
			case 240: return 297 == line;
		}
	}

	assert(0);
	return false;
}

void VDP::CalcNextPixel(uint16_t * col, uint16_t * line, bool * hsync, bool * vsync) {
	*hsync = false;
	*vsync = false;

	(*col)++;
	if (*col == this->GetTotalWidth()) {
		*col = 0;
		(*line)++;
		if (*line == this->GetTotalHeight())
			*line = 0;
	}

	if (*col == 271) {
		if (*line == this->GetTotalHeight() - 2)
			this->_interrupt = true;

		if ((this->_registers[0] & 0x10) && ((this->_registers[10] == 0) || (((*line - 3) % (this->_registers[10] + 1)) == 0)))
			this->_interrupt = true;
	}

	// 256 Active Display +
	// 15 Right Border +
	// 8 Right Blank <- Suponemos que cambia al final del Right Blank
	// 26 HSync
	if (*col == 279) {
		*hsync = true;
		*vsync = this->IsVSYNC(*line);
	}
}

void VDP::OnTickBorder() {
	int x = 0;
	int y = 0;
	bool draw = false;
	if ((this->_col >= this->GetWidth()) || (this->_line >= this->GetHeight())) {
		if (this->_col >= (this->GetTotalWidth() - LEFTBORDER)) {
			draw = true;
			x = this->_col - (this->GetTotalWidth() - LEFTBORDER);
		} else {
			if (this->_col < this->GetWidth() + RIGHTBORDER) {
				draw = true;
				x = LEFTBORDER + this->_col;
			}
		}

		if (this->_line >= (this->GetTotalHeight() - this->GetActiveTop())) {
			draw = true;
			y = this->_line - (this->GetTotalHeight() - this->GetActiveTop());
		} else {
			if (this->_line < (this->GetHeight() + this->GetBorderBottom())) {
				draw = true;
				y = this->GetActiveTop() + this->_line;
			}
		}
	}

	if (draw)
		this->_data->WriteByte(x + (y * this->GetVisualWidth()), this->_cram[this->_registers[7] & 0x0F]);
}

// Background
uint8_t VDP::GetSpriteColor(uint16_t sprite, int x, int y, bool flipx, bool flipy, bool otherPalete, bool doble) const {
	if (doble) {
		x = x >> 1;
		y = y >> 1;
	}


	int64_t offset;
	if (flipy)
		offset = sprite * 32 + ((7 - y) * 4);
	else
		offset = sprite * 32 + (y * 4);

	uint8_t byte1 = this->_vram->ReadByte(offset);
	uint8_t byte2 = this->_vram->ReadByte(offset + 1);
	uint8_t byte3 = this->_vram->ReadByte(offset + 2);
	uint8_t byte4 = this->_vram->ReadByte(offset + 3);
	uint8_t mask, realX;

	if (flipx) {
		mask = 0x80 >> (7 - x);
		realX = x;
	} else {
		mask = 0x80 >> x;
		realX = 7 - x;
	}

	uint8_t c;

	c  = (  byte1 & mask) >> realX;
	c |= (((byte2 & mask) >> realX) << 1);
	c |= (((byte3 & mask) >> realX) << 2);
	c |= (((byte4 & mask) >> realX) << 3);
	if (otherPalete)
		c |= 16;

	// assert(c < 32);
	return c;
}

// Sprite
bool VDP::GetSpritePixel(uint8_t * color) const {
	uint8_t x = this->_col;
	uint8_t y = this->_line;


	int offset = 0;
	if (this->_registers[0] & 0x8) {
		offset = 8;
//		x += 8;
	}

	int16_t sx;
	int16_t sy;
	uint16_t pattern;
	uint8_t height = (this->_registers[1] & 0x2) ? 16 : 8;
	uint8_t width = 8;

	bool doble = (this->_registers[1] & 0x1);
	if (doble) {
		height = height << 1;
		width = width << 1;
	}

	uint16_t base = ((uint16_t) this->_registers[5] & 0x7E) << 7;

	int cont = 0;
	for (int n = 0; n < 64; n++) {
		sy = this->_vram->ReadByte(base + n);

		if (sy == 0xD0)
			return false;

		if (sy > (255 - height))
			sy -= 256;

		if ((y <= sy) || (y > (sy + height)))
			continue;

		cont++;
		if (cont > 8)
			break;

		sx = this->_vram->ReadByte(base + 128 + (n * 2));
		sx -= offset;

		if ((x < sx) || (x >= (sx + width)))
			continue;

		pattern = this->_vram->ReadByte(base + 129 + (n * 2));
		if (this->_registers[6] & 0x4)
			pattern |= 0x100;

		* color = this->GetSpriteColor(pattern, x - sx, y - sy - 1, false, false, true, doble);
		if ((*color & 0xF) == 0)
			continue;

		return true;
	}

	return false;
}

uint8_t VDP::GetBackgroundPixel(uint16_t sprite, int16_t x, int16_t y, bool flipx, bool flipy, bool otherPalete, bool bgPriority) const {
	uint8_t color;

	if (!bgPriority) {
		if (this->GetSpritePixel(&color))
			return this->_cram[color];

		color = this->GetSpriteColor(sprite, x, y, flipx, flipy, otherPalete, false);

		return this->_cram[color];
	} else {
		color = this->GetSpriteColor(sprite, x, y, flipx, flipy, otherPalete, false);
		if ((color & 0xF) == 0) {
			if (this->GetSpritePixel(&color))
				return this->_cram[color];
		}

		return this->_cram[color];
	}
}

bool VDP::OnTick(uint32_t counter) {
	bool ret = false;
	bool hsync, vsync;

	this->CalcNextPixel(&this->_col, &this->_line, &hsync, &vsync);
	if (vsync) {
		this->_status |= 0x80;
		ret = true;
	}

//	if (hsync)
//		this->_status |= 0x40;

	if (this->_line == 0 && this->_col == 0)
		this->_verticalScroll = this->_registers[9];

	if ((this->_col < this->_width) && (this->_line < this->_height)) {
		int16_t col = this->_col;
		int16_t line = this->_line;
		bool vScroll = true;
		bool hScroll = true;
		bool black = false;

		if (this->_registers[1] & 0x10)
			line -= 32;

		if (((line >> 3) <= 1) && (this->_registers[0] & 0x40))
			hScroll = false;

		if ((((col - (this->_registers[8] & 0x7))>> 3) >= 24) && (this->_registers[0] & 0x80))
			vScroll = false;
		else
			if ((((col >> 3) >= 24) && (this->_registers[0] & 0x80)) && !hScroll)
				vScroll = false;

		if (col < (this->_registers[8] & 0x7) && hScroll && ((this->_registers[8] & 0x7) != 0))
			black = true;

		if (hScroll) {
			col = col - this->_registers[8];
			while (col < 0) col += 256;
		}

		if (vScroll) {
			line = line + this->_verticalScroll;
			while (line >= 0xe0) line -= 0xe0;
		}

		int32_t pos;
		if (this->_showBorder)
			pos = this->_col + this->GetActiveLeft() + ((this->_line + this->GetActiveTop()) * this->GetVisualWidth());
		else
			pos = this->_col + (this->_line * this->GetVisualWidth());

		// Blank Display
		if (black || !(this->_registers[1] & 0x40) || ((this->_registers[0] & 0x20) && (this->_col < 8))) {
			this->_data->WriteByte(pos, this->_cram[this->_registers[7] & 0x0F]);
		} else {
			uint16_t base = (this->_registers[2] & 0x0E) << 10;
			int32_t offset = base + ((line >> 3) * 64) + ((col >> 3) * 2);
			uint8_t byte1 = this->_vram->ReadByte(offset);
			uint8_t byte2 = this->_vram->ReadByte(offset + 1);
			uint16_t sprite = ((byte2 & 0x1) << 8) | byte1;
			bool flipx = byte2 & 2;
			bool flipy = byte2 & 4;
			bool otherPalette = byte2 & 8;
			bool priority = byte2 & 16;
			this->_data->WriteByte(pos, this->GetBackgroundPixel(sprite, col & 0x7, line & 0x7, flipx, flipy, otherPalette, priority));
		}
	} else {
		if (this->_showBorder)
			this->OnTickBorder();
	}

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

	// Clear bits 6, 7, 8
	if (resetStatus)
		this->_status &= 0x1F;

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
	if (!this->_controlMode) {
		this->_controlMode = true;
		this->_address = (this->_address & 0x3F00) | value;
		return;
	}

	this->_address = (value & 0x3F) << 8 | (this->_address & 0xFF);

	this->_portState = value >> 6;

	switch (this->_portState) {
		// A byte of VRAM is read from the location defined by the
		// address register and is stored in the read buffer. The
		// address register is incremented by one. Writes to the
		// data port go to VRAM.
		case 0:
			this->_readbuffer = this->_vram->ReadByte(this->_address);
			this->_address = (this->_address + 1) & 0x3FFF;
			break;

		// Writes to the data port go to VRAM.
		case 1:
			break;

		// This value signifies a VDP register write, explained
		// below. Writes to the data port go to VRAM.
		case 2:
			{
				uint8_t pos = value & 0xF;
				if (pos < 11) {
					this->_registers[pos] = this->_address & 0xFF;
					this->UpdateAllRegisters();
				}
			}
			break;

		// Writes to the data port go to CRAM.
		case 3:
			break;
	}

	this->_controlMode = false;
}

void VDP::WriteDataByte(uint8_t value) {
	switch (_portState) {
		case 0:
		case 1:
		case 2:
			this->_vram->WriteByte(this->_address, value);
			this->_readbuffer = value;
			break;
		case 3:
			this->_cram[this->_address & 0x1F] = value;
			break;
	}

	this->_address = (this->_address + 1) & 0x3FFF;
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
			if (this->_ntsc) {
				switch (this->_height) {
					case 192:
						return this->NTSCx192[this->_line];
						break;
					case 224:
						return this->NTSCx224[this->_line];
						break;
					case 240:
						return this->NTSCx240[this->_line];
						break;
				}
			} else {
				switch (this->_height) {
					case 192:
						return this->PALx192[this->_line];
						break;
					case 224:
						return this->PALx224[this->_line];
						break;
					case 240:
						return this->PALx240[this->_line];
						break;
				}
			}
		} else
			return this->HORSYNC[this->_col];
	}

	if (port >= 0x80 && port <= 0xBF) {
		r = true;
		if (even) {
			uint8_t ret;

			switch (this->_portState) {
				default:
				case 0:
					ret = this->_readbuffer;
					this->_readbuffer = this->_vram->ReadByte(this->_address);
					break;
				case 1:
				case 2:
					ret = this->_vram->ReadByte(this->_address);
					break;
				case 3:
					ret = this->_cram[this->_address & 0x1F];
					break;
			}

			this->_address = (this->_address + 1) & 0x3FFF;

			return ret;
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

bool VDP::GetInterrupt() {
	if (this->_interrupt) {
		this->_interrupt = false;
		return true;
	}

	return false;
}

uint8_t VDP::GetBackColor() const {
	return this->_cram[this->_registers[7] & 0x0F];
}
