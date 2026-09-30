#pragma once

#include <awui/Emulation/Processors/Z80/Opcode.h>
#include <awui/Emulation/Processors/Z80/Registers.h>
#include <stdint.h>

namespace awui::Emulation::Processors::Z80 {
	class CPUInst {
	  private:
		void FillFlags();

	  protected:
		struct saveData {
			int64_t cycles;
			bool inInterrupt : 1;
			bool isSuspended : 1;
			bool isEndlessLoop : 1;
			bool afterEI : 1; // La instrucción anterior fue EI: no se aceptan interrupciones todavía
			Word addressBus;
			Registers registers;
		} m_saveData;

		// No se guarda
		bool m_showLog : 1;
		bool m_showNotImplemented : 1;
		Opcode m_opcode;

		void (*m_writeMemoryCB)(uint16_t pos, uint8_t value, void *);
		void *m_writeMemoryDataCB;
		uint8_t (*m_readMemoryCB)(uint16_t pos, void *);
		void *m_readMemoryDataCB;
		void (*m_writePortCB)(uint8_t port, uint8_t value, void *);
		void *m_writePortDataCB;
		uint8_t (*m_readPortCB)(uint8_t pos, void *);
		void *m_readPortDataCB;

		void WriteMemory(uint16_t pos, uint8_t value);
		uint8_t ReadPort(uint8_t port) const;
		void WritePort(uint8_t port, uint8_t value) const;

		// 8-Bit Load Group
		void LDrr(uint8_t reg1, uint8_t reg2, uint8_t cycles, uint8_t size);
		void LDAri(uint8_t value);
		void LDrn(uint8_t reg);
		void LDriin(uint8_t reg);
		void LDrHL(uint8_t reg);
		void LDrXXd(uint8_t reg, uint8_t reg2);
		void LDssr(uint16_t offset, uint8_t value);
		void LDXXdr(uint8_t xx, uint8_t reg);
		void LDXXdn(uint8_t xx);

		// 16-Bit Load Group
		void LDddnn(uint8_t reg);
		void LDddnnX(uint8_t reg);
		void LDdd_nn(uint8_t reg);
		void LDHL_nn();
		void LDnndd(uint8_t reg);
		void LDHLdd();
		void PUSH16(uint8_t reg);
		void PUSH16X(uint8_t reg);
		void POP16(uint8_t reg);
		void POP16X(uint8_t reg);
		void LDtofrom(uint8_t to, uint16_t value, uint8_t cycles, uint8_t size);

		// Exchange, Block Transfer, and Search Group
		void EX_SPHL();
		void EX_ssX(uint8_t ss2);
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
		void BITbssd(uint8_t bit, uint8_t reg);

		void SET(uint8_t reg, uint8_t bit);
		void SETHL(uint8_t bit);
		void SETbssd(uint8_t bit, uint8_t reg);

		void RES(uint8_t reg, uint8_t bit);
		void RESHL(uint8_t bit);
		void RESbssd(uint8_t bit, uint8_t reg);

		// Jump Group
		void JPccnn(bool cc);
		void JR(bool cc);

		// Call And Return Group
		void RET();
		void RETcc(bool cc);
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
		virtual ~CPUInst();

		inline uint16_t GetPC() const { return m_saveData.registers.GetPC(); }
		inline void SetPC(uint16_t pc) { m_saveData.registers.SetPC(pc); }
		inline int64_t GetCycles() const { return m_saveData.cycles; }
		inline void IncCycles(uint8_t inc) { m_saveData.cycles += inc; }
		inline Registers *GetRegisters() { return &(m_saveData.registers); }
		inline uint32_t GetAddressBus() const { return m_saveData.addressBus.W; }

		uint8_t ReadMemory(uint16_t pos);
		void CallInterrupt(uint16_t offset);

		static int GetSaveSize();
		void LoadState(uint8_t *data);
		void SaveState(uint8_t *data);

		void SetWriteMemoryCB(void (*fun)(uint16_t, uint8_t, void *), void *data);
		void SetReadMemoryCB(uint8_t (*fun)(uint16_t, void *), void *data);
		void SetWritePortCB(void (*fun)(uint8_t, uint8_t, void *), void *data);
		void SetReadPortCB(uint8_t (*fun)(uint8_t, void *), void *data);

		inline void SetInInterrupt(bool mode) { m_saveData.inInterrupt = mode; }
		inline bool IsInInterrupt() const { return m_saveData.inInterrupt; }
		inline bool IsAfterEI() const { return m_saveData.afterEI; }
	};
} // namespace awui::Emulation::Processors::Z80
