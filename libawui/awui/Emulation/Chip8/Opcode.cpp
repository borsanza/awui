/*
 * awui/Emulation/Chip8/Opcode.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Opcode.h"

#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/String.h>

using namespace awui::Emulation::Chip8;

Opcode::Opcode() {
}

Opcode::~Opcode() {
}

void Opcode::SetByte1(uint8_t byte1) {
	this->_byte1 = byte1;
}

void Opcode::SetByte2(uint8_t byte2) {
	this->_byte2 = byte2;
}

uint8_t Opcode::GetX() const {
	return this->_byte1 & 0xf;
}

uint8_t Opcode::GetY() const {
	return this->_byte2 >> 4;
}

uint16_t Opcode::GetNNN() const {
	return ((this->_byte1 & 0xf) << 8) | this->_byte2;
}

uint8_t Opcode::GetKK() const {
	return this->_byte2;
}

uint8_t Opcode::GetNN() const {
	return this->_byte2;
}

uint8_t Opcode::GetN() const {
	return this->_byte2 & 0xf;
}

uint16_t Opcode::GetOpcode() const {
	return (this->_byte1 << 8) | (this->_byte2);
}

int Opcode::GetEnum(uint8_t chipmode) const {
	switch (this->_byte1 >> 4) {
		case 0x0:
			switch (this->GetX()) {
				case 0x0:
					switch (this->GetY()) {
						case 0x1:
							switch (this->GetN()) {
								case 0x0:
									return Ox0010;
								case 0x1:
									return Ox0011;
							}
							break;

						case 0xC:
							return Ox00CN;

						case 0xE:
							switch (this->GetN()) {
								case 0x0:
									return Ox00E0;
								case 0xE:
									return Ox00EE;
							}
							break;

						case 0xF:
							switch (this->GetN()) {
								case 0xB:
									return Ox00FB;

								case 0xC:
									return Ox00FC;

								case 0xD:
									return Ox00FD;

								case 0xE:
									return Ox00FE;

								case 0xF:
									return Ox00FF;
							}
							break;
					}
					break;

				case 0x1:
					return Ox01NN;

				case 0x2:
					if ((chipmode == CHIP8HIRES) && (this->_byte2 == 0x30))
						return Ox0230;

					return Ox02NN;

				case 0x3:
					return Ox03NN;

				case 0x4:
					return Ox04NN;
			}
			break;

		case 0x1:
			return Ox1NNN;

		case 0x2:
			return Ox2NNN;

		case 0x3:
			return Ox3XKK;

		case 0x4:
			return Ox4XKK;

		case 0x5:
			switch (this->GetN()) {
				case 0x0:
					return Ox5XY0;
			}
			break;

		case 0x6:
			return Ox6XKK;

		case 0x7:
			return Ox7XKK;

		case 0x8:
			switch (this->GetN()) {
			case 0x0:
					return Ox8XY0;
				case 0x1:
					return Ox8XY1;
				case 0x2:
					return Ox8XY2;
				case 0x3:
					return Ox8XY3;
				case 0x4:
					return Ox8XY4;
				case 0x5:
					return Ox8XY5;
				case 0x6:
					return Ox8XY6;
				case 0x7:
					return Ox8XY7;
				case 0xE:
					return Ox8XYE;
			}
			break;

		case 0x9:
			switch (this->GetN()) {
				case 0x0:
					return Ox9XY0;
			}
			break;

		case 0xA:
			return OxANNN;

		case 0xB:
			return OxBNNN;

		case 0xC:
			return OxCXKK;

		case 0xD:
			return OxDXYN;

		case 0xE:
			switch (this->_byte2) {
				case 0x9E:
					return OxEX9E;
				case 0xA1:
					return OxEXA1;
			}
			break;

		case 0xF:
			switch (this->_byte2) {
				case 0x07:
					return OxFX07;
				case 0x0A:
					return OxFX0A;
				case 0x15:
					return OxFX15;
				case 0x18:
					return OxFX18;
				case 0x1E:
					return OxFX1E;
				case 0x29:
					return OxFX29;
				case 0x33:
					return OxFX33;
				case 0x55:
					return OxFX55;
				case 0x65:
					return OxFX65;
				case 0x75:
					return OxFX75;
				case 0x85:
					return OxFX85;
			}
			break;
	}

	return OxNOTIMPLEMENTED;
}

char Opcode::DecToHex(int value) const {
	if ((value >= 10) && (value <= 15))
		return ('A' + value - 10);

	return '0' + value;
}

void Opcode::ShowLog(int pc) const {
	Console::Write(Convert::ToString(pc));
	Console::Write(": ");
	Console::Write(Convert::ToString(DecToHex(this->_byte1 >> 4)));
	Console::Write(Convert::ToString(DecToHex(this->_byte1 & 0xF)));
	Console::Write(Convert::ToString(DecToHex(this->_byte2 >> 4)));
	Console::Write(Convert::ToString(DecToHex(this->_byte2 & 0xF)));
	Console::Write(": ");
	Console::WriteLine("");
}