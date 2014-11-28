#ifndef _AWUI_EMULATION_MASTERSYSTEM_CPU_H
#define _AWUI_EMULATION_MASTERSYSTEM_CPU_H

#include <awui/Emulation/MasterSystem/CPUInst.h>
#include <awui/String.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class CPU : public CPUInst {
				private:
					void Print(const char * str, ...);
					void CheckInterrupts();

				public:
					CPU();
					~CPU();

					void LoadRom(const String file);
					void RunOpcode();
					void OnTick();
					bool IsHalted() const;

					uint16_t GetAddressBus() const;
					void SetAddressBus(uint16_t);

					void Reset();

					void PrintLog();

					void CallPaused();
			};
		}
	}
}

#endif
