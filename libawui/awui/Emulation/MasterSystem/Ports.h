#ifndef _AWUI_EMULATION_MASTERSYSTEM_PORTS_H
#define _AWUI_EMULATION_MASTERSYSTEM_PORTS_H

#include <stdint.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class VDP;

			class Ports {
				private:
					VDP * _vdp;

				public:
					Ports();
					~Ports();

					void SetVDP(VDP * vdp);
					void WriteByte(uint8_t port, uint8_t value);
					uint8_t ReadByte(uint8_t port) const;
			};
		}
	}
}

#endif