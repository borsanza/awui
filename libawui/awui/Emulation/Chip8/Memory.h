#ifndef _AWUI_EMULATION_CHIP8_MEMORY_H
#define _AWUI_EMULATION_CHIP8_MEMORY_H

#include <awui/String.h>

namespace awui {
	namespace IO {
		class MemoryStream;
	}

	namespace Emulation {
		namespace Chip8 {
			class Memory {
				private:
					IO::MemoryStream * _memory;

				public:
					Memory(int32_t capacity);
					~Memory();

					void LoadRom(const String file);
			};
		}
	}
}

#endif