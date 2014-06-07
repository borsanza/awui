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
#include <awui/Emulation/Chip8/Stack.h>
#include <awui/String.h>

using namespace awui::Emulation::Chip8;

Processor::Processor() {
	this->_screen = new Screen(64, 32);
	this->_memory = new Memory(4096);
	this->_registers = new Registers(16);
	this->_stack = new Stack();
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

/*
TODO
0NNN	Calls RCA 1802 program at address NNN.
4XNN	Skips the next instruction if VX doesn't equal NN.
8XY1	Sets VX to VX or VY.
8XY2	Sets VX to VX and VY.
8XY3	Sets VX to VX xor VY.
8XY4	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
8XY5	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
8XY6	Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.[2]
8XY7	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
8XYE	Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.[2]
9XY0	Skips the next instruction if VX doesn't equal VY.
BNNN	Jumps to the address NNN plus V0.
EX9E	Skips the next instruction if the key stored in VX is pressed.
EXA1	Skips the next instruction if the key stored in VX isn't pressed.
FX07	Sets VX to the value of the delay timer.
FX0A	A key press is awaited, and then stored in VX.
FX15	Sets the delay timer to VX.
FX18	Sets the sound timer to VX.
FX29	Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
*/

bool Processor::RunOpcode() {
	bool drawed = 0;
	int opcode1 = this->_memory->ReadByte(this->_pc);
	int opcode2 = this->_memory->ReadByte(this->_pc + 1);
	int op1 = opcode1 >> 4;
	int op2 = opcode1 & 0xf;
	int op3 = opcode2 >> 4;
	int op4 = opcode2 & 0xf;

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
					// 00EE: Returns from a subroutine.
					case 0x0ee:
						this->_pc = this->_stack->Pop();
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
				this->_stack->Push(this->_pc + 2);
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
			switch (op4) {
				// 8XY0: Sets VX to the value of VY.
				case 0x0:
					this->_registers->SetV(op2, this->_registers->GetV(op3));
					this->_pc += 2;
					break;
				default:
					break;
			}
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
			switch (opcode2) {
				// FX1E: Adds VX to I.[3]
				case 0x1E:
					this->_registers->SetI(this->_registers->GetI() + this->_registers->GetV(op2));
					this->_pc += 2;
					break;

				// FX33: Stores the Binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.)
				case 0x33:
					{
						int value = this->_registers->GetV(op2);
						int offset = this->_registers->GetI();
						int units = value % 10;
						value = value / 10;
						int tens = value % 10;
						int hundreds = value / 10;
						this->_memory->WriteByte(offset, hundreds);
						this->_memory->WriteByte(offset + 1, tens);
						this->_memory->WriteByte(offset + 2, units);
						this->_pc += 2;
					}
					break;

				// FX55: Stores V0 to VX in memory starting at address I.
				case 0x55:
					{
						int offset = this->_registers->GetI();
						for (int i = 0; i <= op2; i++)
							this->_memory->WriteByte(offset + i, this->_registers->GetV(i));
						this->_pc += 2;
					}
					break;

				// FX65: Fills V0 to VX with values from memory starting at address I.
				case 0x65:
					{
						int offset = this->_registers->GetI();
						for (int i = 0; i <= op2; i++)
							this->_registers->SetV(i, this->_memory->ReadByte(offset + i));
						this->_pc += 2;
					}
					break;
				default:
					break;
			}
			break;
	}

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