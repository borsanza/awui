/*
 * awui/Emulation/Chip8/CPU.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "CPU.h"

#include <assert.h>
#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/Emulation/Chip8/Input.h>
#include <awui/Emulation/Chip8/Memory.h>
#include <awui/Emulation/Chip8/Registers.h>
#include <awui/Emulation/Chip8/Screen.h>
#include <awui/Emulation/Chip8/Stack.h>
#include <awui/Emulation/Chip8/Sound.h>
#include <awui/Math.h>
#include <awui/Random.h>
#include <awui/String.h>

using namespace awui::Emulation::Chip8;

// TODO: Mirar de mejorar el borrado de los colores

CPU::CPU() {
	this->_screen = new Screen(64, 32);
	this->_memory = new Memory(4096);
	this->_registers = new Registers(16);
	this->_input = new Input();
	this->_stack = new Stack();
	this->_random = new Random();
	this->_sound = new Sound();
	this->_colors = 0;

	this->Reset();

	this->_firstTime = true;
}

CPU::~CPU() {
	this->_sound->Stop();

	delete this->_input;
	delete this->_memory;
	delete this->_random;
	delete this->_registers;
	delete this->_screen;
	delete this->_stack;
	delete this->_sound;

	if (this->_colors)
		free(this->_colors);
}

void CPU::LoadRom(const String file) {
	this->_memory->LoadRom(file);
}

void CPU::Reset() {
	this->_registers->Clear();
	delete this->_screen;
	this->_screen = new Screen(64, 32);
	this->_stack->Clear();
	this->_delayTimer = 0;
	this->_soundTimer = 0;
	this->_finished = 0;
	this->_pc = 0x200;
	this->_chip8mode = CHIP8;
	this->_imageUpdated = false;
	this->_sound->Stop();

	this->_memory->Reload();

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

	uint8_t superFontHex[160] = {
		0x00, 0x70, 0x88, 0x98, 0xA8, 0xC8, 0x88, 0x70, 0x00, 0x00, // 0
		0x00, 0x20, 0x60, 0x20, 0x20, 0x20, 0x20, 0x70, 0x00, 0x00, // 1
		0x00, 0x70, 0x88, 0x08, 0x10, 0x20, 0x40, 0xF8, 0x00, 0x00, // 2
		0x00, 0xF8, 0x10, 0x20, 0x10, 0x08, 0x88, 0x70, 0x00, 0x00, // 3
		0x00, 0x10, 0x30, 0x50, 0x90, 0xF8, 0x10, 0x10, 0x00, 0x00, // 4
		0x00, 0xF8, 0x80, 0xF0, 0x08, 0x08, 0x88, 0x70, 0x00, 0x00, // 5
		0x00, 0x30, 0x40, 0x80, 0xF0, 0x88, 0x88, 0x70, 0x00, 0x00, // 6
		0x00, 0xF8, 0x08, 0x10, 0x20, 0x40, 0x40, 0x40, 0x00, 0x00, // 7
		0x00, 0x70, 0x88, 0x88, 0x70, 0x88, 0x88, 0x70, 0x00, 0x00, // 8
		0x00, 0x70, 0x88, 0x88, 0x78, 0x08, 0x10, 0x60, 0x00, 0x00, // 9
		0x00, 0x70, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x88, 0x00, 0x00, // A
		0x00, 0xF0, 0x88, 0x88, 0xF0, 0x88, 0x88, 0xF0, 0x00, 0x00, // B
		0x00, 0x70, 0x88, 0x80, 0x80, 0x80, 0x88, 0x70, 0x00, 0x00, // C
		0x00, 0xF0, 0x88, 0x88, 0x88, 0x88, 0x88, 0xF0, 0x00, 0x00, // D
		0x00, 0xF8, 0x80, 0x80, 0xF0, 0x80, 0x80, 0xF8, 0x00, 0x00, // E
		0x00, 0xF8, 0x80, 0x80, 0xF0, 0x80, 0x80, 0x80, 0x00, 0x00, // F
	};

	for (int i = 0; i < 80; i++)
		this->_memory->WriteByte(i, fontHex[i]);

	for (int i = 0; i < 160; i++)
		this->_memory->WriteByte(i + 256, superFontHex[i]);
}

void CPU::OnTick() {
	if (_firstTime) {
		this->_imageUpdated = true;
		_firstTime = false;
	}

	if (this->_delayTimer)
		this->_delayTimer--;

	if (this->_soundTimer)
		this->_soundTimer--;

	float ticks;
	switch (this->_chip8mode) {
		default:
		case CHIP8:
		case CHIP8HIRES:
			ticks = 500.0f;
			break;
		case SUPERCHIP8:
			ticks = 1000.0f;
			break;
		case MEGACHIP8:
			ticks = 1000000.0f;
			break;
	}

	int iterations = (int) Math::Round(ticks / 60.0f);
//	iterations = 2;

	for (int i = 0; i < iterations; i++) {
		if (this->_finished)
			break;

		int mode = this->RunOpcode(i);
		if ((mode == -1) && (this->_chip8mode == MEGACHIP8))
			break;
	}

	if (this->_finished)
		this->_finished++;

	if (this->_soundTimer)
		this->_sound->Play();
	else
		this->_sound->Stop();

	if (this->_finished > 300)
		this->Reset();
}

int CPU::RunOpcode(int iteration) {
	this->_opcode.SetByte1(this->_memory->ReadByte(this->_pc));
	this->_opcode.SetByte2(this->_memory->ReadByte(this->_pc + 1));

	if ((this->_pc == 0x200) && (this->_opcode.GetOpcode() == 0x1260)) {
		if ((this->_screen->GetWidth() != 64) ||  (this->_screen->GetHeight() != 64)) {
			delete this->_screen;
			this->_screen = new Screen(64, 64);
		}

		this->_chip8mode = CHIP8HIRES;
		this->_opcode.SetByte2(0xc0);
	}

	int drawed = 0;
	int enumopcode = this->_opcode.GetEnum(this->_chip8mode);
	// this->_opcode.ShowLog(this->_pc, enumopcode);

	switch (enumopcode) {
		// Disable Megachip mode
		case Ox0010:
			if ((this->_screen->GetWidth() != 64) ||  (this->_screen->GetHeight() != 32)) {
				delete this->_screen;
				this->_screen = new Screen(64, 32);
			}

			this->_chip8mode = CHIP8;
			this->_pc += 2;
			break;

		// Enable Megachip mode
		case Ox0011:
			if ((this->_screen->GetWidth() != 256) ||  (this->_screen->GetHeight() != 192)) {
				delete this->_screen;
				this->_screen = new Screen(256, 192);
			}

			this->_chip8mode = MEGACHIP8;
			this->_pc += 2;
			break;

		// Scroll screen Nibble lines up
		case Ox00BN:
			this->_screen->ScrollUp(this->_opcode.GetN());
			this->_imageUpdated = true;
			drawed = 1;
			this->_pc += 2;
			break;

		// Scroll screen Nibble lines down
		case Ox00CN:
			this->_screen->ScrollDown(this->_opcode.GetN());
			this->_imageUpdated = true;
			drawed = 1;
			this->_pc += 2;
			break;

		// Chip 8 HiRes
		case Ox0230:
		// Clears the screen
		case Ox00E0:
			if ((iteration != 0) && (this->_chip8mode == MEGACHIP8)) {
				drawed = -1;
				break;
			}

			this->_screen->Clear();
			this->_pc += 2;
			break;

		// Returns from a subroutine
		case Ox00EE:
			this->_pc = this->_stack->Pop();
			break;

		// Scroll screen 4 pixels right. 2 pixels in low Mode
		case Ox00FB:
			this->_screen->ScrollRight(this->_screen->GetWidth() / 32);
			this->_imageUpdated = true;
			drawed = 1;
			this->_pc += 2;
			break;

		// Scroll screen 4 pixels left. 2 pixels in low Mode
		case Ox00FC:
			this->_screen->ScrollLeft(this->_screen->GetWidth() / 32);
			this->_imageUpdated = true;
			drawed = 1;
			this->_pc += 2;
			break;

		// Exit Chip Interpreter
		case Ox00FD:
			this->_finished = 1;
			break;

		// Disable extended screen mode (64 x 32)
		case Ox00FE:
			if ((this->_screen->GetWidth() != 64) ||  (this->_screen->GetHeight() != 32)) {
				delete this->_screen;
				this->_screen = new Screen(64, 32);
			}

			this->_chip8mode = CHIP8;
			this->_pc += 2;
			break;

		// Enable extended screen mode (128 x 64)
		case Ox00FF:
			if ((this->_screen->GetWidth() != 128) ||  (this->_screen->GetHeight() != 64)) {
				delete this->_screen;
				this->_screen = new Screen(128, 64);
			}

			this->_chip8mode = SUPERCHIP8;
			this->_pc += 2;
			break;

		// I=(nn<<16)+nnnn , PC+=2;
		case Ox01NN:
			{
				this->_pc += 2;
				int byte1 = this->_opcode.GetNN();
				int byte2 = this->_memory->ReadByte(this->_pc);
				int byte3 = this->_memory->ReadByte(this->_pc + 1);
				uint32_t i = (byte1 << 16) | (byte2 << 8) | byte3;
				this->_registers->SetI(i);
				this->_pc += 2;
			}
			break;

		// Load nn-colors palette at I
		case Ox02NN:
			{
				int total = this->_opcode.GetNN();
				uint32_t offset = this->_registers->GetI();
				if (this->_colors)
					free(this->_colors);
				this->_colors = (uint32_t *) malloc(sizeof(uint32_t *) * (total + 1));
				this->_colors[0] = 0xFF << 24;

				for (int i = 0; i < total; i++) {
					this->_colors[i + 1] = 	this->_memory->ReadByte(offset) << 24 |
											this->_memory->ReadByte(offset + 1) << 16 |
											this->_memory->ReadByte(offset + 2) << 8 |
											this->_memory->ReadByte(offset + 3);
					offset += 4;
				}

				this->_pc += 2;
			}
			break;

		// Set Sprite-width to nn		(SPRW  nn)
		case Ox03NN:
			this->_spriteWidth = this->_opcode.GetNN();
			if (this->_spriteWidth == 0)
				this->_spriteWidth = 256;
			this->_pc += 2;
			break;

		// Set Sprite-height to nn	(SPRH  nn)
		case Ox04NN:
			this->_spriteHeight = this->_opcode.GetNN();
			if (this->_spriteHeight == 0)
				this->_spriteHeight = 256;
			this->_pc += 2;
			break;

		// Play digitised sound at I
		case Ox060N:
			Console::WriteLine(Convert::ToString(this->_opcode.GetN()));
			this->_pc += 2;
			break;

		// Stop digitised sound
		case Ox0700:
			Console::WriteLine("Stop");
			this->_pc += 2;
			break;

		// Jumps to address NNN
		case Ox1NNN:
			{
				uint16_t offset = this->_opcode.GetNNN();

				if (offset == this->_pc) {
						Console::WriteLine(" --- ROM FINISHED --- ");
						this->_finished = 1;
				}

				this->_pc = offset;
			}
			break;

		// Calls subroutine at NNN
		case Ox2NNN:
			{
				this->_stack->Push(this->_pc + 2);
				this->_pc = this->_opcode.GetNNN();
			}
			break;

		// Skips the next instruction if VX equals KK
		case Ox3XKK:
			if (this->_registers->GetV(this->_opcode.GetX()) == this->_opcode.GetKK())
				this->_pc += 2;

			this->_pc += 2;
			break;

		// Skips the next instruction if VX doesn't equal KK
		case Ox4XKK:
			{
				if (this->_registers->GetV(this->_opcode.GetX()) != this->_opcode.GetKK())
					this->_pc += 2;

				this->_pc += 2;
			}
			break;

		// Skips the next instruction if VX equals VY
		case Ox5XY0:
			if (this->_registers->GetV(this->_opcode.GetX()) == this->_registers->GetV(this->_opcode.GetY()))
				this->_pc += 2;

			this->_pc += 2;
			break;

		// Store number KK in register VX
		case Ox6XKK:
			this->_registers->SetV(this->_opcode.GetX(), this->_opcode.GetKK());
			this->_pc += 2;
			break;

		// Add the value KK to register VX
		case Ox7XKK:
			{
				uint8_t x = this->_opcode.GetX();
				this->_registers->SetV(x, this->_registers->GetV(x) + this->_opcode.GetKK());
				this->_pc += 2;
			}
			break;

		// VX = VY
		case Ox8XY0:
			this->_registers->SetV(this->_opcode.GetX(), this->_registers->GetV(this->_opcode.GetY()));
			this->_pc += 2;
			break;

		// VX = VX OR VY
		case Ox8XY1:
			{
				uint8_t x = this->_opcode.GetX();
				uint8_t y = this->_opcode.GetY();
				this->_registers->SetV(x, this->_registers->GetV(x) | this->_registers->GetV(y));
				this->_pc += 2;
			}
			break;

		// VX = VX AND VY
		case Ox8XY2:
			{
				uint8_t x = this->_opcode.GetX();
				uint8_t y = this->_opcode.GetY();
				this->_registers->SetV(x, this->_registers->GetV(x) & this->_registers->GetV(y));
				this->_pc += 2;
			}
			break;

		// VX = VX XOR VY
		case Ox8XY3:
			{
				uint8_t x = this->_opcode.GetX();
				uint8_t y = this->_opcode.GetY();
				this->_registers->SetV(x, this->_registers->GetV(x) ^ this->_registers->GetV(y));
				this->_pc += 2;
			}
			break;


		// VX = VX + VY
		// Set VF to 01 if a carry occurs
		// Set VF to 00 if a carry does not occur
		case Ox8XY4:
			{
				uint8_t x = this->_opcode.GetX();
				uint8_t y = this->_opcode.GetY();
				int sum = this->_registers->GetV(x) + this->_registers->GetV(y);
				this->_registers->SetV(0xF, (sum > 255)? 1 : 0);
				this->_registers->SetV(x, (uint8_t) sum);
				this->_pc += 2;
			}
			break;

		// VX = VX - VY
		// Set VF to 00 if a borrow occurs
		// Set VF to 01 if a borrow does not occur
		case Ox8XY5:
			{
				uint8_t x = this->_opcode.GetX();
				uint8_t vx = this->_registers->GetV(x);
				uint8_t vy = this->_registers->GetV(this->_opcode.GetY());
				this->_registers->SetV(0xF, (vy <= vx)? 1 : 0);
				this->_registers->SetV(x, (uint8_t) (vx - vy));
				this->_pc += 2;
			}
			break;

		// VX = VX >> 1
		// Set register VF to the least significant bit prior to the shift
		case Ox8XY6:
			{
				uint8_t x = this->_opcode.GetX();
				int value = this->_registers->GetV(x);
				this->_registers->SetV(0xF, (value & 0x1) ? 1 : 0);
				this->_registers->SetV(x, value >> 1);
				this->_pc += 2;
			}
			break;

		// VX = VY - VX
		// Set VF to 00 if a borrow occurs
		// Set VF to 01 if a borrow does not occur
		case Ox8XY7:
			{
				uint8_t x = this->_opcode.GetX();
				uint8_t vx = this->_registers->GetV(x);
				uint8_t vy = this->_registers->GetV(this->_opcode.GetY());
				this->_registers->SetV(0xF, (vx <= vy)? 1 : 0);
				this->_registers->SetV(x, (uint8_t) (vy - vx));
				this->_pc += 2;
			}
			break;

		// VX = VX << 1
		// Set register VF to the most significant bit prior to the shift
		case Ox8XYE:
			{
				uint8_t x = this->_opcode.GetX();
				uint8_t value = this->_registers->GetV(x);
				this->_registers->SetV(0xF, (value & 128) ? 1 : 0);
				this->_registers->SetV(x, value << 1);
				this->_pc += 2;
			}
			break;

		// Skips the next instruction if VX doesn't equal VY
		case Ox9XY0:
			if (this->_registers->GetV(this->_opcode.GetX()) != this->_registers->GetV(this->_opcode.GetY()))
				this->_pc += 2;

			this->_pc += 2;
			break;

		// Sets I to the address NNN
		case OxANNN:
			{
				this->_registers->SetI(this->_opcode.GetNNN());
				this->_pc += 2;
			}
			break;

		// Jumps to the address NNN plus V0.
		case OxBNNN:
			{
				this->_pc = this->_opcode.GetNNN() + this->_registers->GetV(0);
			}
			break;

		// Sets VX to a random number and NN
		case OxCXKK:
			this->_registers->SetV(this->_opcode.GetX(), this->_random->NextByte() & this->_opcode.GetKK());
			this->_pc += 2;
			break;

		// Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels
		// and a height of N pixels. Each row of 8 pixels is read as bit-coded
		// (with the most significant bit of each byte displayed on the left)
		// starting from memory location I; I value doesn't change after the
		// execution of this instruction. As described above, VF is set to 1 if any
		// screen pixels are flipped from set to unset when the sprite is drawn,
		// and to 0 if that doesn't happen
		case OxDXYN:
			{
				uint8_t x = this->_registers->GetV(this->_opcode.GetX());
				uint8_t y = this->_registers->GetV(this->_opcode.GetY());

				uint8_t pixelCleared = 0;
				uint32_t i = this->_registers->GetI();
				uint8_t height = this->_opcode.GetN();
				switch (this->_chip8mode) {
					default:
					case CHIP8:
					case SUPERCHIP8:
						if ((height == 0) && (this->_chip8mode == SUPERCHIP8)) {
							for (int y1 = 0; y1 < 16; y1++) {
								for (int x1 = 0; x1 < 2; x1++) {
									uint8_t p = this->_memory->ReadByte(i + (y1 * 2) + x1);
									int bit = 1;
									for (int xbit = 7; xbit >= 0; xbit--) {
										int val = (p & bit)? 1 : 0;
										if (this->_screen->SetPixelXOR(x + (x1 * 8) + xbit, y + y1, val))
											pixelCleared = 1;
										bit  = bit << 1;
									}
								}
							}
						} else {
							if (height == 0)
								height = 16;
							for (int y1 = 0; y1 < height; y1++) {
								uint8_t p = this->_memory->ReadByte(i + y1);
								int bit = 1;
								for (int x1 = 7; x1 >= 0; x1--) {
									int val = (p & bit)? 1 : 0;
									if (this->_screen->SetPixelXOR(x + x1, y + y1, val))
										pixelCleared = 1;
									bit  = bit << 1;
								}
							}
						}

						break;

					case MEGACHIP8:
						for (uint32_t y1 = 0; y1 < this->_spriteHeight; y1++) {
							for (uint32_t x1 = 0; x1 < this->_spriteWidth; x1++) {
								uint8_t p = this->_memory->ReadByte(i + (y1 * this->_spriteWidth) + x1);
								uint32_t color = this->_colors[p];
								this->_screen->SetPixel(x + x1, y + y1, color);
							}
						}
						break;
				}

				this->_registers->SetV(0xF, pixelCleared);
				this->_pc += 2;
				this->_imageUpdated = true;
				drawed = 1;
			}
			break;

		// Skips the next instruction if the key stored in VX is pressed
		case OxEX9E:
			if (this->_input->IsKeyPressed(this->_registers->GetV(this->_opcode.GetX())))
				this->_pc += 2;

			this->_pc += 2;
			break;

		// Skips the next instruction if the key stored in VX isn't pressed
		case OxEXA1:
			if (!this->_input->IsKeyPressed(this->_registers->GetV(this->_opcode.GetX())))
				this->_pc += 2;

			this->_pc += 2;
			break;

		// Sets VX to the value of the delay timer
		case OxFX07:
			this->_registers->SetV(this->_opcode.GetX(), this->_delayTimer);
			this->_pc += 2;
			break;

		// A key press is awaited, and then stored in VX
		case OxFX0A:
			{
				int key = this->_input->GetKey();
				if (key != -1) {
					this->_registers->SetV(this->_opcode.GetX(), key);
					this->_pc += 2;
				}
			}
			break;

		// Sets the delay timer to VX
		case OxFX15:
			this->_delayTimer = this->_registers->GetV(this->_opcode.GetX());
			this->_pc += 2;
			break;

		// Sets the sound timer to VX
		case OxFX18:
			this->_soundTimer = this->_registers->GetV(this->_opcode.GetX());
			this->_pc += 2;
			break;

		// I = I + VX
		case OxFX1E:
			{
				int32_t value = this->_registers->GetI() + this->_registers->GetV(this->_opcode.GetX());
				if (this->_chip8mode == MEGACHIP8) {
					this->_registers->SetV(0xF, (value > 0xFFFFFF) ? 1 : 0);
					this->_registers->SetI(value & 0xFFFFFF);
				} else {
					this->_registers->SetV(0xF, (value > 0xFFF) ? 1 : 0);
					this->_registers->SetI(value & 0xFFF);
				}
				this->_pc += 2;
			}
			break;

		// Sets I to the location of the sprite for the character in VX.
		// Characters 0-F (in hexadecimal) are represented by a 4x5 font
		case OxFX29:
			this->_registers->SetI(this->_registers->GetV(this->_opcode.GetX()) * 5);
			this->_pc += 2;
			break;

		// Point I to 10-byte font sprite
		case OxFX30:
			this->_registers->SetI(256 + (this->_registers->GetV(this->_opcode.GetX()) * 10));
			this->_pc += 2;
			break;

		// Stores the Binary-coded decimal representation of VX, with the
		// most significant of three digits at the address in I, the middle
		// digit at I plus 1, and the least significant digit at I plus 2.
		// (In other words, take the decimal representation of VX, place the
		// hundreds digit in memory at location in I, the tens digit at
		// location I+1, and the ones digit at location I+2.)
		case OxFX33:
			{
				int value = this->_registers->GetV(this->_opcode.GetX());
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

		// Stores V0 to VX in memory starting at address I
		// I is set to I + X + 1 after operation
		case OxFX55:
			{
				uint8_t x = this->_opcode.GetX();
				int offset = this->_registers->GetI();
				for (int i = 0; i <= x; i++)
					this->_memory->WriteByte(offset + i, this->_registers->GetV(i));

//				this->_registers->SetI(offset + x + 1);
				this->_pc += 2;
			}
			break;

		// Fills V0 to VX with values from memory starting at address I
		// I is set to I + X + 1 after operation
		case OxFX65:
			{
				uint8_t x = this->_opcode.GetX();
				int offset = this->_registers->GetI();
				for (int i = 0; i <= x; i++)
					this->_registers->SetV(i, this->_memory->ReadByte(offset + i));

//				this->_registers->SetI(offset + x + 1);
				this->_pc += 2;
			}
			break;

		// FX75: Stores V0 to VX in RPL memory
		case OxFX75:
			{
				uint8_t x = this->_opcode.GetX();
				int offset = 128;
				for (int i = 0; i <= x; i++)
					this->_memory->WriteByte(offset + i, this->_registers->GetV(i));

				this->_pc += 2;
			}
			break;

		// FX85: Fills V0 to VX with values from RPL memory
		case OxFX85:
			{
				uint8_t x = this->_opcode.GetX();
				int offset = 128;
				for (int i = 0; i <= x; i++)
					this->_registers->SetV(i, this->_memory->ReadByte(offset + i));

				this->_pc += 2;
			}
			break;

		default:
		case OxNOTIMPLEMENTED:
			assert(0);
			break;
	}

	return drawed;
}

Screen * CPU::GetScreen() {
	return _screen;
}

bool CPU::GetImageUpdated() const {
	return this->_imageUpdated;
}

void CPU::SetImageUpdated(bool mode) {
	this->_imageUpdated = mode;
}

uint8_t CPU::GetChip8Mode() const {
	return this->_chip8mode;
}