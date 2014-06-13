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
					String _file;

				public:
					Memory(int32_t capacity);
					~Memory();

					void LoadRom(const String file);

					uint8_t ReadByte(int64_t pos);
					void WriteByte(int64_t pos, uint8_t value);

					void Reload();
			};
		}
	}
}

#endif