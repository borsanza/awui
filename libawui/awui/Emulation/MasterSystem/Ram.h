#ifndef _AWUI_EMULATION_MASTERSYSTEM_RAM_H
#define _AWUI_EMULATION_MASTERSYSTEM_RAM_H

#include <stdint.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class Ram {
				private:
					uint8_t * _data;
					int32_t _size;

				public:
					Ram(int32_t size);
					~Ram();

					void Clear();
					void Resize(int32_t size);

					uint8_t ReadByte(int64_t pos) const;
					void WriteByte(int64_t pos, uint8_t value);
			};
		}
	}
}

#endif