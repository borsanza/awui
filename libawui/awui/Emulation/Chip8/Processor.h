#ifndef _AWUI_EMULATION_CHIP8_PROCESSOR_H
#define _AWUI_EMULATION_CHIP8_PROCESSOR_H

#include <awui/Random.h>
#include <awui/String.h>

#include <stdint.h>

#define WIDTH 64
#define HEIGHT 32

namespace awui {
	namespace Emulation {
		namespace Chip8 {
			class Processor {
				private:
					int _pc;
					int _sp;
					Random _random;
					uint16_t _i;
					uint8_t _registers[16];
					uint8_t _memory[4096];
					uint8_t _screen[WIDTH * HEIGHT];

					bool RunOpcode();
					void PrintScreen();
					bool SetPixelXOR(int x, int y, bool value);

				public:
					Processor();

					void LoadRom(const String file);
					void OnTick();
			};
		}
	}
}

#endif
