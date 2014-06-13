#ifndef _AWUI_EMULATION_CHIP8_OPCODE_H
#define _AWUI_EMULATION_CHIP8_OPCODE_H

#include <stdint.h>

namespace awui {
	namespace Emulation {
		namespace Chip8 {
			enum {
				CHIP8 = 1,
				SUPERCHIP8,
				CHIP8HIRES,
				MEGACHIP8,
			};

			// http://en.wikipedia.org/wiki/CHIP-8
			enum {
				OxNOTIMPLEMENTED = -1,
				Ox0010 = 1,
				Ox0011,
				Ox00BN,
				Ox00CN,
				Ox00E0,
				Ox00EE,
				Ox00FB,
				Ox00FC,
				Ox00FD,
				Ox00FE,
				Ox00FF,
				Ox01NN,
				Ox0230,
				Ox02NN,
				Ox03NN,
				Ox04NN,
				Ox060N,
				Ox0700,
				Ox1NNN,
				Ox2NNN,
				Ox3XKK,
				Ox4XKK,
				Ox5XY0,
				Ox6XKK,
				Ox7XKK,
				Ox8XY0,
				Ox8XY1,
				Ox8XY2,
				Ox8XY3,
				Ox8XY4,
				Ox8XY5,
				Ox8XY6,
				Ox8XY7,
				Ox8XYE,
				Ox9XY0,
				OxANNN,
				OxBNNN,
				OxCXKK,
				OxDXYN,
				OxEX9E,
				OxEXA1,
				OxFX07,
				OxFX0A,
				OxFX15,
				OxFX18,
				OxFX1E,
				OxFX29,
				OxFX30,
				OxFX33,
				OxFX55,
				OxFX65,
				OxFX75,
				OxFX85,
			};

			class Opcode {
				private:
					uint8_t _byte1;
					uint8_t _byte2;

					char DecToHex(int value) const;

				public:
					Opcode();
					~Opcode();

					void SetByte1(uint8_t byte1);
					void SetByte2(uint8_t byte2);

					uint16_t GetOpcode() const;
					uint8_t GetX() const;
					uint8_t GetY() const;
					uint16_t GetNNN() const;
					uint8_t GetKK() const;
					uint8_t GetNN() const;
					uint8_t GetN() const;

					int GetEnum(uint8_t chipmode) const;

					void ShowLog(int pc) const;
			};
		}
	}
}

#endif