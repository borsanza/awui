#ifndef _AWUI_EMULATION_CHIP8_CPU_H
#define _AWUI_EMULATION_CHIP8_CPU_H

#include <awui/String.h>

namespace awui {
	class Random;

	namespace Emulation {
		namespace Chip8 {
			class Memory;
			class Registers;
			class Screen;
			class Stack;

			class CPU {
				private:
					int _pc;
					int _sp;
					Screen * _screen;
					Registers * _registers;
					Memory * _memory;
					Random * _random;
					Stack * _stack;
					uint8_t _delayTimer;
					uint8_t _soundTimer;
					int _finished;
					bool _imageUpdated;

					bool RunOpcode();

				public:
					CPU();
					~CPU();

					void LoadRom(const String file);
					void OnTick();

					Screen * GetScreen();

					bool GetImageUpdated();
					void SetImageUpdated(bool mode);
			};
		}
	}
}

#endif