#ifndef _AWUI_EMULATION_CHIP8_REGISTERS_H
#define _AWUI_EMULATION_CHIP8_REGISTERS_H

namespace awui {
	namespace Emulation {
		namespace Chip8 {
			class Registers {
				private:
					unsigned char * _v;
					unsigned char _length;
					unsigned int _i;

				public:
					Registers(unsigned char n);
					~Registers();

					void Clear();

					void SetV(unsigned char pos, unsigned char value);
					unsigned char GetV(unsigned char pos);

					void SetI(unsigned int value);
					unsigned int GetI();
			};
		}
	}
}

#endif