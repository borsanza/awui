#ifndef _AWUI_EMULATION_CHIP8_PROCESSOR_H
#define _AWUI_EMULATION_CHIP8_PROCESSOR_H

#include <awui/String.h>

namespace awui {
	class Random;

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
					Random * _random;
					bool _paused = false;
					bool _imageUpdated;

					bool RunOpcode();

				public:
					Processor();
					~Processor();

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