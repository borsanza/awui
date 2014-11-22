#ifndef _AWUI_EMULATION_MASTERSYSTEM_PORTS_H
#define _AWUI_EMULATION_MASTERSYSTEM_PORTS_H

#include <stdint.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class CPU;

			class Ports {
				private:
					CPU * _cpu;
					uint8_t _region;
					bool _getRegion;

				public:
					Ports();
					~Ports();

					void SetCPU(CPU * cpu);
					void WriteByte(uint8_t port, uint8_t value);
					uint8_t ReadByte(uint8_t port) const;
			};
		}
	}
}

#endif
