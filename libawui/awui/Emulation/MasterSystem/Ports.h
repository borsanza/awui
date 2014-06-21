#ifndef _AWUI_EMULATION_MASTERSYSTEM_PORTS_H
#define _AWUI_EMULATION_MASTERSYSTEM_PORTS_H

#include <stdint.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class Ports {
				public:
					Ports();
					~Ports();

					void WriteByte(uint8_t pos, uint8_t value);
					uint8_t ReadByte(uint8_t pos) const;
			};
		}
	}
}

#endif