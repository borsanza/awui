#ifndef _AWUI_EMULATION_MASTERSYSTEM_CPU_H
#define _AWUI_EMULATION_MASTERSYSTEM_CPU_H

#include <awui/String.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class Registers;
			class Rom;

			class CPU {
				private:
					int16_t _pc;
					Registers * _registers;
					Rom * _rom;
					int64_t _cycles;

					void RunOpcode();
					void Reset();

				public:
					CPU();
					~CPU();

					void LoadRom(const String file);
					void OnTick();
			};
		}
	}
}

#endif