/*
 * awui/Emulation/Processors/Z80/CPUInst.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "CPUInst.h"

#include <string.h>

using namespace awui::Emulation::Processors::Z80;

uint8_t ZS_Flags[256];
uint8_t PZS_Flags[256];

CPUInst::CPUInst() {
	this->FillFlags();
	this->Reset();
}

CPUInst::~CPUInst() {
}

void CPUInst::FillFlags() {
	for (int i = 0; i < 256; i++) {
		int aux = i;
		bool parity = true;
		while (aux != 0) {
			if (aux & 1)
				parity = !parity;

			aux = aux >> 1;
		}

		ZS_Flags[i] = ((i == 0) ? Flag_Z : 0) | ((i > 127) ? Flag_S : 0);
		PZS_Flags[i] = ZS_Flags[i] | (parity ? Flag_P : 0);
	}
}

void CPUInst::Reset() {
	this->d._cycles = 0;
	this->d._inInterrupt = false;
	this->d._isSuspended = false;
	this->d._registers.Clear();
}

void CPUInst::WriteMemory(uint16_t pos, uint8_t value) const {
	this->_writeMemoryCB(pos, value, this->_writeMemoryDataCB);
}

uint8_t CPUInst::ReadMemory(uint16_t pos) const {
	return this->_readMemoryCB(pos, this->_readMemoryDataCB);
}

void CPUInst::WritePort(uint8_t port, uint8_t value) const {
	this->_writePortCB(port, value, this->_writePortDataCB);
}

uint8_t CPUInst::ReadPort(uint8_t port) const {
	return this->_readPortCB(port, this->_readPortDataCB);
}

/******************************************************************************/
/****************************** 8-Bit Load Group ******************************/
/******************************************************************************/

