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
					VDP * _vdp;
					float _frame;
					float _oldFrame;
					bool _showLog;

					void RunOpcode();
					void Print(const char * str, ...);

				public:
					CPU();
					~CPU();

					void LoadRom(const String file);
					void OnTick();

					uint16_t GetAddressBus() const;
					void SetAddressBus(uint16_t);

					VDP * GetVDP();
					Registers * GetRegisters();
			};
		}
	}
}

#endif