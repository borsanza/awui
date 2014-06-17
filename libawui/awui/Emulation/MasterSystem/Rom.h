#ifndef _AWUI_EMULATION_MASTERSYSTEM_ROM_H
#define _AWUI_EMULATION_MASTERSYSTEM_ROM_H

#include <awui/String.h>

namespace awui {
	namespace IO {
		class MemoryStream;
	}

	namespace Emulation {
		namespace MasterSystem {
			class Rom {
				private:
					IO::MemoryStream * _rom;
					String _file;

				public:
					Rom(int32_t capacity);
					~Rom();

					void LoadRom(const String file);

					uint8_t ReadByte(int64_t pos);
					void WriteByte(int64_t pos, uint8_t value);

					void Reload();
			};
		}
	}
}

#endif