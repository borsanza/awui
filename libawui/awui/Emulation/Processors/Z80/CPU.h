#ifndef _AWUI_EMULATION_PROCESSORS_Z80_CPU_H
#define _AWUI_EMULATION_PROCESSORS_Z80_CPU_H

#include <awui/Emulation/Processors/Z80/CPUInst.h>

namespace awui {
	namespace Emulation {
		namespace Processors {
			namespace Z80 {
				class CPU : public CPUInst {
					private:
						void Print(const char * str, ...);

					public:
						CPU();
						~CPU();

						void RunOpcode();
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
}

#endif
