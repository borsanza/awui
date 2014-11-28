#include "Processor.h"
#include <awui/IO/FileStream.h>

using namespace awui::Emulation::Chip8;
using namespace awui::IO;

Processor::Processor() {
	for (int i = 0; i < 16  ; i++) _registers[i] = 0;
	for (int i = 0; i < 4096; i++) _memory[i] = 0;
	ClearScreen();
}

void Processor::ClearScreen() {
	for (int i = 0; i < WIDTH * HEIGHT; i++)
		_screen[i] = 0;
}

void Processor::LoadRom(const String file) {
	_pc = 0x200;
	FileStream fs(file, FileMode::Open, FileAccess::Read);

	while (fs.GetPosition() < fs.GetLength())
		_memory[_pc++] = fs.ReadByte();

	fs.Close();
	_pc = 0x200;
}

void Processor::OnTick() {
	// Intentando 400Hz, similar a 60Hz * 7
	bool draw = 0;
	for (int i = 0; i < 7; i++)
		draw += RunOpcode();

	if (draw)
		PrintScreen();
}

bool Processor::RunOpcode() {
	bool drawed = 0;
	int opcode1 = _memory[_pc];
	int opcode2 = _memory[_pc + 1];
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
						ClearScreen();
						_pc += 2;
						break;
				}
			}
			break;

		// 1NNN: Jumps to address NNN.
		case 0x1:
			_pc = op2 << 8 | opcode2;
			break;

		// 3XNN: Skips the next instruction if VX equals NN.
		case 0x3:
			if (_registers[op2] == opcode2)
				_pc += 2;

			_pc += 2;
			break;

		// 5XY0: Skips the next instruction if VX equals VY.
		case 0x5:
			if (_registers[op2] == _registers[op3])
				_pc += 2;

			_pc += 2;
			break;

		// 6XNN: Sets VX to NN.
		case 0x6:
			_registers[op2] = opcode2;
			_pc += 2;
			break;

		// 7XNN: Adds NN to VX.
		case 0x7:
			_registers[op2] = _registers[op2] + opcode2;
			_pc += 2;
			break;

		// ANNN: Sets I to the address NNN.
		case 0xa:
			_i = op2 << 8 | opcode2;
			_pc += 2;
			break;

		// CXKK: Sets VX to a random number and NN.
		case 0xc:
			_registers[op2] = _random.Next(0, 256) & opcode2;
			_pc += 2;
			break;

		// DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels
		// and a height of N pixels. Each row of 8 pixels is read as bit-coded
		// (with the most significant bit of each byte displayed on the left)
		// starting from memory location I; I value doesn't change after the
		// execution of this instruction. As described above, VF is set to 1 if any
		// screen pixels are flipped from set to unset when the sprite is drawn,
		// and to 0 if that doesn't happen.
		case 0xd: {
			int x = _registers[op2];
			int y = _registers[op3];
			int height = op4;

			int changed = 0;
			for (int y1 = 0; y1 < height; y1++) {
				uint8_t p = _memory[_i + y1];
				int bit = 1;
				for (int x1 = 7; x1 >= 0; x1--) {
					int val = (p & bit)? 1 : 0;
					if (SetPixelXOR(x + x1, y + y1, val))
						changed = 1;
					bit  = bit << 1;
				}
			}

			_registers[0xF] = changed;

			_pc += 2;
			drawed = 1;
			}
			break;
	}

	return drawed;
}

bool Processor::SetPixelXOR(int x, int y, bool value) {
	bool ret = false;
	int offset = (y * WIDTH) + x;

	int oldValue = _screen[offset];
	int newValue = oldValue ^ value;

	if (oldValue != newValue) {
		ret = true;
		_screen[offset] = newValue;
	}

	return ret;
}

void Processor::PrintScreen() {
	printf("\n ");
	for (int j = 0; j < WIDTH + 2; j++)
		printf("-");

	printf("\n");
	for (int i = 0; i < HEIGHT; i++) {
		printf(" |");
		for (int j = 0; j < WIDTH; j++) {
			if (_screen[(i * WIDTH) + j] == 1)
				printf("0");
			else
				printf(" ");
		}

		printf("|\n");
	}

	printf(" ");
	for (int j = 0; j < WIDTH + 2; j++)
		printf("-");

	printf("\n");
}

