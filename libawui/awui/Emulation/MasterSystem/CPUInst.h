#ifndef _AWUI_EMULATION_MASTERSYSTEM_CPUINST_H
#define _AWUI_EMULATION_MASTERSYSTEM_CPUINST_H

#include <stdint.h>

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

			enum {
				MAPPER_NONE = 1,
				MAPPER_SEGA = 2,
			};

			class CPUInst {
				protected:
					UINT16 _addressBus;
					Ports * _ports;
					Ram * _ram;
					Registers * _registers;
					Rom * _rom;
					Ram * _boardram;
					VDP * _vdp;

					uint8_t _controlbyte;
					uint8_t _frame0;
					uint8_t _frame1;
					uint8_t _frame2;
					uint8_t _mapper;

					int64_t _cycles;

					void WriteMemory(uint16_t pos, uint8_t value);

					// 8-Bit Load Group
					void LDrr(uint8_t reg1, uint8_t reg2, uint8_t cycles, uint8_t size);
					void LDAri(uint8_t value);
					void LDrn(uint8_t reg, uint8_t cycles, uint8_t size);
					void LDrHL(uint8_t reg);
					void LDrXXd(uint8_t reg, uint8_t reg2);
					void LDssr(uint16_t offset, uint8_t value);
					void LDXXdr(uint8_t xx, uint8_t reg);
					void LDXXdn(uint8_t xx);

					// 16-Bit Load Group
					void LDddnn(uint8_t reg, uint8_t size = 3);
					void LDdd_nn(uint8_t reg, uint8_t cycles, uint8_t size);
					void LDnndd(uint8_t reg, uint8_t cycles = 20, uint8_t size = 4);
					void PUSH16(uint8_t reg, uint8_t cycles, uint8_t size);
					void POP16(uint8_t reg, uint8_t cycles, uint8_t size);
					void LDtofrom(uint8_t to, uint16_t value, uint8_t cycles, uint8_t size);

					// Exchange, Block Transfer, and Search Group
					void LDI();
					void LDIR();
					void LDD();
					void LDDR();
					void CPI();
					void CPIR();
					void CPD();
					void CPDR();

					// 8-Bit Arithmetic Group
					void ADD(uint8_t value, uint8_t cycles = 4, uint8_t size = 1);
					void ADC(uint8_t b, uint8_t cycles = 4, uint8_t size = 1);
					void SUB(uint8_t value, uint8_t cycles = 4, uint8_t size = 1);
					void SBC(uint8_t value, uint8_t cycles = 4, uint8_t size = 1);
					void AND(uint8_t value, uint8_t cycles = 4, uint8_t size = 1);
					void OR(uint8_t value, uint8_t cycles = 4, uint8_t size = 1);
					void XOR(uint8_t b, uint8_t cycles = 4, uint8_t size = 1);
					void CP(uint8_t value, uint8_t cycles = 4, uint8_t size = 1);
					void INCr(uint8_t reg, uint8_t cycles, uint8_t size);
					void DECm(uint8_t reg, uint8_t cycles, uint8_t size);
					void DECHL();
					void INCHL();
					void INCXXd(uint8_t xx);
					void DECXXd(uint8_t xx);

					// General-Purpose Arithmetic and CPU Control Group
					void DAA();
					void CPL();
					void NEG();
					void CCF();
					void SCF();

					// 16-Bit Arithmetic Group
					void ADCHLss(uint8_t reg);
					void SBCHLss(uint8_t reg);
					void ADDXXpp(uint8_t XX, uint8_t pp, uint8_t cycles, uint8_t size);
					void INCss(uint8_t reg, uint8_t cycles, uint8_t size);
					void DECss(uint8_t reg, uint8_t cycles, uint8_t size);

					// Rotate and Shift Group
					void RLCA();
					void RLA();
					void RRCA();
					void RRA();

					void RLC(uint8_t reg);
					void RLC_HL();
					void RRC(uint8_t reg);
					void RRC_HL();
					void RL(uint8_t reg);
					void RL_HL();
					void RR(uint8_t reg);
					void RR_HL();
					void SLA(uint8_t reg);
					void SLA_HL();
					void SRA(uint8_t reg);
					void SRA_HL();
					void SRL(uint8_t reg);
					void SRL_HL();
					void SLL(uint8_t reg);
					void SLL_HL();

					void RLCXXd(uint8_t reg);
					void RRCXXd(uint8_t reg);
					void RLXXd(uint8_t reg);
					void RRXXd(uint8_t reg);
					void SLAXXd(uint8_t reg);
					void SRAXXd(uint8_t reg);
					void SLLXXd(uint8_t reg);
					void SRLXXd(uint8_t reg);

					void RLD();
					void RRD();

					// Bit Set, Reset, and Test Group
					void BIT(uint8_t param, uint8_t compare);
					void BITHL(uint8_t compare);
					void SET(uint8_t reg, uint8_t bit);
					void SETHL(uint8_t bit);
					void RES(uint8_t reg, uint8_t bit);
					void RESHL(uint8_t bit);
					void BITbssd(uint8_t bit, uint8_t reg, uint8_t d);
					void RESETbssd(uint8_t bit, uint8_t reg, uint8_t d);
					void SETbssd(uint8_t bit, uint8_t reg, uint8_t d);

					// Jump Group
					void JPccnn(bool cc);
					void JR(bool cc);

					// Call And Return Group
					void RET(bool cc, uint8_t cycles = 11);
					void RSTp(uint8_t p);
					void CALLnn();
					void CALLccnn(bool cc);
					void CallInterrupt(uint16_t offset);
					void RETI();
					void RETN();

					// Input and Output Group
					void OUTnA();
					void OUTCr(uint8_t reg);
					void OUTC();
					void OUTI();
					void OUTD();
					void Reset();

				public:
					CPUInst();
					~CPUInst();

					uint8_t ReadMemory(uint16_t pos) const;

					uint32_t GetCRC32();
					void SetMapper(uint8_t mapper);
			};
		}
	}
}

#endif
