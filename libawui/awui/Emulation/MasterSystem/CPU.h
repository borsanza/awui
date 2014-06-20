#ifndef _AWUI_EMULATION_MASTERSYSTEM_CPU_H
#define _AWUI_EMULATION_MASTERSYSTEM_CPU_H

#include <awui/String.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class Ram;
			class Registers;
			class Rom;

			class CPU {
				private:
					int16_t _pc;
					Ram * _ram;
					Registers * _registers;
					Rom * _rom;
					int64_t _cycles;

					void RunOpcode();
					void Reset();

					void WriteMemory(uint16_t pos, uint8_t value);
					uint8_t ReadMemory(uint16_t pos);

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