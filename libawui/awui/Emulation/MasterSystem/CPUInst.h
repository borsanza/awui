#ifndef _AWUI_EMULATION_MASTERSYSTEM_CPUINST_H
#define _AWUI_EMULATION_MASTERSYSTEM_CPUINST_H

#include <stdint.h>
#include <awui/Emulation/MasterSystem/CPUInst.h>

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

			class CPUInst {
				protected:
					UINT16 _addressBus;
					Ports * _ports;
					Ram * _ram;
					Registers * _registers;
					Rom * _rom;
					int64_t _cycles;

					void Reset();
					void WriteMemory(uint16_t pos, uint8_t value);

					// 8-Bit Load Group
					void LDrr(uint8_t reg1, uint8_t reg2);
					void LDrn(uint8_t reg);
					void LDrHL(uint8_t reg);
					void LDrXXd(uint8_t reg, uint8_t reg2);
					void LDssr(uint8_t reg, uint8_t ss);

					// 16-Bit Load Group
					void LDddnn(uint8_t reg);
					void LDnndd(uint8_t reg);
					void PUSHqq(uint8_t reg1, uint8_t reg2);
					void PUSH16(uint8_t reg);
					void POPqq(uint8_t reg1, uint8_t reg2);
					void POP16(uint8_t reg);

					// Exchange, Block Transfer, and Search Group

					// 8-Bit Arithmetic Group
					void ADD(uint8_t value, uint8_t cycles = 4, uint8_t size = 1);
					void ADC(uint8_t b, uint8_t cycles = 4, uint8_t size = 1);
					void SUB(uint8_t value, uint8_t cycles = 4, uint8_t size = 1);
					void SBC(uint8_t value, uint8_t cycles = 4, uint8_t size = 1);
					void AND(uint8_t value, uint8_t cycles = 4, uint8_t size = 1);
					void OR(uint8_t value, uint8_t cycles = 4, uint8_t size = 1);
					void XOR(uint8_t b, uint8_t cycles = 4, uint8_t size = 1);
					void CP(uint8_t value, uint8_t cycles = 4, uint8_t size = 1);
					void INCr(uint8_t reg);
					void DECm(uint8_t reg);
					void DECHL();
					void INCHL();

					// General-Purpose Arithmetic and CPU Control Group
					void CPL();

					// 16-Bit Arithmetic Group
					void ADDHLss(uint8_t reg);
					void SBCHLss(uint8_t reg);
					void INCss(uint8_t reg);
					void INCXX(uint8_t reg);
					void DECss(uint8_t reg);

					// Rotate and Shift Group
					void RLCA();
					void RLA();
					void RRCA();
					void RRA();
					void RL(uint8_t reg);
					void RR(uint8_t reg);
					void SLA(uint8_t reg);
					void SRA(uint8_t reg);

					// Bit Set, Reset, and Test Group
					void BIT(uint8_t param, uint8_t compare, uint8_t cycles = 8);
					void SET(uint8_t reg, uint8_t bit);
					void SETHL(uint8_t bit);
					void RES(uint8_t reg, uint8_t bit);
					void RESHL(uint8_t bit);
					void SETbssd(uint8_t bit, uint8_t reg, uint8_t d);

					// Jump Group
					void JPccnn(bool cc);
					void JR(bool cc);

					// Call And Return Group
					void RET(bool cc, uint8_t cycles = 11);
					void RSTp(uint8_t p);
					void CALLnn();
					void CALLccnn(bool cc);
					void CallInterrupt();
					void RETI();

					// Input and Output Group
					void OUTnA();
					void OUTCr(uint8_t reg);
					void OUTC();

				public:
					CPUInst();
					~CPUInst();

					uint8_t ReadMemory(uint16_t pos);
			};
		}
	}
}

#endif