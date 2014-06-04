#ifndef _AWUI_EMULATION_CHIP8_PROCESSOR_H
#define _AWUI_EMULATION_CHIP8_PROCESSOR_H

#include <awui/String.h>

namespace awui {
	namespace Emulation {
		namespace Chip8 {
			class Screen;
			class Memory;
			class Registers;

			class Processor {
				private:
					int _pc;
					int _sp;
					Screen * _screen;
					Registers * _registers;
					Memory * _memory;

				public:
					Processor();
					~Processor();

					void LoadRom(const String file);
			};
		}
	}
}

#endif