#ifndef _AWUI_EMULATION_MASTERSYSTEM_RAM_H
#define _AWUI_EMULATION_MASTERSYSTEM_RAM_H

#include <stdint.h>

namespace awui {
	namespace IO {
		class MemoryStream;
	}

	namespace Emulation {
		namespace MasterSystem {
			class Ram {
				private:
					IO::MemoryStream * _memory;

				public:
					Ram();
					~Ram();

					uint8_t ReadByte(int64_t pos);
					void WriteByte(int64_t pos, uint8_t value);
			};
		}
	}
}

#endif