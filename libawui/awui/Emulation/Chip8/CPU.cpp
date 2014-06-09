/*
 * awui/Emulation/Chip8/CPU.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "CPU.h"

#include <assert.h>
#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/Random.h>
#include <awui/Emulation/Chip8/Input.h>
#include <awui/Emulation/Chip8/Memory.h>
#include <awui/Emulation/Chip8/Registers.h>
#include <awui/Emulation/Chip8/Screen.h>
#include <awui/Emulation/Chip8/Stack.h>
#include <awui/Emulation/Chip8/Sound.h>
#include <awui/String.h>

using namespace awui::Emulation::Chip8;

CPU::CPU() {
	this->_screen = new Screen(64, 32);
	this->_memory = new Memory(4096);
	this->_registers = new Registers(16);
	this->_input = new Input();
	this->_stack = new Stack();
	this->_random = new Random();
	this->_sound = new Sound();
	this->_pc = 0x200;
	this->_imageUpdated = true;
	this->_finished = 0;
	this->_delayTimer = 0;
	this->_soundTimer = 0;

	uint8_t fontHex[80] = {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	for (int i = 0; i < 80; i++)
		this->_memory->WriteByte(i, fontHex[i]);
}

CPU::~CPU() {
	delete this->_input;
	delete this->_memory;
	delete this->_random;
	delete this->_registers;
	delete this->_screen;
	delete this->_stack;
	delete this->_sound;
}

void CPU::LoadRom(const String file) {
	this->_memory->LoadRom(file);
}

void CPU::OnTick() {
	if (this->_delayTimer)
		this->_delayTimer--;

	if (this->_soundTimer)
		this->_soundTimer--;

	// Intentando 400Hz, similar a 60Hz * 7
	bool draw = 0;
	for (int i = 0; i < 7; i++) {
		if (this->_finished)
			break;

		draw += this->RunOpcode();
	}

	if (this->_finished)
		this->_finished++;

	if (this->_finished > 300) {
		this->_screen->Clear();
		this->_finished = 0;
		this->_pc = 0x200;
	}

	if (this->_soundTimer)
		_sound->Play();
	else
		_sound->Stop();

//	if (draw)
//		this->_screen->WriteConsole();
}

void DebugOpCode(String str) {
//	Console::Write(str);
}

void DebugOpCodeLine(String str) {
//	Console::WriteLine(str);
}

char DecToHex(int value) {
	if ((value >= 10) && (value <= 15))
		return ('A' + value - 10);

	return '0' + value;
}

bool CPU::RunOpcode() {
	bool drawed = 0;
	int opcode1 = this->_memory->ReadByte(this->_pc);
	int opcode2 = this->_memory->ReadByte(this->_pc + 1);
	int op1 = opcode1 >> 4;
	int op2 = opcode1 & 0xf;
	int op3 = opcode2 >> 4;
	int op4 = opcode2 & 0xf;

	DebugOpCode(Convert::ToString(this->_pc));
	DebugOpCode(" : ");
	DebugOpCode(Convert::ToString(DecToHex(op1)));
	DebugOpCode(Convert::ToString(DecToHex(op2)));
	DebugOpCode(Convert::ToString(DecToHex(op3)));
	DebugOpCode(Convert::ToString(DecToHex(op4)));
	DebugOpCode(" : ");

  // http://en.wikipedia.org/wiki/CHIP-8
	switch (op1) {
		case 0x0:
			{
				int offset = op2 << 8 | opcode2;
				switch (offset) {
					// 00E0: Clears the screen
					case 0x0e0:
						this->_screen->Clear();
						this->_imageUpdated = true;
						drawed = 1;
						this->_pc += 2;
						break;
					// 00EE: Returns from a subroutine
					case 0x0ee:
						this->_pc = this->_stack->Pop();
						DebugOpCode("Return ");
						DebugOpCode(Convert::ToString(this->_pc));
						break;
					default:
						break;
				}
			}
			break;

		// 1NNN: Jumps to address NNN
		case 0x1:
			{
				int offset = op2 << 8 | opcode2;

				DebugOpCode("Jump to ");
				DebugOpCode(Convert::ToString(offset));

				if (offset == this->_pc) {
						DebugOpCodeLine(" --- ROM FINISHED --- ");
						this->_finished = 1;
				}

				this->_pc = offset;
			}
			break;

		// 2NNN: Calls subroutine at NNN
		case 0x2:
			{
				int offset = op2 << 8 | opcode2;
				DebugOpCode("Call ");
				DebugOpCode(Convert::ToString(offset));
				this->_stack->Push(this->_pc + 2);
				this->_pc = offset;
			}
			break;

		// 3XNN: Skips the next instruction if VX equals NN
		case 0x3:
			{
				int vx = this->_registers->GetV(op2);
				DebugOpCode("V");
				DebugOpCode(Convert::ToString(DecToHex(op2)));
				DebugOpCode(" == ");
				DebugOpCode(Convert::ToString(opcode2));
				DebugOpCode(" (");
				DebugOpCode(Convert::ToString(vx));
				DebugOpCode(" == ");
				DebugOpCode(Convert::ToString(opcode2));
				DebugOpCode(")");
				if (vx == opcode2)
					this->_pc += 2;

				this->_pc += 2;
			}
			break;

		// 4XNN: Skips the next instruction if VX doesn't equal NN
		case 0x4:
			{
				int value = this->_registers->GetV(op2);
				DebugOpCode("V");
				DebugOpCode(Convert::ToString(DecToHex(op2)));
				DebugOpCode(" != ");
				DebugOpCode(Convert::ToString(opcode2));
				DebugOpCode(" (");
				DebugOpCode(Convert::ToString(value));
				DebugOpCode(" != ");
				DebugOpCode(Convert::ToString(opcode2));
				DebugOpCode(")");

				if (value != opcode2)
					this->_pc += 2;

				this->_pc += 2;
			}
			break;

		// 5XY0: Skips the next instruction if VX equals VY
		case 0x5:
			assert(op4 == 0);
			if (this->_registers->GetV(op2) == this->_registers->GetV(op3))
				this->_pc += 2;

			this->_pc += 2;
			break;

		// 6XNN: Store number NN in register VX
		case 0x6:
			DebugOpCode("V");
			DebugOpCode(Convert::ToString(DecToHex(op2)));
			DebugOpCode(" = ");
			DebugOpCode(Convert::ToString(opcode2));
			this->_registers->SetV(op2, opcode2);
			this->_pc += 2;
			break;

		// 7XNN: Add the value NN to register VX
		case 0x7:
			this->_registers->SetV(op2, this->_registers->GetV(op2) + opcode2);
			this->_pc += 2;
			break;

		case 0x8:
			switch (op4) {
				// 8XY0: VX = VY
				case 0x0:
					this->_registers->SetV(op2, this->_registers->GetV(op3));
					this->_pc += 2;
					break;

				// 8XY1: VX = VX OR VY
				case 0x1:
					this->_registers->SetV(op2, this->_registers->GetV(op2) | this->_registers->GetV(op3));
					this->_pc += 2;
					break;

				// 8XY2: VX = VX AND VY
				case 0x2:
					this->_registers->SetV(op2, this->_registers->GetV(op2) & this->_registers->GetV(op3));
					this->_pc += 2;
					break;

				// 8XY3: VX = VX XOR VY
				case 0x3:
					this->_registers->SetV(op2, this->_registers->GetV(op2) ^ this->_registers->GetV(op3));
					this->_pc += 2;
					break;

				// 8XY4: VX = VX + VY
				//       Set VF to 01 if a carry occurs
				//       Set VF to 00 if a carry does not occur
				case 0x4:
					{
						int sum = this->_registers->GetV(op2) + this->_registers->GetV(op3);
						this->_registers->SetV(0xF, (sum >= 256)? 1 : 0);
						this->_registers->SetV(op2, (uint8_t) sum);
						this->_pc += 2;
					}
					break;

				// 8XY5: VX = VX - VY
				//       Set VF to 00 if a borrow occurs
				//       Set VF to 01 if a borrow does not occur
				case 0x5:
					{
						int subs = this->_registers->GetV(op2) - this->_registers->GetV(op3);
						this->_registers->SetV(0xF, (subs >= 0)? 1 : 0);
						this->_registers->SetV(op2, (uint8_t) subs);
						this->_pc += 2;
					}
					break;

				// 8XY6: VX = VX >> 1
				//       Set register VF to the least significant bit prior to the shift
				case 0x6:
					{
						int value = this->_registers->GetV(op2);
						this->_registers->SetV(0xF, (value & 0x1) ? 1 : 0);
						this->_registers->SetV(op2, value >> 1);
						this->_pc += 2;
					}
					break;

				// 8XY7: VX = VY - VX
				//       Set VF to 00 if a borrow occurs
				//       Set VF to 01 if a borrow does not occur
				case 0x7:
					{
						int subs = this->_registers->GetV(op3) - this->_registers->GetV(op2);
						this->_registers->SetV(0xF, (subs >= 0)? 1 : 0);
						this->_registers->SetV(op2, (uint8_t) subs);
						this->_pc += 2;
					}
					break;

				// 8XYE: VX = VX << 1
				//       Set register VF to the most significant bit prior to the shift
				case 0xE:
					{
						uint8_t value = this->_registers->GetV(op2);
						this->_registers->SetV(0xF, (value & 128) ? 1 : 0);
						this->_registers->SetV(op2, value << 1);
						this->_pc += 2;
					}
					break;
				default:
					break;
			}
			break;

		// 9XY0: Skips the next instruction if VX doesn't equal VY
		case 0x9:
			assert(op4 == 0);
			if (this->_registers->GetV(op2) != this->_registers->GetV(op3))
				this->_pc += 2;

			this->_pc += 2;
			break;

		// ANNN: Sets I to the address NNN
		case 0xA:
			{
				int offset = op2 << 8 | opcode2;
				DebugOpCode("I = ");
				DebugOpCode(Convert::ToString(offset));
				this->_registers->SetI(offset);
				this->_pc += 2;
			}
			break;

		// BNNN: Jumps to the address NNN plus V0.
		case 0xB:
			{
				int offset = op2 << 8 | opcode2;
				offset += this->_registers->GetV(0);

				DebugOpCode("Jump to ");
				DebugOpCode(Convert::ToString(offset));

				this->_pc = offset;
			}
			break;

		// CXKK: Sets VX to a random number and NN
		case 0xC:
			this->_registers->SetV(op2, this->_random->Next(0, 256) & opcode2);
			this->_pc += 2;
			break;

		// DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels
		// and a height of N pixels. Each row of 8 pixels is read as bit-coded
		// (with the most significant bit of each byte displayed on the left)
		// starting from memory location I; I value doesn't change after the
		// execution of this instruction. As described above, VF is set to 1 if any
		// screen pixels are flipped from set to unset when the sprite is drawn,
		// and to 0 if that doesn't happen
		case 0xD: {
			int x = this->_registers->GetV(op2);
			int y = this->_registers->GetV(op3);
			int height = op4;

			int pixelCleared = 0;
			for (int y1 = 0; y1 < height; y1++) {
				uint8_t p = this->_memory->ReadByte(this->_registers->GetI() + y1);
				int bit = 1;
				for (int x1 = 7; x1 >= 0; x1--) {
					int val = (p & bit)? 1 : 0;
					if (this->_screen->SetPixelXOR(x + x1, y + y1, val))
						pixelCleared = 1;
					bit  = bit << 1;
				}
			}

			this->_registers->SetV(0xF, pixelCleared);
			DebugOpCode("Draw(");
			DebugOpCode(Convert::ToString(x));
			DebugOpCode(", ");
			DebugOpCode(Convert::ToString(y));
			DebugOpCode(")");
			this->_pc += 2;
			this->_imageUpdated = true;
			drawed = 1;
			}
			break;

		case 0xE:
			switch (opcode2) {
				// EX9E: Skips the next instruction if the key stored in VX is pressed
				case 0x9E:
					if (this->_input->IsKeyPressed(this->_registers->GetV(op2)))
						this->_pc += 2;

					this->_pc += 2;
					break;

				// EXA1: Skips the next instruction if the key stored in VX isn't pressed
				case 0xA1:
					if (!this->_input->IsKeyPressed(this->_registers->GetV(op2)))
						this->_pc += 2;

					this->_pc += 2;
					break;
			}
			break;
		case 0xF:
			switch (opcode2) {
				// FX0A: A key press is awaited, and then stored in VX
				case 0x0A:
					{
						int key = this->_input->GetKey();
						if (key != -1) {
							this->_registers->SetV(op2, key);
							this->_pc += 2;
						}
					}
					break;
				// FX07: Sets VX to the value of the delay timer
				case 0x07:
					DebugOpCode("V");
					DebugOpCode(Convert::ToString(DecToHex(op2)));
					DebugOpCode(" = ");
					DebugOpCode(Convert::ToString(this->_delayTimer));
					DebugOpCode(" (delay Timer)");
					this->_registers->SetV(op2, this->_delayTimer);
					this->_pc += 2;
					break;
				// FX15: Sets the delay timer to VX
				case 0x15:
					{
						int value = this->_registers->GetV(op2);
						DebugOpCode("_delayTimer");
						DebugOpCode(" = ");
						DebugOpCode(Convert::ToString(value));
						DebugOpCode(" (V");
						DebugOpCode(Convert::ToString(DecToHex(op2)));
						DebugOpCode(")");
						this->_delayTimer = value;
						this->_pc += 2;
					}
					break;
				// FX18: Sets the sound timer to VX
				case 0x18:
					this->_soundTimer = this->_registers->GetV(op2);
					this->_pc += 2;
					break;
				// FX1E: I = VX + I
				case 0x1E:
					this->_registers->SetI(this->_registers->GetI() + this->_registers->GetV(op2));
					this->_pc += 2;
					break;

				// FX29: Sets I to the location of the sprite for the character in VX.
				// Characters 0-F (in hexadecimal) are represented by a 4x5 font
				case 0x29:
					this->_registers->SetI(this->_registers->GetV(op2) * 5);
					this->_pc += 2;
					break;

				// FX33: Stores the Binary-coded decimal representation of VX, with the
				// most significant of three digits at the address in I, the middle
				// digit at I plus 1, and the least significant digit at I plus 2.
				// (In other words, take the decimal representation of VX, place the
				// hundreds digit in memory at location in I, the tens digit at
				// location I+1, and the ones digit at location I+2.)
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

				// FX55: Stores V0 to VX in memory starting at address I
				//       I is set to I + X + 1 after operation
				case 0x55:
					{
						int offset = this->_registers->GetI();
						for (int i = 0; i <= op2; i++)
							this->_memory->WriteByte(offset + i, this->_registers->GetV(i));

						this->_registers->SetI(offset + op2 + 1);
						this->_pc += 2;
					}
					break;

				// FX65: Fills V0 to VX with values from memory starting at address I
				//       I is set to I + X + 1 after operation
				case 0x65:
					{
						int offset = this->_registers->GetI();
						for (int i = 0; i <= op2; i++)
							this->_registers->SetV(i, this->_memory->ReadByte(offset + i));

						this->_registers->SetI(offset + op2 + 1);
						this->_pc += 2;
					}
					break;
				default:
					break;
			}
			break;
	}

	DebugOpCodeLine("");

	return drawed;
}

Screen * CPU::GetScreen() {
	return _screen;
}

bool CPU::GetImageUpdated() {
	return this->_imageUpdated;
}

void CPU::SetImageUpdated(bool mode) {
	this->_imageUpdated = mode;
}
