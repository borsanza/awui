#ifndef _AWUI_EMULATION_MASTERSYSTEM_REGISTERS_H
#define _AWUI_EMULATION_MASTERSYSTEM_REGISTERS_H

#include <stdint.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class Registers {
				private:
					uint16_t _sp;

				public:
					Registers();
					~Registers();

					void Clear();

					void SetSP(uint16_t value);
					uint16_t GetSP();
			};
		}
	}
}

#endif