// |1|4| The value are loaded into reg
void CPUInst::LDrr(uint8_t reg, uint8_t value, uint8_t cycles, uint8_t size) {
	this->d._registers.SetRegm(reg, value);
	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

// |1|4| The contents of reg2 are loaded into reg1
void CPUInst::LDAri(uint8_t value) {
	this->d._registers.SetA(value);

	this->d._registers.SetF(
		ZS_Flags[value] |
		(this->d._registers.GetIFF2() ? Flag_P : 0) |
		(this->d._registers.GetF() & Flag_C)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 9;
}

// |2|7| Loads * into reg
void CPUInst::LDrn(uint8_t reg, uint8_t cycles, uint8_t size) {
	this->d._registers.SetRegm(reg, this->ReadMemory(this->d._registers.GetPC() + size - 1));
	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

// |1|7| The contents of (hl) are loaded into reg
void CPUInst::LDrHL(uint8_t reg) {
	this->d._registers.SetRegm(reg, this->ReadMemory(this->d._registers.GetHL()));
	this->d._registers.IncPC();
	this->d._cycles += 7;
}

// |3|19| Loads the value pointed to by ix plus * into reg
void CPUInst::LDrXXd(uint8_t reg, uint8_t reg2) {
	this->d._registers.SetRegm(reg, this->ReadMemory(this->d._registers.GetRegss(reg2) + (int8_t)this->ReadMemory(this->d._registers.GetPC() + 2)));
	this->d._registers.IncPC(3);
	this->d._cycles += 19;
}

// |1|7| The contents of reg are loaded into (ss).
void CPUInst::LDssr(uint16_t offset, uint8_t value) {
	this->WriteMemory(offset, value);
	this->d._registers.IncPC();
	this->d._cycles += 7;
}

// |3|19| Stores reg to the memory location pointed to by xx plus *.
void CPUInst::LDXXdr(uint8_t xx, uint8_t reg) {
	uint16_t x = this->d._registers.GetRegss(xx);
	uint16_t offset = x + ((int8_t) this->ReadMemory(this->d._registers.GetPC() + 2));
	this->WriteMemory(offset, this->d._registers.GetRegm(reg));
	this->d._registers.IncPC(3);
	this->d._cycles += 19;
}

// |4|19| Stores * to the memory location pointed to by xx plus *.
void CPUInst::LDXXdn(uint8_t xx) {
	uint16_t pc = this->d._registers.GetPC();
	uint16_t offset = this->d._registers.GetRegss(xx) + ((int8_t) this->ReadMemory(pc + 2));
	uint8_t n = this->ReadMemory(pc + 3);
	this->WriteMemory(offset, n);
	this->d._registers.IncPC(4);
	this->d._cycles += 19;
}

/******************************************************************************/
/***************************** 16-Bit Load Group ******************************/
/******************************************************************************/

// |3|10| Loads ** into reg
void CPUInst::LDddnn(uint8_t reg, uint8_t size) {
	uint16_t pc = this->d._registers.GetPC();
	this->d._registers.SetRegss(reg, (this->ReadMemory(pc + size - 1) << 8) | this->ReadMemory(pc + size - 2));
	this->d._registers.IncPC(size);
	this->d._cycles += 10;
}

// |4|20| Loads the value pointed to by ** into reg.
void CPUInst::LDdd_nn(uint8_t reg, uint8_t cycles, uint8_t size) {
	uint16_t pc = this->d._registers.GetPC();
	uint16_t offset = (this->ReadMemory(pc + size - 1) << 8) | this->ReadMemory(pc + size - 2);
	this->d._registers.SetRegss(reg, (this->ReadMemory(offset + 1) << 8) | this->ReadMemory(offset));
	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

// |4|20| Stores reg into the memory location pointed to by **
void CPUInst::LDnndd(uint8_t reg, uint8_t cycles, uint8_t size) {
	uint16_t pc = this->d._registers.GetPC();
	uint16_t word = this->d._registers.GetRegss(reg);
	uint8_t high = word >> 8;
	uint8_t low = word;
	uint16_t offset = (this->ReadMemory(pc + size - 1) << 8) | this->ReadMemory(pc + size - 2);
	this->WriteMemory(offset, low);
	this->WriteMemory(offset + 1, high);
	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

// |2|15| sp is decremented and ixh is stored into the memory location pointed to by sp. sp is decremented again and ixl is stored into the memory location pointed to by sp.
void CPUInst::PUSH16(uint8_t reg, uint8_t cycles, uint8_t size) {
	Word value;
	value.W = this->d._registers.GetRegss(reg);
	uint16_t sp = this->d._registers.GetSP();
	this->WriteMemory(sp - 1, value.H);
	this->WriteMemory(sp - 2, value.L);
	this->d._registers.SetSP(sp - 2);
	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

// |2|14| The memory location pointed to by sp is stored into ixl and sp is incremented. The memory location pointed to by sp is stored into ixh and sp is incremented again.
void CPUInst::POP16(uint8_t reg, uint8_t cycles, uint8_t size) {
	uint16_t sp = this->d._registers.GetSP();
	Word value;
	value.H = this->ReadMemory(sp + 1);
	value.L = this->ReadMemory(sp);
	this->d._registers.SetRegss(reg, value.W);
	this->d._registers.SetSP(sp + 2);
	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

void CPUInst::LDtofrom(uint8_t to, uint16_t value, uint8_t cycles, uint8_t size) {
	this->d._registers.SetRegss(to, value);
	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

/******************************************************************************/
/***************** Exchange, Block Transfer, and Search Group *****************/
/******************************************************************************/

// EX (ss1), ss2
// |1|19| Exchanges (ss1) with l, and (ss1+1) with h.
void CPUInst::EX_ss(uint8_t ss1, uint8_t ss2, uint8_t cycles, uint8_t size) {
	uint16_t ss = this->d._registers.GetRegss(ss1);
	uint16_t aux = (this->ReadMemory(ss + 1) << 8) | this->ReadMemory(ss);

	Word value2;
	value2.W = this->d._registers.GetRegss(ss2);

	this->WriteMemory(ss + 1, value2.H);
	this->WriteMemory(ss, value2.L);
	this->d._registers.SetRegss(ss2, aux);

	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

// |2|16| Transfers a byte of data from the memory location pointed to by hl to the memory location pointed to by de. Then hl and de are incremented and bc is decremented.
void CPUInst::LDI() {
	uint8_t dataHL = this->ReadMemory(this->d._registers.GetHL());
	uint8_t valueFlag = dataHL + this->d._registers.GetA();

	this->WriteMemory(this->d._registers.GetDE(), dataHL);

	this->d._registers.SetHL(this->d._registers.GetHL() + 1);
	this->d._registers.SetDE(this->d._registers.GetDE() + 1);
	uint16_t value = this->d._registers.GetBC() - 1;
	this->d._registers.SetBC(value);

	this->d._registers.SetF(
		((valueFlag & 2) ? Flag_F5 : 0) |
		((valueFlag & 8) ? Flag_F3 : 0) |
		((value != 0) ? Flag_V : 0) |
		(this->d._registers.GetF() & (Flag_S | Flag_Z | Flag_C))
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 16;
}

// |2|21/16| Transfers a byte of data from the memory location pointed to by hl to the memory location pointed to by de. Then hl and de are incremented and bc is decremented. If bc is not zero, this operation is repeated. Interrupts can trigger while this instruction is processing.
void CPUInst::LDIR() {
	uint16_t hl = this->d._registers.GetHL();
	uint16_t de = this->d._registers.GetDE();
	uint16_t bc = this->d._registers.GetBC() - 1;
	uint8_t value = this->ReadMemory(hl);
	uint8_t valueFlag = value + this->d._registers.GetA();

	this->WriteMemory(de, value);
	this->d._registers.SetHL(hl + 1);
	this->d._registers.SetDE(de + 1);
	this->d._registers.SetBC(bc);

	this->d._registers.SetF(
		((valueFlag & 2) ? Flag_F5 : 0) |
		((valueFlag & 8) ? Flag_F3 : 0) |
		(this->d._registers.GetF() & (Flag_S | Flag_Z | Flag_C))
	);

	if (bc == 0) {
		this->d._registers.IncPC(2);
		this->d._cycles += 16;
	} else
		this->d._cycles += 21;
}

// |2|16| Transfers a byte of data from the memory location pointed to by hl to the memory location pointed to by de. Then hl, de, and bc are decremented.
void CPUInst::LDD() {
	uint16_t HL = this->d._registers.GetHL();
	uint16_t DE = this->d._registers.GetDE();
	uint16_t BC = this->d._registers.GetBC() - 1;
	uint8_t value = this->ReadMemory(HL);
	uint8_t valueFlag = value + this->d._registers.GetA();

	this->WriteMemory(DE, value);

	this->d._registers.SetHL(HL - 1);
	this->d._registers.SetDE(DE - 1);
	this->d._registers.SetBC(BC);

	this->d._registers.SetF(
		((valueFlag & 2) ? Flag_F5 : 0) |
		((valueFlag & 8) ? Flag_F3 : 0) |
		((BC != 0) ? Flag_P : 0) |
		(this->d._registers.GetF() & (Flag_S | Flag_Z | Flag_C))
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 16;
}

// |2|21/16| Transfers a byte of data from the memory location pointed to by hl to the memory location pointed to by de. Then hl, de, and bc are decremented. If bc is not zero, this operation is repeated. Interrupts can trigger while this instruction is processing.
void CPUInst::LDDR() {
	uint16_t HL = this->d._registers.GetHL();
	uint16_t DE = this->d._registers.GetDE();
	uint16_t BC = this->d._registers.GetBC() - 1;
	uint8_t value = this->ReadMemory(HL);
	uint8_t valueFlag = value + this->d._registers.GetA();

	this->WriteMemory(DE, value);

	this->d._registers.SetHL(HL - 1);
	this->d._registers.SetDE(DE - 1);
	this->d._registers.SetBC(BC);

	this->d._registers.SetF(
		((valueFlag & 2) ? Flag_F5 : 0) |
		((valueFlag & 8) ? Flag_F3 : 0) |
		(this->d._registers.GetF() & (Flag_S | Flag_Z | Flag_C))
	);

	if (BC != 0) {
		this->d._cycles += 21;
	} else {
		this->d._registers.IncPC(2);
		this->d._cycles += 16;
	}
}

// |2|16| Compares the value of the memory location pointed to by hl with a. Then hl is incremented and bc is decremented.
void CPUInst::CPI() {
	uint16_t HL = this->d._registers.GetHL();
	uint8_t b = this->ReadMemory(HL);
	uint8_t old = this->d._registers.GetA();
	uint8_t value = old - b;
	uint16_t BC = this->d._registers.GetBC() - 1;

	this->d._registers.SetHL(HL + 1);
	this->d._registers.SetBC(BC);

	int newH = (value & 0xF) > (old & 0xF);
	this->d._registers.SetF(
		ZS_Flags[value] |
		(newH ? Flag_H : 0) |
		(((value - newH) & 2) ? Flag_F5 : 0) |
		(((value - newH) & 8) ? Flag_F3 : 0) |
		((BC != 0) ? Flag_V : 0) |
		Flag_N |
		(this->d._registers.GetF() & Flag_C)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 16;
}

// |2|21/16| Compares the value of the memory location pointed to by hl with a. Then hl is incremented and bc is decremented. If bc is not zero and z is not set, this operation is repeated. Interrupts can trigger while this instruction is processing.
void CPUInst::CPIR() {
	uint16_t HL = this->d._registers.GetHL();
	uint8_t b = this->ReadMemory(HL);
	uint8_t old = this->d._registers.GetA();
	uint8_t value = old - b;
	uint16_t BC = this->d._registers.GetBC() - 1;

	this->d._registers.SetHL(HL + 1);
	this->d._registers.SetBC(BC);

	int newH = (value & 0xF) > (old & 0xF);
	this->d._registers.SetF(
		ZS_Flags[value] |
		(newH ? Flag_H : 0) |
		(((value - newH) & 2) ? Flag_F5 : 0) |
		(((value - newH) & 8) ? Flag_F3 : 0) |
		((BC != 0) ? Flag_V : 0) |
		Flag_N |
		(this->d._registers.GetF() & Flag_C)
	);

	if ((BC != 0) && (value != 0)) {
		this->d._cycles += 21;
	} else {
		this->d._registers.IncPC(2);
		this->d._cycles += 16;
	}
}

// |2|16| Compares the value of the memory location pointed to by hl with a. Then hl and bc are decremented.
void CPUInst::CPD() {
	uint16_t HL = this->d._registers.GetHL();
	uint8_t b = this->ReadMemory(HL);
	uint8_t old = this->d._registers.GetA();
	uint8_t value = old - b;
	uint16_t BC = this->d._registers.GetBC() - 1;

	this->d._registers.SetHL(HL - 1);
	this->d._registers.SetBC(BC);

	int newH = (value & 0xF) > (old & 0xF);
	this->d._registers.SetF(
		ZS_Flags[value] |
		(newH ? Flag_H : 0) |
		(((value - newH) & 2) ? Flag_F5 : 0) |
		(((value - newH) & 8) ? Flag_F3 : 0) |
		((BC != 0) ? Flag_V : 0) |
		Flag_N |
		(this->d._registers.GetF() & Flag_C)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 16;
}

// |2|21/16| Compares the value of the memory location pointed to by hl with a. Then hl and bc are decremented. If bc is not zero and z is not set, this operation is repeated. Interrupts can trigger while this instruction is processing.
void CPUInst::CPDR() {
	uint16_t HL = this->d._registers.GetHL();
	uint8_t b = this->ReadMemory(HL);
	uint8_t old = this->d._registers.GetA();
	uint8_t value = old - b;
	uint16_t BC = this->d._registers.GetBC() - 1;

	this->d._registers.SetHL(HL - 1);
	this->d._registers.SetBC(BC);

	int newH = (value & 0xF) > (old & 0xF);
	this->d._registers.SetF(
		ZS_Flags[value] |
		(newH ? Flag_H : 0) |
		(((value - newH) & 2) ? Flag_F5 : 0) |
		(((value - newH) & 8) ? Flag_F3 : 0) |
		((BC != 0) ? Flag_V : 0) |
		Flag_N |
		(this->d._registers.GetF() & Flag_C)
	);

	if ((BC != 0) && (value != 0)) {
		this->d._cycles += 21;
	} else {
		this->d._registers.IncPC(2);
		this->d._cycles += 16;
	}
}

/******************************************************************************/
/*************************** 8-Bit Arithmetic Group ***************************/
/******************************************************************************/

// |1|4| Adds valueb to a.
void CPUInst::ADD(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t A = this->d._registers.GetA();
	Word w;
	w.W = ((uint16_t) A) + ((uint16_t) b);

	this->d._registers.SetF(
		(w.L & (Flag_F3 | Flag_F5)) |
		ZS_Flags[w.L] |
		((A ^ b ^ w.L) & Flag_H) |
		((((~(A ^ b)) & (b ^ w.L)) & 0x80) ? Flag_V : 0) |
		w.H
	);

	this->d._registers.SetA(w.L);

	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

// |1|4| Adds l and the carry flag to a.
void CPUInst::ADC(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t A = this->d._registers.GetA();
	Word w;
	w.W = ((uint16_t) A) + ((uint16_t) b) + (this->d._registers.GetF() & Flag_C);

	this->d._registers.SetF(
		(w.L & (Flag_F3 | Flag_F5)) |
		ZS_Flags[w.L] |
		((A ^ b ^ w.L) & Flag_H) |
		((((~(A ^ b)) & (b ^ w.L)) & 0x80) ? Flag_V : 0) |
		w.H
	);

	this->d._registers.SetA(w.L);

	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

// |1|4| Subtracts reg from a.
void CPUInst::SUB(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t A = this->d._registers.GetA();
	int16_t pvalue = ((int16_t) A) - ((int16_t) b);
	uint8_t value = pvalue;

	this->d._registers.SetA(value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		ZS_Flags[value] |
		(((A ^ b ^ value) & 0x10) ? Flag_H : 0) |
		(((A ^ b) & (A ^ value) & 0x80) ? Flag_V : 0) |
		((pvalue < 0) ? Flag_C : 0) |
		Flag_N
	);

	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

// |1|4| Subtracts e and the carry flag from a.
void CPUInst::SBC(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t A = this->d._registers.GetA();
	int16_t pvalue = ((int16_t) A) - ((int16_t) b);
	uint8_t value = pvalue;

	if (this->d._registers.GetF() & Flag_C) {
		value--;
		pvalue--;
	}

	this->d._registers.SetA(value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		ZS_Flags[value] |
		(((A ^ b ^ value) & 0x10) ? Flag_H : 0) |
		(((A ^ b) & (A ^ value) & 0x80) ? Flag_V : 0) |
		((pvalue < 0) ? Flag_C : 0) |
		Flag_N
	);

	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

// |1|4|Bitwise AND on a with valueb.
void CPUInst::AND(uint8_t valueb, uint8_t cycles, uint8_t size) {
	uint8_t value = this->d._registers.GetA() & valueb;

	this->d._registers.SetA(value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		Flag_H
	);

	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

// |1|4| Bitwise OR on a with valueb
void CPUInst::OR(uint8_t valueb, uint8_t cycles, uint8_t size) {
	uint8_t value = this->d._registers.GetA() | valueb;

	this->d._registers.SetA(value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value]
	);

	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

// |1/2|4/7| Bitwise XOR on a with b.
void CPUInst::XOR(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t value = this->d._registers.GetA() ^ b;

	this->d._registers.SetA(value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value]
	);

	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

void CPUInst::CP(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t A = this->d._registers.GetA();
	uint8_t value = A - b;
	uint16_t pvalue = (uint16_t) A - (uint16_t) b;

	this->d._registers.SetF(
		(b & (Flag_F3 | Flag_F5)) |
		ZS_Flags[value] |
		(((A ^ b ^ value) & 0x10) ? Flag_H : 0) |
		Flag_N |
		((pvalue & 0x100) ? Flag_C : 0) |
		(((A ^ b) & (A ^ value) & 0x80) ? Flag_V : 0)
	);

	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

// |1|4| Adds one to reg
void CPUInst::INCr(uint8_t reg, uint8_t cycles, uint8_t size) {
	uint8_t old = this->d._registers.GetRegm(reg);
	uint8_t value = old + 1;

	this->d._registers.SetRegm(reg, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		ZS_Flags[value] |
		((value & 0xF) ? 0 : Flag_H) |
		((old == 0x7F) ? Flag_V : 0) |
		(this->d._registers.GetF() & Flag_C)
	);

	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

// |1|4| Subtracts one from m
void CPUInst::DECm(uint8_t reg, uint8_t cycles, uint8_t size) {
	uint8_t old = this->d._registers.GetRegm(reg);
	uint8_t value = old - 1;

	this->d._registers.SetRegm(reg, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		ZS_Flags[value] |
		(((value & 0xF) == 0xF) ? Flag_H : 0) |
		((old == 0x80) ? Flag_V : 0) |
		Flag_N |
		(this->d._registers.GetF() & Flag_C)
	);

	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

// |1|11| Subtracts one from (hl)
void CPUInst::DECHL() {
	uint16_t HL = this->d._registers.GetHL();
	uint8_t old = this->ReadMemory(HL);
	uint8_t value = old - 1;

 	this->WriteMemory(HL, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		ZS_Flags[value] |
		(((value & 0xF) == 0xF) ? Flag_H : 0) |
		((old == 0x80) ? Flag_V : 0) |
		Flag_N |
		(this->d._registers.GetF() & Flag_C)
	);

	this->d._registers.IncPC();
	this->d._cycles += 11;
}

// |1|11| Adds one to (hl).
void CPUInst::INCHL() {
	uint8_t old = this->ReadMemory(this->d._registers.GetHL());
	uint8_t value = old + 1;

	this->WriteMemory(this->d._registers.GetHL(), value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		ZS_Flags[value] |
		((value & 0xF) ? 0 : Flag_H) |
		((old == 0x7F) ? Flag_V : 0) |
		(this->d._registers.GetF() & Flag_C)
	);

	this->d._registers.IncPC();
	this->d._cycles += 11;
}

// |3|23| Adds one to the memory location pointed to by ix plus *.
void CPUInst::INCXXd(uint8_t xx) {
	uint16_t pc = this->d._registers.GetPC();
	uint16_t offset = this->d._registers.GetRegss(xx) + ((int8_t) this->ReadMemory(pc + 2));
	uint8_t old = this->ReadMemory(offset);
	uint8_t value = old + 1;

	this->WriteMemory(offset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		ZS_Flags[value] |
		((value & 0xF) ? 0 : Flag_H) |
		((old == 0x7F) ? Flag_V : 0) |
		(this->d._registers.GetF() & Flag_C)
	);

	this->d._registers.IncPC(3);
	this->d._cycles += 23;
}

// |3|23| Subtracts one from the memory location pointed to by ix plus *.
void CPUInst::DECXXd(uint8_t xx) {
	uint16_t pc = this->d._registers.GetPC();
	uint16_t offset = this->d._registers.GetRegss(xx) + ((int8_t) this->ReadMemory(pc + 2));
	uint8_t old = this->ReadMemory(offset);
	uint8_t value = old - 1;

	this->WriteMemory(offset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		ZS_Flags[value] |
		(((value & 0xF) == 0xF) ? Flag_H : 0) |
		((old == 0x80) ? Flag_V : 0) |
		Flag_N |
		(this->d._registers.GetF() & Flag_C)
	);

	this->d._registers.IncPC(3);
	this->d._cycles += 23;
}

/******************************************************************************/
/************** General-Purpose Arithmetic and CPU Control Group **************/
/******************************************************************************/

// |1|4| Adjusts a for BCD addition and subtraction operations.
void CPUInst::DAA() {
	uint8_t A = this->d._registers.GetA();
	uint8_t tmp = A;
	bool N = this->d._registers.GetF() & Flag_N;
	bool H = this->d._registers.GetF() & Flag_H;
	bool C = this->d._registers.GetF() & Flag_C;

	if (N) {
		if (H || ((A & 0xF) > 9))
			tmp -= 0x06;
		if (C || (A > 0x99))
			tmp -= 0x60;
	} else {
		if (H || ((A & 0xF) > 9))
			tmp += 0x06;
		if (C || (A > 0x99))
			tmp += 0x60;
	}

	this->d._registers.SetA(tmp);

	this->d._registers.SetF(
		(tmp & (Flag_F3 | Flag_F5)) |
		(((A ^ tmp) & 0x10) ? Flag_H : 0) |
		PZS_Flags[tmp] |
		((C || A > 0x99)? Flag_C : 0) |
		(this->d._registers.GetF() & Flag_N)
	);

	this->d._registers.IncPC();
	this->d._cycles += 4;
}

// |1|4| The contents of a are inverted (one's complement).
void CPUInst::CPL() {
	uint8_t A = ~this->d._registers.GetA();
	this->d._registers.SetA(A);

	this->d._registers.SetF(
		(A & (Flag_F3 | Flag_F5)) |
		Flag_N |
		Flag_H |
		(this->d._registers.GetF() & (Flag_C | Flag_P | Flag_Z | Flag_S))
	);

	this->d._registers.IncPC();
	this->d._cycles += 4;
}

// |2|8| The contents of a are negated (two's complement). Operation is the same as subtracting a from zero.
void CPUInst::NEG() {
	uint8_t A = this->d._registers.GetA();
	uint8_t value = 0 - A;
	this->d._registers.SetA(value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		ZS_Flags[value] |
		(((A ^ value) & 0x10) ? Flag_H : 0) |
		((A == 0x80) ? Flag_V : 0) |
		((A != 0) ? Flag_C : 0) |
		Flag_N
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 8;
}

// |1|4| Inverts the carry flag.
void CPUInst::CCF() {
	bool carry = this->d._registers.GetF() & Flag_C;
	uint8_t A = this->d._registers.GetA();

	this->d._registers.SetF(
		(A & (Flag_F3 | Flag_F5)) |
		(carry ? Flag_H : Flag_C) |
		(this->d._registers.GetF() & (Flag_P | Flag_Z | Flag_S))
	);

	this->d._registers.IncPC();
	this->d._cycles += 4;
}

// |1|4| Sets the carry flag.
void CPUInst::SCF() {
	uint8_t A = this->d._registers.GetA();

	this->d._registers.SetF(
		(A & (Flag_F3 | Flag_F5)) |
		Flag_C |
		(this->d._registers.GetF() & (Flag_P | Flag_Z | Flag_S))
	);

	this->d._registers.IncPC();
	this->d._cycles += 4;
}

/******************************************************************************/
/*************************** 16-Bit Arithmetic Group **************************/
/******************************************************************************/

// |2|15| Adds ss and the carry flag to hl.
void CPUInst::ADCHLss(uint8_t reg) {
	uint16_t hl = this->d._registers.GetHL();
	uint16_t b = this->d._registers.GetRegss(reg);
	Word w;
	w.W = hl + b + (this->d._registers.GetF() & Flag_C);

	this->d._registers.SetHL(w.W);

	this->d._registers.SetF(
		((w.W & Flag_F3H) ? Flag_F3 : 0) |
		((w.W & Flag_F5H) ? Flag_F5 : 0) |
		(w.H & Flag_S) |
		((w.W == 0) ? Flag_Z : 0) |
		(((hl ^ b ^ w.W) & 0x1000) ? Flag_H : 0) |
		(((~(hl ^ b)) & (b ^ w.W) & 0x8000) ? Flag_V : 0) |
		((((uint32_t) hl + (uint32_t) b +  (uint32_t) (this->d._registers.GetF() & Flag_C)) & 0x10000) ? Flag_C : 0)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 15;
}

// |2|15| Subtracts reg and the carry flag from hl.
void CPUInst::SBCHLss(uint8_t reg) {
	uint16_t hl = this->d._registers.GetHL();
	uint16_t b = this->d._registers.GetRegss(reg);
	uint16_t value = hl - b;

	if (this->d._registers.GetF() & Flag_C)
		value--;

	this->d._registers.SetHL(value);

	this->d._registers.SetF(
		((value & Flag_F3H) ? Flag_F3 : 0) |
		((value & Flag_F5H) ? Flag_F5 : 0) |
		((value & 0x8000) ? Flag_S : 0) |
		((value == 0) ? Flag_Z : 0) |
		(((value & 0xFFF) > (hl & 0xFFF)) ? Flag_H : 0) |
		(((hl ^ b) & (hl ^ value) & 0x8000) ? Flag_V : 0) |
		Flag_N |
		((value > hl) ? Flag_C : 0)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 15;
}

// |2|15| The value of pp is added to XX.
void CPUInst::ADDXXpp(uint8_t XX, uint16_t reg2, uint8_t cycles, uint8_t size) {
	uint16_t reg1 = this->d._registers.GetRegss(XX);
	uint32_t value = (uint32_t)reg1 + (uint32_t)reg2;

	this->d._registers.SetRegss(XX, value);

	this->d._registers.SetF(
		(value & Flag_F3H ? Flag_F3 : 0) |
		(value & Flag_F5H ? Flag_F5 : 0) |
		(((reg1 ^ reg2 ^ ((uint16_t) value)) & 0x1000) ? Flag_H : 0) |
		((value & 0x10000) ? Flag_C : 0) |
		(this->d._registers.GetF() & (Flag_Z | Flag_S | Flag_P))
	);

	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

// |1|6| Adds one to reg
void CPUInst::INCss(uint8_t reg, uint8_t cycles, uint8_t size) {
	this->d._registers.SetRegss(reg, this->d._registers.GetRegss(reg) + 1);
	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

// |1|6| Subtracts one from ss
void CPUInst::DECss(uint8_t reg, uint8_t cycles, uint8_t size) {
	this->d._registers.SetRegss(reg, this->d._registers.GetRegss(reg) - 1);
	this->d._registers.IncPC(size);
	this->d._cycles += cycles;
}

/******************************************************************************/
/*************************** Rotate and Shift Group ***************************/
/******************************************************************************/

// |1|4| The contents of a are rotated left one bit position. The contents of bit 7 are copied to the carry flag and bit 0.
void CPUInst::RLCA() {
	uint8_t old = this->d._registers.GetA();
	uint8_t value = (old << 1);
	if (old & 0x80)
		value |= 1;

	this->d._registers.SetA(value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		((old & 0x80) ? Flag_C : 0) |
		(this->d._registers.GetF() & (Flag_Z | Flag_S | Flag_P))
	);

	this->d._registers.IncPC();
	this->d._cycles += 4;
}

// |1|4| The contents of a are rotated left one bit position. The contents of bit 7 are copied to the carry flag and the previous contents of the carry flag are copied to bit 0.
void CPUInst::RLA() {
	uint8_t old = this->d._registers.GetA();
	uint8_t value = (old << 1);
	if (this->d._registers.GetF() & Flag_C)
		value |= 1;

	this->d._registers.SetA(value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		((old & 0x80) ? Flag_C : 0) |
		(this->d._registers.GetF() & (Flag_Z | Flag_S | Flag_P))
	);

	this->d._registers.IncPC();
	this->d._cycles += 4;
}

// |1|4| The contents of a are rotated right one bit position. The contents of bit 0 are copied to the carry flag and bit 7.
void CPUInst::RRCA() {
	uint8_t old = this->d._registers.GetA();
	uint8_t value = (old >> 1);
	if (old & 0x01)
		value |= 0x80;

	this->d._registers.SetA(value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		((old & 0x01) ? Flag_C : 0) |
		(this->d._registers.GetF() & (Flag_Z | Flag_S | Flag_P))
	);

	this->d._registers.IncPC();
	this->d._cycles += 4;
}

// |1|4| The contents of a are rotated right one bit position. The contents of bit 0 are copied to the carry flag and the previous contents of the carry flag are copied to bit 7.
void CPUInst::RRA() {
	uint8_t old = this->d._registers.GetA();
	uint8_t value = (old >> 1);
	if (this->d._registers.GetF() & Flag_C)
		value |= 0x80;

	this->d._registers.SetA(value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		((old & 0x01) ? Flag_C : 0) |
		(this->d._registers.GetF() & (Flag_Z | Flag_S | Flag_P))
	);

	this->d._registers.IncPC();
	this->d._cycles += 4;
}

// |2|8| The contents of b are rotated left one bit position. The contents of bit 7 are copied to the carry flag and bit 0.
void CPUInst::RLC(uint8_t reg) {
	uint8_t old = this->d._registers.GetRegm(reg);
	uint8_t value = (old << 1);
	if (old & 0x80)
		value |= 1;

	this->d._registers.SetRegm(reg, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x80) ? Flag_C : 0)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 8;
}

// |2|15| The contents of (hl) are rotated left one bit position. The contents of bit 7 are copied to the carry flag and bit 0.
void CPUInst::RLC_HL() {
	uint16_t offset = this->d._registers.GetHL();
	uint8_t old = this->ReadMemory(offset);
	uint8_t value = (old << 1);
	if (old & 0x80)
		value |= 1;

	this->WriteMemory(offset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x80) ? Flag_C : 0)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 15;
}

// |4|23| The contents of the memory location pointed to by ix plus * are rotated left one bit position. The contents of bit 7 are copied to the carry flag and bit 0.
void CPUInst::RLCXXd(uint8_t reg) {
	uint16_t XX = this->d._registers.GetRegss(reg);
	uint16_t finalOffset = XX + ((int8_t) this->ReadMemory(this->d._registers.GetPC() + 2));
	uint8_t old = this->ReadMemory(finalOffset);

	uint8_t value = (old << 1);
	if (old & 0x80)
		value |= 1;

	this->WriteMemory(finalOffset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x80) ? Flag_C : 0)
	);

	this->d._registers.IncPC(4);
	this->d._cycles += 23;
}

// |2|8| The contents of b are rotated left one bit position. The contents of bit 7 are copied to the carry flag and the previous contents of the carry flag are copied to bit 0.
void CPUInst::RL(uint8_t reg) {
	uint8_t old = this->d._registers.GetRegm(reg);
	uint8_t value = (old << 1);
	if (this->d._registers.GetF() & Flag_C)
		value |= 1;

	this->d._registers.SetRegm(reg, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x80) ? Flag_C : 0)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 8;
}

// |2|15| The contents of (hl) are rotated left one bit position. The contents of bit 7 are copied to the carry flag and the previous contents of the carry flag are copied to bit 0.
void CPUInst::RL_HL() {
	uint16_t offset = this->d._registers.GetHL();
	uint8_t old = this->ReadMemory(offset);
	uint8_t value = (old << 1);
	if (this->d._registers.GetF() & Flag_C)
		value |= 1;

	this->WriteMemory(offset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x80) ? Flag_C : 0)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 15;
}

// |4|23| The contents of the memory location pointed to by ix plus * are rotated left one bit position. The contents of bit 7 are copied to the carry flag and the previous contents of the carry flag are copied to bit 0.
void CPUInst::RLXXd(uint8_t reg) {
	uint16_t XX = this->d._registers.GetRegss(reg);
	uint16_t finalOffset = XX + ((int8_t) this->ReadMemory(this->d._registers.GetPC() + 2));
	uint8_t old = this->ReadMemory(finalOffset);

	uint8_t value = (old << 1);
	if (this->d._registers.GetF() & Flag_C)
		value |= 1;

	this->WriteMemory(finalOffset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x80) ? Flag_C : 0)
	);

	this->d._registers.IncPC(4);
	this->d._cycles += 23;
}

// |2|8| The contents of b are rotated right one bit position. The contents of bit 0 are copied to the carry flag and bit 7.
void CPUInst::RRC(uint8_t reg) {
	uint8_t old = this->d._registers.GetRegm(reg);
	uint8_t value = (old >> 1);
	if (old & 0x01)
		value |= 0x80;

	this->d._registers.SetRegm(reg, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x01) ? Flag_C : 0)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 8;
}

// |2|15| The contents of (hl) are rotated right one bit position. The contents of bit 0 are copied to the carry flag and bit 7.
void CPUInst::RRC_HL() {
	uint16_t offset = this->d._registers.GetHL();
	uint8_t old = this->ReadMemory(offset);
	uint8_t value = (old >> 1);
	if (old & 0x01)
		value |= 0x80;

	this->WriteMemory(offset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x01) ? Flag_C : 0)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 15;
}

// |4|23| The contents of the memory location pointed to by ix plus * are rotated right one bit position. The contents of bit 0 are copied to the carry flag and bit 7.
void CPUInst::RRCXXd(uint8_t reg) {
	uint16_t XX = this->d._registers.GetRegss(reg);
	uint16_t finalOffset = XX + ((int8_t) this->ReadMemory(this->d._registers.GetPC() + 2));
	uint8_t old = this->ReadMemory(finalOffset);

	uint8_t value = (old >> 1);
	if (old & 0x01)
		value |= 0x80;

	this->WriteMemory(finalOffset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x01) ? Flag_C : 0)
	);

	this->d._registers.IncPC(4);
	this->d._cycles += 23;
}

// |2|8| The contents of b are rotated right one bit position. The contents of bit 0 are copied to the carry flag and the previous contents of the carry flag are copied to bit 7.
void CPUInst::RR(uint8_t reg) {
	uint8_t old = this->d._registers.GetRegm(reg);
	uint8_t value = (old >> 1);
	if (this->d._registers.GetF() & Flag_C)
		value |= 0x80;

	this->d._registers.SetRegm(reg, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x01) ? Flag_C : 0)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 8;
}

// |2|15| The contents of (hl) are rotated right one bit position. The contents of bit 0 are copied to the carry flag and the previous contents of the carry flag are copied to bit 7.
void CPUInst::RR_HL() {
	uint16_t offset = this->d._registers.GetHL();
	uint8_t old = this->ReadMemory(offset);
	uint8_t value = (old >> 1);
	if (this->d._registers.GetF() & Flag_C)
		value |= 0x80;

	this->WriteMemory(offset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x01) ? Flag_C : 0)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 15;
}

// |4|23| The contents of the memory location pointed to by ix plus * are rotated right one bit position. The contents of bit 0 are copied to the carry flag and the previous contents of the carry flag are copied to bit 7.
void CPUInst::RRXXd(uint8_t reg) {
	uint16_t XX = this->d._registers.GetRegss(reg);
	uint16_t finalOffset = XX + ((int8_t) this->ReadMemory(this->d._registers.GetPC() + 2));
	uint8_t old = this->ReadMemory(finalOffset);

	uint8_t value = (old >> 1);
	if (this->d._registers.GetF() & Flag_C)
		value |= 0x80;

	this->WriteMemory(finalOffset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x01) ? Flag_C : 0)
	);

	this->d._registers.IncPC(4);
	this->d._cycles += 23;
}

// |2|8| The contents of b are shifted left one bit position. The contents of bit 7 are copied to the carry flag and a zero is put into bit 0.
void CPUInst::SLA(uint8_t reg) {
	uint8_t old = this->d._registers.GetRegm(reg);
	uint8_t value = old << 1;

	this->d._registers.SetRegm(reg, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x80) ? Flag_C : 0));

	this->d._registers.IncPC(2);
	this->d._cycles += 8;
}

// |2|15| The contents of (hl) are shifted left one bit position. The contents of bit 7 are copied to the carry flag and a zero is put into bit 0.
void CPUInst::SLA_HL() {
	uint16_t offset = this->d._registers.GetHL();
	uint8_t old = this->ReadMemory(offset);
	uint8_t value = old << 1;

	this->WriteMemory(offset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x80) ? Flag_C : 0));

	this->d._registers.IncPC(2);
	this->d._cycles += 15;
}

// |4|23| The contents of the memory location pointed to by ix plus * are shifted left one bit position. The contents of bit 7 are copied to the carry flag and a zero is put into bit 0.
void CPUInst::SLAXXd(uint8_t reg) {
	uint16_t XX = this->d._registers.GetRegss(reg);
	uint16_t finalOffset = XX + ((int8_t) this->ReadMemory(this->d._registers.GetPC() + 2));
	uint8_t old = this->ReadMemory(finalOffset);

	uint8_t value = old << 1;

	this->WriteMemory(finalOffset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x80) ? Flag_C : 0));

	this->d._registers.IncPC(4);
	this->d._cycles += 23;
}

// |2|8| The contents of b are shifted right one bit position. The contents of bit 0 are copied to the carry flag and the previous contents of bit 7 are unchanged.
void CPUInst::SRA(uint8_t reg) {
	uint8_t old = this->d._registers.GetRegm(reg);
	uint8_t value =  (old & 0x80) | (old >> 1);

	this->d._registers.SetRegm(reg, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x01) ? Flag_C : 0));

	this->d._registers.IncPC(2);
	this->d._cycles += 8;
}

// |2|15| The contents of (hl) are shifted right one bit position. The contents of bit 0 are copied to the carry flag and the previous contents of bit 7 are unchanged.
void CPUInst::SRA_HL() {
	uint16_t offset = this->d._registers.GetHL();
	uint8_t old = this->ReadMemory(offset);
	uint8_t value =  (old & 0x80) | (old >> 1);

	this->WriteMemory(offset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x01) ? Flag_C : 0));

	this->d._registers.IncPC(2);
	this->d._cycles += 15;
}

