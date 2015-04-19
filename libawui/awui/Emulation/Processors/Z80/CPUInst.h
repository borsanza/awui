#ifndef _AWUI_EMULATION_PROCESSORS_Z80_CPUINST_H
#define _AWUI_EMULATION_PROCESSORS_Z80_CPUINST_H

#include <awui/Emulation/Processors/Z80/Opcode.h>
#include <awui/Emulation/Processors/Z80/Registers.h>
#include <stdint.h>

namespace awui {
	namespace Emulation {
		namespace Processors {
			namespace Z80 {
				class CPUInst {
					protected:
						struct saveData {
							int64_t _cycles;
							bool _inInterrupt:1;
							bool _isSuspended:1;
							bool _isEndlessLoop:1;
							Word _addressBus;
							Registers _registers;
						} d;

						// No se guarda
						bool _showLog:1;
						bool _showNotImplemented:1;
						Opcode _opcode;

						void (*_writeMemoryCB) (uint16_t pos, uint8_t value, void *);
						void * _writeMemoryDataCB;
						uint8_t (*_readMemoryCB) (uint16_t pos, void *);
						void * _readMemoryDataCB;
						void (*_writePortCB) (uint8_t port, uint8_t value, void *);
						void * _writePortDataCB;
						uint8_t (*_readPortCB) (uint8_t pos, void *);
						void * _readPortDataCB;

						void WriteMemory(uint16_t pos, uint8_t value) const;
						uint8_t ReadPort(uint8_t port) const;
						void WritePort(uint8_t port, uint8_t value) const;

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
						void EX_ss(uint8_t ss1, uint8_t ss2, uint8_t cycles, uint8_t size);
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
						void ADDXXpp(uint8_t XX, uint16_t reg2, uint8_t cycles, uint8_t size);
						void INCss(uint8_t reg, uint8_t cycles, uint8_t size);
						void DECss(uint8_t reg, uint8_t cycles, uint8_t size);

						// Rotate and Shift Group
						void RLCA();
						void RLA();
						void RRCA();
						void RRA();

						void RLC(uint8_t reg);
						void RLC_HL();
						void RLCXXd(uint8_t reg);

						void RRC(uint8_t reg);
						void RRC_HL();
						void RRCXXd(uint8_t reg);

						void RL(uint8_t reg);
						void RL_HL();
						void RLXXd(uint8_t reg);

						void RR(uint8_t reg);
						void RR_HL();
						void RRXXd(uint8_t reg);

						void SLA(uint8_t reg);
						void SLA_HL();
						void SLAXXd(uint8_t reg);

						void SRA(uint8_t reg);
						void SRA_HL();
						void SRAXXd(uint8_t reg);

						void SLL(uint8_t reg);
						void SLL_HL();
						void SLLXXd(uint8_t reg);

						void SRL(uint8_t reg);
						void SRL_HL();
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
						void RETI();
						void RETN();

						// Input and Output Group
						void INrC(uint8_t reg);
						void INI();
						void OUTnA();
						void OUTC(uint8_t value);
						void OUTI();
						void OUTD();

						// Other
						void Reset();

					public:
						CPUInst();
						~CPUInst();

						inline uint16_t GetPC() const { return this->d._registers.GetPC(); }
						inline int64_t GetCycles() const { return this->d._cycles; }
						inline Registers * GetRegisters() { return &(this->d._registers); }
						inline uint32_t GetAddressBus() const { return this->d._addressBus.W; }

						uint8_t ReadMemory(uint16_t pos) const;
						void CallInterrupt(uint16_t offset);

						static int GetSaveSize();
						void LoadState(uint8_t * data);
						void SaveState(uint8_t * data);

						void SetWriteMemoryCB(void (* fun)(uint16_t, uint8_t, void *), void * data);
						void SetReadMemoryCB(uint8_t (* fun)(uint16_t, void *), void * data);
						void SetWritePortCB(void (* fun)(uint8_t, uint8_t, void *), void * data);
						void SetReadPortCB(uint8_t (* fun)(uint8_t, void *), void * data);

						inline void SetInInterrupt(bool mode) { this->d._inInterrupt = mode; }
						inline bool IsInInterrupt() { return this->d._inInterrupt; }
				};
			}
		}
	}
}

#endif
