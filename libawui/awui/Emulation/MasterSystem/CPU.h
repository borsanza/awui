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

					void ADDHLs(uint8_t reg);
					void BIT(uint8_t param, uint8_t compare, uint8_t cycles = 8);
					void DECm(uint8_t reg);
					void DECss(uint8_t reg);
					void DECHL();
					void INCr(uint8_t reg);
					void INCss(uint8_t reg);
					void JR(bool condition);
					void LDddnn(uint8_t reg);
					void LDHLr(uint8_t reg);
					void LDnndd(uint8_t reg);
					void LDrn(uint8_t reg);
					void LDrr(uint8_t reg1, uint8_t reg2);
					void ORs(uint8_t reg);
					void POPqq(uint8_t reg1, uint8_t reg2);
					void PUSHqq(uint8_t reg1, uint8_t reg2);
					void XORs(uint8_t b, uint8_t cycles = 4, uint8_t size = 1);

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