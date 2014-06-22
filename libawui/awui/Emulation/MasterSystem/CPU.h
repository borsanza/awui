#ifndef _AWUI_EMULATION_MASTERSYSTEM_CPU_H
#define _AWUI_EMULATION_MASTERSYSTEM_CPU_H

#include <awui/Emulation/MasterSystem/Opcode.h>
#include <awui/String.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class Ports;
			class Ram;
			class Registers;
			class Rom;
			class VDP;

			class CPU {
				private:
					Ram * _ram;
					Registers * _registers;
					Rom * _rom;
					Ports * _ports;
					VDP * _vdp;
					int64_t _cycles;
					Opcode _opcode;

					float _frame;
					float _oldFrame;
					bool _showLog;

					void RunOpcode();
					void Reset();

					void WriteMemory(uint16_t pos, uint8_t value);
					uint8_t ReadMemory(uint16_t pos);

					void BIT(uint8_t param, uint8_t compare);
					void JR(bool condition);

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