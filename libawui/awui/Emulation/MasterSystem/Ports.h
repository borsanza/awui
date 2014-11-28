#ifndef _AWUI_EMULATION_MASTERSYSTEM_PORTS_H
#define _AWUI_EMULATION_MASTERSYSTEM_PORTS_H

#include <stdint.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class CPUInst;

			class Ports {
				private:
					uint8_t _region;
					uint8_t _maskRegion;
					bool _getRegion:1;

				public:
					Ports();
					~Ports();

					void WriteByte(CPUInst * cpu, uint8_t port, uint8_t value);
					uint8_t ReadByte(CPUInst * cpu, uint8_t port) const;
			};
		}
	}
}

#endif
