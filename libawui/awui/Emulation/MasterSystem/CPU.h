#ifndef _AWUI_EMULATION_MASTERSYSTEM_CPU_H
#define _AWUI_EMULATION_MASTERSYSTEM_CPU_H

#include <awui/Emulation/MasterSystem/Opcode.h>
#include <awui/String.h>

namespace awui {
	namespace Emulation {
		union UINT16 {
				uint16_t _w;
				struct {
						uint8_t _l;
						uint8_t _h;
				};
		};

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
					UINT16 _addressBus;

					float _frame;
					float _oldFrame;
					bool _showLog;

					void RunOpcode();
					void Reset();

					void WriteMemory(uint16_t pos, uint8_t value);

					void ADD(uint8_t value, uint8_t cycles = 4, uint8_t size = 1);
					void ADDHLs(uint8_t reg);
					void ADC(uint8_t b, uint8_t cycles = 4, uint8_t size = 1);
					void AND(uint8_t value, uint8_t cycles = 4, uint8_t size = 1);
					void BIT(uint8_t param, uint8_t compare, uint8_t cycles = 8);
					void CP(uint8_t value, uint8_t cycles = 4, uint8_t size = 1);
					void DECm(uint8_t reg);
					void DECss(uint8_t reg);
					void DECHL();
					void INCr(uint8_t reg);
					void INCss(uint8_t reg);
					void JPccnn(bool cc);
					void JR(bool cc);
					void LDddnn(uint8_t reg);
					void LDHLr(uint8_t reg);
					void LDnndd(uint8_t reg);
					void LDrn(uint8_t reg);
					void LDrr(uint8_t reg1, uint8_t reg2);
					void LDrHL(uint8_t reg);
					void OR(uint8_t value, uint8_t cycles = 4, uint8_t size = 1);
					void OUTCr(uint8_t reg);
					void POPqq(uint8_t reg1, uint8_t reg2);
					void PUSHqq(uint8_t reg1, uint8_t reg2);
					void POP16(uint8_t reg);
					void PUSH16(uint8_t reg);
					void SET(uint8_t reg, uint8_t bit);
					void SETHL(uint8_t bit);
					void RES(uint8_t reg, uint8_t bit);
					void RESHL(uint8_t bit);
					void RET(bool cc, uint8_t cycles = 11);
					void RSTp(uint8_t p);
					void XOR(uint8_t b, uint8_t cycles = 4, uint8_t size = 1);
					void Print(const char * str, ...);

				public:
					CPU();
					~CPU();

					void LoadRom(const String file);
					void OnTick();

					uint16_t GetAddressBus() const;
					void SetAddressBus(uint16_t);
					uint8_t ReadMemory(uint16_t pos);

					VDP * GetVDP();
					Registers * GetRegisters();
			};
		}
	}
}

#endif