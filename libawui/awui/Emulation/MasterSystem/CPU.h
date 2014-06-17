#ifndef _AWUI_EMULATION_MASTERSYSTEM_CPU_H
#define _AWUI_EMULATION_MASTERSYSTEM_CPU_H

#include <awui/String.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class Rom;

			class CPU {
				private:
					int _pc;
					Rom * _rom;

					void RunOpcode();

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