// |4|23| The contents of the memory location pointed to by ix plus * are shifted right one bit position. The contents of bit 0 are copied to the carry flag and the previous contents of bit 7 are unchanged.
void CPUInst::SRAXXd(uint8_t reg) {
	uint16_t XX = this->d._registers.GetRegss(reg);
	uint16_t finalOffset = XX + ((int8_t) this->ReadMemory(this->d._registers.GetPC() + 2));
	uint8_t old = this->ReadMemory(finalOffset);

	uint8_t value =  (old & 0x80) | (old >> 1);

	this->WriteMemory(finalOffset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x01) ? Flag_C : 0));

	this->d._registers.IncPC(4);
	this->d._cycles += 23;
}

// |2|8| The contents of b are shifted left one bit position. The contents of bit 7 are put into the carry flag and a one is put into bit 0.
void CPUInst::SLL(uint8_t reg) {
	uint8_t old = this->d._registers.GetRegm(reg);
	uint8_t value = (old << 1) | 1;

	this->d._registers.SetRegm(reg, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x80) ? Flag_C : 0));

	this->d._registers.IncPC(2);
	this->d._cycles += 8;
}

// |2|15| The contents of (hl) are shifted left one bit position. The contents of bit 7 are put into the carry flag and a one is put into bit 0.
void CPUInst::SLL_HL() {
	uint16_t offset = this->d._registers.GetHL();
	uint8_t old = this->ReadMemory(offset);
	uint8_t value = (old << 1) | 1;

	this->WriteMemory(offset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x80) ? Flag_C : 0));

	this->d._registers.IncPC(2);
	this->d._cycles += 15;
}

