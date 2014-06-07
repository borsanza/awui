/*
 * awui/Emulation/Chip8/Processor.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Processor.h"

#include <assert.h>
#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/Random.h>
#include <awui/Emulation/Chip8/Memory.h>
#include <awui/Emulation/Chip8/Registers.h>
#include <awui/Emulation/Chip8/Screen.h>
#include <awui/String.h>

using namespace awui::Emulation::Chip8;

Processor::Processor() {
	this->_screen = new Screen(64, 32);
	this->_memory = new Memory(4096);
	this->_registers = new Registers(16);
	this->_random = new Random();
	this->_pc = 0x200;
	this->_imageUpdated = true;
	this->_paused = false;
}

Processor::~Processor() {
	delete this->_random;
	delete this->_screen;
	delete this->_memory;
	delete this->_registers;
}

void Processor::LoadRom(const String file) {
	this->_memory->LoadRom(file);
}

void Processor::OnTick() {
	// Intentando 400Hz, similar a 60Hz * 7
	bool draw = 0;
	for (int i = 0; i < 7; i++) {
		if (this->_paused)
			break;

		draw += this->RunOpcode();
	}

//	if (draw)
//		this->_screen->WriteConsole();
}

bool Processor::RunOpcode() {
	bool drawed = 0;
	int opcode1 = this->_memory->ReadByte(this->_pc);
	int opcode2 = this->_memory->ReadByte(this->_pc + 1);
	int op1 = opcode1 >> 4;
	int op2 = opcode1 & 0xf;
	int op3 = opcode2 >> 4;
	int op4 = opcode2 & 0xf;

  // http://en.wikipedia.org/wiki/CHIP-8
	switch (op1) {
		case 0x0:
			{
				int offset = op2 << 8 | opcode2;
				switch (offset) {
					// 00E0: Clears the screen.
					case 0x0e0:
						this->_screen->Clear();
						this->_imageUpdated = true;
						drawed = 1;
						this->_pc += 2;
						break;
					case 0x0ee:
						break;
					default:
						break;
				}
			}
			break;

		// 1NNN: Jumps to address NNN.
		case 0x1:
			{
				int offset = op2 << 8 | opcode2;

				if (offset == this->_pc)
						this->_paused = true;

				this->_pc = offset;
			}
			break;
		// 2NNN: Calls subroutine at NNN.
		case 0x2:
			{
				this->_pc += 2;
				this->_pc = op2 << 8 | opcode2;
			}
			break;

		// 3XNN: Skips the next instruction if VX equals NN.
		case 0x3:
			if (this->_registers->GetV(op2) == opcode2)
				this->_pc += 2;

			this->_pc += 2;
			break;
		case 0x4:
			break;

		// 5XY0: Skips the next instruction if VX equals VY.
		case 0x5:
			assert(op4 == 0);
			if (this->_registers->GetV(op2) == this->_registers->GetV(op3))
				this->_pc += 2;

			this->_pc += 2;
			break;

		// 6XNN: Sets VX to NN.
		case 0x6:
			this->_registers->SetV(op2, opcode2);
			this->_pc += 2;
			break;

		// 7XNN: Adds NN to VX.
		case 0x7:
			this->_registers->SetV(op2, this->_registers->GetV(op2) + opcode2);
			this->_pc += 2;
			break;
		case 0x8:
			break;
		case 0x9:
			break;

		// ANNN: Sets I to the address NNN.
		case 0xa:
			this->_registers->SetI(op2 << 8 | opcode2);
			this->_pc += 2;
			break;
		case 0xb:
			break;

		// CXKK: Sets VX to a random number and NN.
		case 0xc:
			this->_registers->SetV(op2, this->_random->Next(0, 256) & opcode2);
			this->_pc += 2;
			break;

		// DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels
		// and a height of N pixels. Each row of 8 pixels is read as bit-coded
		// (with the most significant bit of each byte displayed on the left)
		// starting from memory location I; I value doesn't change after the
		// execution of this instruction. As described above, VF is set to 1 if any
		// screen pixels are flipped from set to unset when the sprite is drawn,
		// and to 0 if that doesn't happen.
		case 0xd: {
			int x = this->_registers->GetV(op2);
			int y = this->_registers->GetV(op3);
			int height = op4;

			int changed = 0;
			for (int y1 = 0; y1 < height; y1++) {
				uint8_t p = this->_memory->ReadByte(this->_registers->GetI() + y1);
				int bit = 1;
				for (int x1 = 7; x1 >= 0; x1--) {
					int val = (p & bit)? 1 : 0;
					if (this->_screen->SetPixelXOR(x + x1, y + y1, val))
						changed = 1;
					bit  = bit << 1;
				}
			}

			this->_registers->SetV(0xF, changed);

			this->_pc += 2;
			this->_imageUpdated = true;
			drawed = 1;
			}
			break;
		case 0xe:
			break;
		case 0xf:
			break;
	}

	Console::WriteLine("");
	Console::Write(Convert::ToString(this->_pc));
	Console::Write(" : ");
	Console::Write(Convert::ToString(op1));
	Console::Write("-");
	Console::Write(Convert::ToString(op2));
	Console::Write("-");
	Console::Write(Convert::ToString(op3));
	Console::Write("-");
	Console::Write(Convert::ToString(op4));

	return drawed;
}

Screen * Processor::GetScreen() {
	return _screen;
}

bool Processor::GetImageUpdated() {
	return this->_imageUpdated;
}

void Processor::SetImageUpdated(bool mode) {
	this->_imageUpdated = mode;
}