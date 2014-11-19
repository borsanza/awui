#ifndef _AWUI_EMULATION_MASTERSYSTEM_CPU_H
#define _AWUI_EMULATION_MASTERSYSTEM_CPU_H

#include <awui/Emulation/MasterSystem/CPUInst.h>
#include <awui/Emulation/MasterSystem/Opcode.h>
#include <awui/String.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class VDP;

			class CPU : public awui::Emulation::MasterSystem::CPUInst {
				private:
					Opcode _opcode;
					float _frame;
					float _oldFrame;
					bool _showLog;
					bool _showLogInt;
					bool _showNotImplemented;
					bool _enableInterrupts;
					bool _inInterrupt;
					bool _isHalted;

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

					VDP * GetVDP();
					Registers * GetRegisters();

					void Reset();
			};
		}
	}
}

#endif