// |4|23| The contents of the memory location pointed to by ix plus * are shifted left one bit position. The contents of bit 7 are put into the carry flag and a one is put into bit 0.
void CPUInst::SLLXXd(uint8_t reg) {
	uint16_t XX = this->d._registers.GetRegss(reg);
	uint16_t finalOffset = XX + ((int8_t) this->ReadMemory(this->d._registers.GetPC() + 2));
	uint8_t old = this->ReadMemory(finalOffset);

	uint8_t value = (old << 1) | 1;

	this->WriteMemory(finalOffset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x80) ? Flag_C : 0));

	this->d._registers.IncPC(4);
	this->d._cycles += 23;
}

// |2|8| The contents of b are shifted right one bit position. The contents of bit 0 are copied to the carry flag and a zero is put into bit 7.
void CPUInst::SRL(uint8_t reg) {
	uint8_t old = this->d._registers.GetRegm(reg);
	uint8_t value =  old >> 1;

	this->d._registers.SetRegm(reg, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x01) ? Flag_C : 0));

	this->d._registers.IncPC(2);
	this->d._cycles += 8;
}

// |2|15| The contents of (hl) are shifted right one bit position. The contents of bit 0 are copied to the carry flag and a zero is put into bit 7.
void CPUInst::SRL_HL() {
	uint16_t offset = this->d._registers.GetHL();
	uint8_t old = this->ReadMemory(offset);
	uint8_t value =  old >> 1;

	this->WriteMemory(offset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x01) ? Flag_C : 0));

	this->d._registers.IncPC(2);
	this->d._cycles += 15;
}

