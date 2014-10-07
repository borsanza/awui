#ifndef _AWUI_EMULATION_MASTERSYSTEM_RAM_H
#define _AWUI_EMULATION_MASTERSYSTEM_RAM_H

#include <stdint.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class Ram {
				private:
					uint8_t * _data;

				public:
					Ram(int32_t size);
					~Ram();

					uint8_t ReadByte(int64_t pos) const;
					void WriteByte(int64_t pos, uint8_t value);
			};
		}
	}
}

#endif