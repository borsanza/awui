/**
 * awui/Emulation/Chip8/CPU.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "CPU.h"

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

#include <stdlib.h>

using namespace awui::Emulation::Chip8;

CPU::CPU() {
	m_seconds = 0.0f;
	m_nextTick = 0.0f;
	m_spriteWidth = 0;
	m_spriteHeight = 0;
	m_pc = 0;
	m_imageUpdated = false;
	m_finished = 0;
	m_soundTimer = 0;
	m_delayTimer = 0;
	m_screen = new Screen(64, 32);
	m_memory = new Memory(4096);
	m_registers = new Registers(16);
	m_input = new Input();
	m_stack = new Stack();
	m_random = new Random();
	m_sound = new Sound();
	m_colors = 0;
	m_frameCounter = 0;
	m_chip8mode = CHIP8;

	Reset();

	m_firstTime = true;
}

CPU::~CPU() {
	m_sound->Stop();

	delete m_input;
	delete m_memory;
	delete m_random;
	delete m_registers;
	delete m_screen;
	delete m_stack;
	delete m_sound;

	if (m_colors)
		free(m_colors);
}

void CPU::LoadRom(const String file) {
	m_memory->LoadRom(file);
}

void CPU::Reset() {
	m_registers->Clear();

  // Es un reset, dudo que cambie el screen o el chipmode...
	m_screen->Clear();

	m_stack->Clear();
	m_delayTimer = 0;
	m_soundTimer = 0;
	m_finished = 0;
	m_pc = 0x200;
	m_imageUpdated = false;
	m_sound->Stop();

	m_memory->Reload();

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
		m_memory->WriteByte(i, fontHex[i]);

	for (int i = 0; i < 160; i++)
		m_memory->WriteByte(i + 256, superFontHex[i]);
}

void CPU::OnTick(float deltaSeconds) {
	m_seconds += deltaSeconds;
	if (m_seconds < m_nextTick) {
		return;
	}

	while (m_seconds >= m_nextTick) {
		m_nextTick += 1.0f / 60.0f;
		DoTick();
	}
}

void CPU::DoTick() {
	m_frameCounter++;

	if (m_firstTime) {
		m_imageUpdated = true;
		m_firstTime = false;
	}

	if (m_delayTimer)
		m_delayTimer--;

	if (m_soundTimer)
		m_soundTimer--;

	float ticks;
	switch (m_chip8mode) {
		default:
		case CHIP8:
			ticks = 400.0f;
			break;
		case CHIP8HIRES:
			ticks = 1000.0f;
			break;
		case SUPERCHIP8:
			ticks = 2000.0f;
			break;
		case MEGACHIP8:
			ticks = 1200000.0f;
			// 30FPS
			if ((m_frameCounter % 2) != 1)
				return;
			break;
	}

	int i;
	int iterations = (int) Math::Round(ticks / 60.0f);

	for (i = 0; i < iterations; i++) {
		if (m_finished)
			break;

		int mode = RunOpcode(i);
		if ((mode == -1) && (m_chip8mode == MEGACHIP8))
			break;
	}

//	Console::WriteLine(Convert::ToString(i));

	if (m_finished)
		m_finished++;

	if (m_soundTimer)
		m_sound->Play();
	else
		m_sound->Stop();

	if (m_finished > 300)
		Reset();
}

int CPU::RunOpcode(int iteration) {
	m_opcode.SetByte1(m_memory->ReadByte(m_pc));
	m_opcode.SetByte2(m_memory->ReadByte(m_pc + 1));

	if ((m_pc == 0x200) && (m_opcode.GetOpcode() == 0x1260)) {
		ChangeResolution(64, 64);
		m_chip8mode = CHIP8HIRES;
		m_opcode.SetByte2(0xc0);
	}

	int drawed = 0;
	int enumopcode = m_opcode.GetEnum(m_chip8mode);
	bool advance = true;
//	_opcode.ShowLog(_pc, enumopcode);

	switch (enumopcode) {
		// Disable Megachip mode
		case Ox0010:
		// Disable extended screen mode (64 x 32)
		case Ox00FE:
			ChangeResolution(64, 32);
			m_chip8mode = CHIP8;
			break;

		// Enable Megachip mode
		case Ox0011:
			ChangeResolution(256, 192);
			m_chip8mode = MEGACHIP8;
			break;

		// Scroll screen Nibble lines up
		case Ox00BN:
			m_screen->ScrollUp(m_opcode.GetN());
			m_imageUpdated = true;
			drawed = 1;
			break;

		// Scroll screen Nibble lines down
		case Ox00CN:
			m_screen->ScrollDown(m_opcode.GetN());
			m_imageUpdated = true;
			drawed = 1;
			break;

		// Chip 8 HiRes
		case Ox0230:
		// Clears the screen
		case Ox00E0:
			if ((iteration != 0) && (m_chip8mode == MEGACHIP8)) {
				drawed = -1;
				advance = false;
				break;
			}

			m_screen->Clear();
			break;

		// Returns from a subroutine
		case Ox00EE:
			m_pc = m_stack->Pop();
			advance = false;
			break;

		// Scroll screen 4 pixels right. 2 pixels in low Mode
		case Ox00FB:
			m_screen->ScrollRight(m_screen->GetWidth() / 32);
			m_imageUpdated = true;
			drawed = 1;
			break;

		// Scroll screen 4 pixels left. 2 pixels in low Mode
		case Ox00FC:
			m_screen->ScrollLeft(m_screen->GetWidth() / 32);
			m_imageUpdated = true;
			drawed = 1;
			break;

		// Exit Chip Interpreter
		case Ox00FD:
			m_finished = 1;
			advance = false;
			break;

		// Enable extended screen mode (128 x 64)
		case Ox00FF:
			ChangeResolution(128, 64);
			m_chip8mode = SUPERCHIP8;
			break;

		// I=(nn<<16)+nnnn , PC+=2;
		case Ox01NN:
			{
				m_pc += 2;
				int byte1 = m_opcode.GetNN();
				int byte2 = m_memory->ReadByte(m_pc);
				int byte3 = m_memory->ReadByte(m_pc + 1);
				uint32_t i = (byte1 << 16) | (byte2 << 8) | byte3;
				m_registers->SetI(i);
			}
			break;

		// Load nn-colors palette at I
		case Ox02NN:
			{
				int total = m_opcode.GetNN();
				uint32_t offset = m_registers->GetI();
				if (m_colors)
					free(m_colors);
				m_colors = (uint32_t *) malloc(sizeof(uint32_t *) * (total + 1));
				m_colors[0] = 0xFF << 24;

				for (int i = 0; i < total; i++) {
					m_colors[i + 1] = 	m_memory->ReadByte(offset) << 24 |
											m_memory->ReadByte(offset + 1) << 16 |
											m_memory->ReadByte(offset + 2) << 8 |
											m_memory->ReadByte(offset + 3);
					offset += 4;
				}
			}
			break;

		// Set Sprite-width to nn		(SPRW  nn)
		case Ox03NN:
			m_spriteWidth = m_opcode.GetNN();
			if (m_spriteWidth == 0)
				m_spriteWidth = 256;
			break;

		// Set Sprite-height to nn	(SPRH  nn)
		case Ox04NN:
			m_spriteHeight = m_opcode.GetNN();
			if (m_spriteHeight == 0)
				m_spriteHeight = 256;
			break;

		// Play digitised sound at I
		case Ox060N:
			Console::WriteLine(Convert::ToString(m_opcode.GetN()));
			break;

		// Stop digitised sound
		case Ox0700:
			Console::WriteLine("Stop");
			break;

		// Jumps to address NNN
		case Ox1NNN:
			{
				uint16_t offset = m_opcode.GetNNN();

				if (offset == m_pc) {
						Console::WriteLine(" --- ROM FINISHED --- ");
						m_finished = 1;
				}

				m_pc = offset;
				advance = false;
			}
			break;

		// Calls subroutine at NNN
		case Ox2NNN:
			m_stack->Push(m_pc + 2);
			m_pc = m_opcode.GetNNN();
			advance = false;
			break;

		// Skips the next instruction if VX equals KK
		case Ox3XKK:
			if (m_registers->GetV(m_opcode.GetX()) == m_opcode.GetKK())
				m_pc += 2;
			break;

		// Skips the next instruction if VX doesn't equal KK
		case Ox4XKK:
			if (m_registers->GetV(m_opcode.GetX()) != m_opcode.GetKK())
				m_pc += 2;
			break;

		// Skips the next instruction if VX equals VY
		case Ox5XY0:
			if (m_registers->GetV(m_opcode.GetX()) == m_registers->GetV(m_opcode.GetY()))
				m_pc += 2;
			break;

		// Store number KK in register VX
		case Ox6XKK:
			m_registers->SetV(m_opcode.GetX(), m_opcode.GetKK());
			break;

		// Add the value KK to register VX
		case Ox7XKK:
			{
				uint8_t x = m_opcode.GetX();
				m_registers->SetV(x, m_registers->GetV(x) + m_opcode.GetKK());
			}
			break;

		// VX = VY
		case Ox8XY0:
			m_registers->SetV(m_opcode.GetX(), m_registers->GetV(m_opcode.GetY()));
			break;

		// VX = VX OR VY
		case Ox8XY1:
			{
				uint8_t x = m_opcode.GetX();
				uint8_t y = m_opcode.GetY();
				m_registers->SetV(x, m_registers->GetV(x) | m_registers->GetV(y));
			}
			break;

		// VX = VX AND VY
		case Ox8XY2:
			{
				uint8_t x = m_opcode.GetX();
				uint8_t y = m_opcode.GetY();
				m_registers->SetV(x, m_registers->GetV(x) & m_registers->GetV(y));
			}
			break;

		// VX = VX XOR VY
		case Ox8XY3:
			{
				uint8_t x = m_opcode.GetX();
				uint8_t y = m_opcode.GetY();
				m_registers->SetV(x, m_registers->GetV(x) ^ m_registers->GetV(y));
			}
			break;


		// VX = VX + VY
		// Set VF to 01 if a carry occurs
		// Set VF to 00 if a carry does not occur
		case Ox8XY4:
			{
				uint8_t x = m_opcode.GetX();
				uint8_t y = m_opcode.GetY();
				int sum = m_registers->GetV(x) + m_registers->GetV(y);
				m_registers->SetV(0xF, (sum > 255)? 1 : 0);
				m_registers->SetV(x, (uint8_t) sum);
			}
			break;

		// VX = VX - VY
		// Set VF to 00 if a borrow occurs
		// Set VF to 01 if a borrow does not occur
		case Ox8XY5:
			{
				uint8_t x = m_opcode.GetX();
				uint8_t vx = m_registers->GetV(x);
				uint8_t vy = m_registers->GetV(m_opcode.GetY());
				m_registers->SetV(0xF, (vy <= vx)? 1 : 0);
				m_registers->SetV(x, (uint8_t) (vx - vy));
			}
			break;

		// VX = VX >> 1
		// Set register VF to the least significant bit prior to the shift
		case Ox8XY6:
			{
				uint8_t x = m_opcode.GetX();
				int value = m_registers->GetV(x);
				m_registers->SetV(0xF, (value & 0x1) ? 1 : 0);
				m_registers->SetV(x, value >> 1);
			}
			break;

		// VX = VY - VX
		// Set VF to 00 if a borrow occurs
		// Set VF to 01 if a borrow does not occur
		case Ox8XY7:
			{
				uint8_t x = m_opcode.GetX();
				uint8_t vx = m_registers->GetV(x);
				uint8_t vy = m_registers->GetV(m_opcode.GetY());
				m_registers->SetV(0xF, (vx <= vy)? 1 : 0);
				m_registers->SetV(x, (uint8_t) (vy - vx));
			}
			break;

		// VX = VX << 1
		// Set register VF to the most significant bit prior to the shift
		case Ox8XYE:
			{
				uint8_t x = m_opcode.GetX();
				uint8_t value = m_registers->GetV(x);
				m_registers->SetV(0xF, (value & 128) ? 1 : 0);
				m_registers->SetV(x, value << 1);
			}
			break;

		// Skips the next instruction if VX doesn't equal VY
		case Ox9XY0:
			if (m_registers->GetV(m_opcode.GetX()) != m_registers->GetV(m_opcode.GetY()))
				m_pc += 2;
			break;

		// Sets I to the address NNN
		case OxANNN:
			m_registers->SetI(m_opcode.GetNNN());
			break;

		// Jumps to the address NNN plus V0.
		case OxBNNN:
			m_pc = m_opcode.GetNNN() + m_registers->GetV(0);
			advance = false;
			break;

		// Sets VX to a random number and NN
		case OxCXKK:
			m_registers->SetV(m_opcode.GetX(), m_random->NextByte() & m_opcode.GetKK());
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
				uint8_t x = m_registers->GetV(m_opcode.GetX());
				uint8_t y = m_registers->GetV(m_opcode.GetY());

				uint8_t pixelCleared = 0;
				uint32_t i = m_registers->GetI();
				uint8_t height = m_opcode.GetN();
				switch (m_chip8mode) {
					default:
					case CHIP8:
					case SUPERCHIP8:
						if ((height == 0) && (m_chip8mode == SUPERCHIP8)) {
							for (int y1 = 0; y1 < 16; y1++) {
								for (int x1 = 0; x1 < 2; x1++) {
									uint8_t p = m_memory->ReadByte(i + (y1 * 2) + x1);
									int bit = 1;
									for (int xbit = 7; xbit >= 0; xbit--) {
										int val = (p & bit)? 1 : 0;
										if (m_screen->SetPixelXOR(x + (x1 * 8) + xbit, y + y1, val))
											pixelCleared = 1;
										bit  = bit << 1;
									}
								}
							}
						} else {
							if (height == 0)
								height = 16;
							for (int y1 = 0; y1 < height; y1++) {
								uint8_t p = m_memory->ReadByte(i + y1);
								int bit = 1;
								for (int x1 = 7; x1 >= 0; x1--) {
									int val = (p & bit)? 1 : 0;
									if (m_screen->SetPixelXOR(x + x1, y + y1, val))
										pixelCleared = 1;
									bit  = bit << 1;
								}
							}
						}

						break;

					case MEGACHIP8:
						for (uint32_t y1 = 0; y1 < m_spriteHeight; y1++) {
							for (uint32_t x1 = 0; x1 < m_spriteWidth; x1++) {
								uint8_t p = m_memory->ReadByte(i + (y1 * m_spriteWidth) + x1);
								if (p == 0)
									continue;
								uint32_t color = m_colors[p];
								m_screen->SetPixel(x + x1, y + y1, color);
							}
						}
						break;
				}

				m_registers->SetV(0xF, pixelCleared);
				m_imageUpdated = true;
				drawed = 1;
			}
			break;

		// Skips the next instruction if the key stored in VX is pressed
		case OxEX9E:
			if (m_input->IsKeyPressed(m_registers->GetV(m_opcode.GetX())))
				m_pc += 2;
			break;

		// Skips the next instruction if the key stored in VX isn't pressed
		case OxEXA1:
			if (!m_input->IsKeyPressed(m_registers->GetV(m_opcode.GetX())))
				m_pc += 2;
			break;

		// Sets VX to the value of the delay timer
		case OxFX07:
			m_registers->SetV(m_opcode.GetX(), m_delayTimer);
			break;

		// A key press is awaited, and then stored in VX
		case OxFX0A:
			{
				int key = m_input->GetKey();
				if (key != -1)
					m_registers->SetV(m_opcode.GetX(), key);
				else
					advance = false;
			}
			break;

		// Sets the delay timer to VX
		case OxFX15:
			m_delayTimer = m_registers->GetV(m_opcode.GetX());
			break;

		// Sets the sound timer to VX
		case OxFX18:
			m_soundTimer = m_registers->GetV(m_opcode.GetX());
			break;

		// I = I + VX
		case OxFX1E:
			{
				int32_t value = m_registers->GetI() + m_registers->GetV(m_opcode.GetX());
				if (m_chip8mode == MEGACHIP8) {
					m_registers->SetV(0xF, (value > 0xFFFFFF) ? 1 : 0);
					m_registers->SetI(value & 0xFFFFFF);
				} else {
					m_registers->SetV(0xF, (value > 0xFFF) ? 1 : 0);
					m_registers->SetI(value & 0xFFF);
				}
			}
			break;

		// Sets I to the location of the sprite for the character in VX.
		// Characters 0-F (in hexadecimal) are represented by a 4x5 font
		case OxFX29:
			m_registers->SetI(m_registers->GetV(m_opcode.GetX()) * 5);
			break;

		// Point I to 10-byte font sprite
		case OxFX30:
			m_registers->SetI(256 + (m_registers->GetV(m_opcode.GetX()) * 10));
			break;

		// Stores the Binary-coded decimal representation of VX, with the
		// most significant of three digits at the address in I, the middle
		// digit at I plus 1, and the least significant digit at I plus 2.
		// (In other words, take the decimal representation of VX, place the
		// hundreds digit in memory at location in I, the tens digit at
		// location I+1, and the ones digit at location I+2.)
		case OxFX33:
			{
				int value = m_registers->GetV(m_opcode.GetX());
				int offset = m_registers->GetI();
				int units = value % 10;
				value = value / 10;
				int tens = value % 10;
				int hundreds = value / 10;
				m_memory->WriteByte(offset, hundreds);
				m_memory->WriteByte(offset + 1, tens);
				m_memory->WriteByte(offset + 2, units);
			}
			break;

		// Stores V0 to VX in memory starting at address I
		// I is set to I + X + 1 after operation
		case OxFX55:
			{
				uint8_t x = m_opcode.GetX();
				int offset = m_registers->GetI();
				for (int i = 0; i <= x; i++)
					m_memory->WriteByte(offset + i, m_registers->GetV(i));

//				_registers->SetI(offset + x + 1);
			}
			break;

		// Fills V0 to VX with values from memory starting at address I
		// I is set to I + X + 1 after operation
		case OxFX65:
			{
				uint8_t x = m_opcode.GetX();
				int offset = m_registers->GetI();
				for (int i = 0; i <= x; i++)
					m_registers->SetV(i, m_memory->ReadByte(offset + i));

//				_registers->SetI(offset + x + 1);
			}
			break;

		// FX75: Stores V0 to VX in RPL memory
		case OxFX75:
			{
				uint8_t x = m_opcode.GetX();
				int offset = 128;
				for (int i = 0; i <= x; i++)
					m_memory->WriteByte(offset + i, m_registers->GetV(i));
			}
			break;

		// FX85: Fills V0 to VX with values from RPL memory
		case OxFX85:
			{
				uint8_t x = m_opcode.GetX();
				int offset = 128;
				for (int i = 0; i <= x; i++)
					m_registers->SetV(i, m_memory->ReadByte(offset + i));
			}
			break;

		default:
		case OxNOTIMPLEMENTED:
			m_opcode.ShowLog(m_pc, enumopcode);
//			assert(0);
			break;
	}

	if (advance)
		m_pc += 2;

	return drawed;
}

Screen * CPU::GetScreen() {
	return m_screen;
}

bool CPU::GetImageUpdated() const {
	return m_imageUpdated;
}

void CPU::SetImageUpdated(bool mode) {
	m_imageUpdated = mode;
}

uint8_t CPU::GetChip8Mode() const {
	return m_chip8mode;
}

void CPU::KeyDown(uint8_t key) {
	m_input->KeyDown(key);
}

void CPU::KeyUp(uint8_t key) {
	m_input->KeyUp(key);
}

void CPU::ChangeResolution(uint16_t width, uint16_t height) {
	if ((m_screen->GetWidth() != width) ||  (m_screen->GetHeight() != height)) {
		delete m_screen;
		m_screen = new Screen(width, height);
	}
}