// |4|23| The contents of the memory location pointed to by ix plus * are shifted right one bit position. The contents of bit 0 are copied to the carry flag and a zero is put into bit 7.
void CPUInst::SRLXXd(uint8_t reg) {
	uint16_t XX = this->d._registers.GetRegss(reg);
	uint16_t finalOffset = XX + ((int8_t) this->ReadMemory(this->d._registers.GetPC() + 2));
	uint8_t old = this->ReadMemory(finalOffset);

	uint8_t value =  old >> 1;

	this->WriteMemory(finalOffset, value);

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		PZS_Flags[value] |
		((old & 0x01) ? Flag_C : 0));

	this->d._registers.IncPC(4);
	this->d._cycles += 23;
}

// |2|18| The contents of the low-order nibble of (hl) are copied to the high-order nibble of (hl). The previous contents are copied to the low-order nibble of a. The previous contents are copied to the low-order nibble of (hl).
void CPUInst::RLD() {
	uint16_t offset = this->d._registers.GetHL();
	uint8_t HL = this->ReadMemory(offset);
	uint8_t A = this->d._registers.GetA();
	uint8_t valueA = (A & 0xF0) | (HL >> 4);
	uint8_t valueHL = (HL << 4) | (A & 0x0F);

	this->d._registers.SetA(valueA);
	this->WriteMemory(offset, valueHL);

	this->d._registers.SetF(
		(valueA & (Flag_F3 | Flag_F5)) |
		PZS_Flags[valueA] |
		(this->d._registers.GetF() & Flag_C)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 18;
}

// |2|18| The contents of the low-order nibble of (hl) are copied to the low-order nibble of a. The previous contents are copied to the high-order nibble of (hl). The previous contents are copied to the low-order nibble of (hl).
void CPUInst::RRD() {
	uint16_t offset = this->d._registers.GetHL();
	uint8_t HL = this->ReadMemory(offset);
	uint8_t A = this->d._registers.GetA();
	uint8_t valueA = (A & 0xF0) | (HL & 0x0F);
	uint8_t valueHL = (A << 4) | (HL >> 4);

	this->d._registers.SetA(valueA);
	this->WriteMemory(offset, valueHL);

	this->d._registers.SetF(
		(valueA & (Flag_F3 | Flag_F5)) |
		PZS_Flags[valueA] |
		(this->d._registers.GetF() & Flag_C)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 18;
}

/******************************************************************************/
/*********************** Bit Set, Reset, and Test Group ***********************/
/******************************************************************************/

// |2|8| Tests bit compare of value.
void CPUInst::BIT(uint8_t valueb, uint8_t compare) {
	uint8_t value = valueb & compare;

	this->d._registers.SetF(
		(value & (Flag_F3 | Flag_F5)) |
		Flag_H |
		PZS_Flags[value] |
		(this->d._registers.GetF() & Flag_C)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 8;
}

void CPUInst::BITHL(uint8_t compare) {
	uint16_t HL = this->d._registers.GetHL();
	uint8_t valueb = this->ReadMemory(HL);
	uint8_t value = valueb & compare;

	this->d._registers.SetF(
		((HL & Flag_F3H) ? Flag_F3 : 0) |
		((HL & Flag_F5H) ? Flag_F5 : 0) |
		Flag_H |
		PZS_Flags[value] |
		(this->d._registers.GetF() & Flag_C)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 12;
}

// |4|20| Tests bit 'bit' of the memory location pointed to by ss plus *.
void CPUInst::BITbssd(uint8_t bit, uint8_t reg, uint8_t d) {
	uint16_t offset = this->d._registers.GetRegss(reg) + ((int8_t) d);
	uint8_t value = this->ReadMemory(offset) & bit;

	this->d._registers.SetF(
		((offset & Flag_F3H) ? Flag_F3 : 0) |
		((offset & Flag_F5H) ? Flag_F5 : 0) |
		Flag_H |
		PZS_Flags[value] |
		(this->d._registers.GetF() & Flag_C)
	);

	this->d._registers.IncPC(4);
	this->d._cycles += 20;
}

// |2|8| Sets bit X of reg.
void CPUInst::SET(uint8_t reg, uint8_t bit) {
	this->d._registers.SetRegm(reg, this->d._registers.GetRegm(reg) | bit);
	this->d._registers.IncPC(2);
	this->d._cycles += 8;
}

// |2|15| Sets bit X of (HL).
void CPUInst::SETHL(uint8_t bit) {
	uint16_t offset = this->d._registers.GetHL();
	this->WriteMemory(offset, this->ReadMemory(offset) | bit);
	this->d._registers.IncPC(2);
	this->d._cycles += 15;
}

// |4|23| Sets bit 'bit' of the memory location pointed to by ss plus *.
void CPUInst::SETbssd(uint8_t bit, uint8_t reg, uint8_t d) {
	uint16_t offset = this->d._registers.GetRegss(reg) + ((int8_t) d);
	this->WriteMemory(offset, this->ReadMemory(offset) | bit);
	this->d._registers.IncPC(4);
	this->d._cycles += 23;
}

// |2|8| Resets bit X of reg.
void CPUInst::RES(uint8_t reg, uint8_t bit) {
	this->d._registers.SetRegm(reg, this->d._registers.GetRegm(reg) & ~bit);
	this->d._registers.IncPC(2);
	this->d._cycles += 8;
}

// |2|15| Resets bit X of (HL).
void CPUInst::RESHL(uint8_t bit) {
	uint16_t offset = this->d._registers.GetHL();
	this->WriteMemory(offset, this->ReadMemory(offset) & ~bit);
	this->d._registers.IncPC(2);
	this->d._cycles += 15;
}

// |4|23| Resets bit 'bit' of the memory location pointed to by ss plus *.
void CPUInst::RESbssd(uint8_t bit, uint8_t reg, uint8_t d) {
	uint16_t offset = this->d._registers.GetRegss(reg) + ((int8_t) d);
	this->WriteMemory(offset, this->ReadMemory(offset) & ~bit);
	this->d._registers.IncPC(4);
	this->d._cycles += 23;
}

/******************************************************************************/
/********************************* Jump Group *********************************/
/******************************************************************************/

// |3|10| If condition cc is true, ** is copied to pc.
void CPUInst::JPccnn(bool cc) {
	if (cc) {
		uint16_t pc = this->d._registers.GetPC();
		this->d._registers.SetPC((this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1));
	} else
		this->d._registers.IncPC(3);

	this->d._cycles += 10;
}

// |2|12/7| If condition cc is true, the signed value * is added to pc.
// The jump is measured from the start of the instruction opcode.
void CPUInst::JR(bool cc) {
	int8_t value = this->ReadMemory(this->d._registers.GetPC() + 1);
	if (cc) {
		this->d._registers.IncPC(value + 2);
		this->d._cycles += 12;
	} else {
		this->d._registers.IncPC(2);
		this->d._cycles += 7;
	}
}

/******************************************************************************/
/*************************** Call And Return Group ****************************/
/******************************************************************************/

// |1|11/5| If condition cc is true, the top stack entry is popped into pc.
void CPUInst::RET(bool cc, uint8_t cycles) {
	if (cc) {
		uint16_t sp = this->d._registers.GetSP();
		uint16_t pc = (this->ReadMemory(sp + 1) << 8) | this->ReadMemory(sp);
		this->d._registers.SetSP(sp + 2);
		this->d._registers.SetPC(pc);
		this->d._cycles += cycles;
	} else {
		this->d._registers.IncPC();
		this->d._cycles += 5;
	}
}

// |1|11| The current pc value plus one is pushed onto the stack, then is loaded with ph.
void CPUInst::RSTp(uint8_t p) {
	Word pc;
	pc.W = this->d._registers.GetPC() + 1;
	uint16_t sp = this->d._registers.GetSP() - 2;
	this->d._registers.SetSP(sp);
	this->WriteMemory(sp + 1, pc.H);
	this->WriteMemory(sp, pc.L);

	this->d._cycles += 11;
	this->d._registers.SetPC(p);
}

// |3|17| The current pc value plus three is pushed onto the stack, then is loaded with **.
void CPUInst::CALLnn() {
	Word pc;
	pc.W = this->d._registers.GetPC() + 3;
	uint16_t sp = this->d._registers.GetSP() - 2;
	this->d._registers.SetSP(sp);
	this->WriteMemory(sp, pc.L);
	this->WriteMemory(sp + 1, pc.H);
	this->d._cycles += 17;
	this->d._registers.SetPC((this->ReadMemory(pc.W - 1) << 8) | this->ReadMemory(pc.W - 2));
}

// |3|17/10|If condition cc is true, the current pc value plus three is pushed onto the stack, then is loaded with **.
void CPUInst::CALLccnn(bool cc) {
	if (cc)
		CALLnn();
	else {
		this->d._registers.IncPC(3);
		this->d._cycles += 10;
	}
}

void CPUInst::CallInterrupt(uint16_t offset) {
	if (this->d._isSuspended) {
		this->d._registers.IncPC();
		this->d._isSuspended = false;
	}

	Word pc;
	pc.W = this->d._registers.GetPC();
	uint16_t sp = this->d._registers.GetSP() - 2;
	this->d._registers.SetSP(sp);
	this->WriteMemory(sp, pc.L);
	this->WriteMemory(sp + 1, pc.H);
	this->d._cycles += 11;
	this->d._registers.SetPC(offset);
}

void CPUInst::RETI() {
	uint16_t sp = this->d._registers.GetSP();
	uint16_t pc = (this->ReadMemory(sp + 1) << 8) | this->ReadMemory(sp);
	this->d._registers.SetSP(sp + 2);
	this->d._registers.SetPC(pc);
	this->d._cycles += 14;
	this->d._registers.SetIFF1(this->d._registers.GetIFF2());
}

void CPUInst::RETN() {
	uint16_t sp = this->d._registers.GetSP();
	uint16_t pc = (this->ReadMemory(sp + 1) << 8) | this->ReadMemory(sp);
	this->d._registers.SetSP(sp + 2);
	this->d._registers.SetPC(pc);
	this->d._cycles += 14;
	this->d._registers.SetIFF1(this->d._registers.GetIFF2());
}

/******************************************************************************/
/*************************** Input and Output Group ***************************/
/******************************************************************************/

// |2|12| A byte from port c is written to reg.
void CPUInst::INrC(uint8_t reg) {
	uint8_t C = this->d._registers.GetC();

	if (reg != Reg_UNDEFINED) {
		this->d._addressBus.L = C;
		this->d._addressBus.H = this->d._registers.GetB();
	}

	uint8_t data = this->ReadPort(C);

	if (reg != Reg_UNDEFINED)
		this->d._registers.SetRegm(reg, data);

	this->d._registers.SetF(PZS_Flags[data] |
		(this->d._registers.GetF() & Flag_C)
	);

	this->d._registers.IncPC(2);
	this->d._cycles += 12;
}

// |2|16| A byte from port c is written to the memory location pointed to by hl. Then hl is incremented and b is decremented.
void CPUInst::INI() {
	uint16_t HL = this->d._registers.GetHL();
	uint8_t B = this->d._registers.GetB();
	uint8_t C = this->d._registers.GetC();

	this->d._addressBus.L = C;
	this->d._addressBus.H = B;
	this->WriteMemory(HL, this->ReadPort(C));
	this->d._addressBus.W = HL;
	B = B - 1;

	this->d._registers.SetHL(HL + 1);
	this->d._registers.SetB(B);
	this->d._registers.SetFFlag(Flag_Z, B == 0);
	this->d._registers.SetFFlag(Flag_N, true);

	this->d._registers.IncPC(2);
	this->d._cycles += 16;
}

// |2|11| The value of a is written to port *.
void CPUInst::OUTnA() {
	uint8_t n = this->ReadMemory(this->d._registers.GetPC() + 1);
	uint8_t data = this->d._registers.GetA();

	this->d._addressBus.L = n;
	this->d._addressBus.H = data;
	this->WritePort(n, data);

	this->d._registers.IncPC(2);
	this->d._cycles += 11;
}

// |2|12| The value of reg is written to port c.
void CPUInst::OUTC(uint8_t value) {
	uint8_t C = this->d._registers.GetC();
	uint8_t B = this->d._registers.GetB();

	this->d._addressBus.L = C;
	this->d._addressBus.H = B;
	this->WritePort(C, value);

	this->d._registers.IncPC(2);
	this->d._cycles += 12;
}

// |2|16| A byte from the memory location pointed to by hl is written to port c. Then hl is incremented and b is decremented.
void CPUInst::OUTI() {
	uint8_t C = this->d._registers.GetC();
	uint8_t B = this->d._registers.GetB() - 1;
	uint16_t HL = this->d._registers.GetHL();
	uint8_t value = this->ReadMemory(HL);

	this->WritePort(C, value);
	this->d._addressBus.L = C;
	this->d._addressBus.H = B;

	this->d._registers.SetHL(HL + 1);
	this->d._registers.SetB(B);
	this->d._registers.SetFFlag(Flag_Z, B == 0);
	this->d._registers.SetFFlag(Flag_N, true);

	this->d._registers.IncPC(2);
	this->d._cycles += 16;
}

// |2|16| A byte from the memory location pointed to by hl is written to port c. Then hl and b are decremented.
void CPUInst::OUTD() {
	uint8_t B = this->d._registers.GetB() - 1;
	uint8_t C = this->d._registers.GetC();
	uint16_t HL = this->d._registers.GetHL();

	this->WritePort(C, this->ReadMemory(HL));
	this->d._addressBus.L = C;
	this->d._addressBus.H = B;

	this->d._registers.SetHL(HL - 1);
	this->d._registers.SetB(B);
	this->d._registers.SetFFlag(Flag_Z, B == 0);
	this->d._registers.SetFFlag(Flag_N, true);

	this->d._registers.IncPC(2);
	this->d._cycles += 16;
}

int CPUInst::GetSaveSize() {
	return sizeof(CPUInst::saveData);
}

void CPUInst::LoadState(uint8_t * data) {
	memcpy (&this->d, data, sizeof(CPUInst::saveData));
}

void CPUInst::SaveState(uint8_t * data) {
	memcpy (data, &this->d, sizeof(CPUInst::saveData));
}

void CPUInst::SetWriteMemoryCB(void (* fun)(uint16_t, uint8_t, void *), void * data) {
	this->_writeMemoryCB = fun;
	this->_writeMemoryDataCB = data;
}

void CPUInst::SetReadMemoryCB(uint8_t (* fun)(uint16_t, void *), void * data) {
	this->_readMemoryCB = fun;
	this->_readMemoryDataCB = data;
}

void CPUInst::SetWritePortCB(void (* fun)(uint8_t, uint8_t, void *), void * data) {
	this->_writePortCB = fun;
	this->_writePortDataCB = data;
}

void CPUInst::SetReadPortCB(uint8_t (* fun)(uint8_t, void *), void * data) {
	this->_readPortCB = fun;
	this->_readPortDataCB = data;
}
