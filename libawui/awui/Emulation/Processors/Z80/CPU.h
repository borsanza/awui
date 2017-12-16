#pragma once

#include <awui/Emulation/Common/Word.h>
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
						virtual ~CPU();

						void RunOpcode();
						bool IsEndlessLoop() const;

						Word GetAddressBus() const;
						void SetAddressBus(Word);

						void Reset();

						void PrintLog();
				};
			}
		}
	}
}
