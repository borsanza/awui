/**
 * awui/Emulation/Processors/Z80/CPUInst.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "CPUInst.h"

#include <stdio.h>
#include <string.h>

using namespace awui::Emulation::Processors::Z80;

uint8_t ZS_Flags[256];
uint8_t PZS_Flags[256];

CPUInst::CPUInst() {
	m_showLog = false;
	m_readPortCB = NULL;
	m_readPortDataCB = NULL;
	m_readMemoryCB = NULL;
	m_readMemoryDataCB = NULL;
	m_writePortCB = NULL;
	m_writePortDataCB = NULL;
	m_writeMemoryCB = NULL;
	m_writeMemoryDataCB = NULL;
	m_showNotImplemented = false;
	FillFlags();
	Reset();
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
	m_saveData.cycles = 0;
	m_saveData.inInterrupt = false;
	m_saveData.isSuspended = false;
	m_saveData.afterEI = false;
	m_saveData.registers.Clear();
}

void CPUInst::WriteMemory(uint16_t pos, uint8_t value) {
	m_writeMemoryCB(pos, value, m_writeMemoryDataCB);
	m_saveData.cycles += 3;
}

uint8_t CPUInst::ReadMemory(uint16_t pos) {
	uint8_t data = m_readMemoryCB(pos, m_readMemoryDataCB);
	m_saveData.cycles += 3;

	return data;
}

void CPUInst::WritePort(uint8_t port, uint8_t value) const {
	m_writePortCB(port, value, m_writePortDataCB);
}

uint8_t CPUInst::ReadPort(uint8_t port) const {
	return m_readPortCB(port, m_readPortDataCB);
}

/******************************************************************************/
/****************************** 8-Bit Load Group ******************************/
/******************************************************************************/

/**
 * LD r, r
 * |1|4| The contents of B/C/D/E/H/L/A are loaded into B/C/D/E/H/L/A.
 * |2|9| Stores the value of A into register I/R.
 * |2|8| The contents of B/C/D/E/H/IXH/IXL/IYH/IYL/A are loaded into B/C/D/E/H/IXH/IXL/IYH/IYL/A.
 */
void CPUInst::LDrr(uint8_t reg, uint8_t value, uint8_t cycles, uint8_t size) {
	m_saveData.registers.SetRegm(reg, value);
	m_saveData.registers.IncPC(size);
	m_saveData.cycles += cycles;
}

/**
 * LD A, i
 * |2|9| Stores the value of register I/R into A
 */
void CPUInst::LDAri(uint8_t value) {
	m_saveData.registers.SetA(value);

	m_saveData.registers.SetF(ZS_Flags[value] | (m_saveData.registers.GetIFF2() ? Flag_P : 0) | (m_saveData.registers.GetF() & Flag_C));

	m_saveData.registers.IncPC(2);
	m_saveData.cycles += 9;
}

/**
 * LD r, * -> pc:4,ss:3
 * |2|7| Loads * into register B/C/D/E/H/L/A
 */
void CPUInst::LDrn(uint8_t reg) {
	m_saveData.cycles += 4;
	m_saveData.registers.IncPC(2);
	m_saveData.registers.SetRegm(reg, ReadMemory(m_saveData.registers.GetPC() - 1));
}

/**
 * LD iih, * -> pc:4,pc+1:4,pc+2:3,pc+2:1 x 5,ii+n:3
 * |3|11| Loads * into IXh/IYh.
 */
void CPUInst::LDriin(uint8_t reg) {
	m_saveData.cycles += 4;
	m_saveData.registers.IncPC(3);
	m_saveData.registers.SetRegm(reg, ReadMemory(m_saveData.registers.GetPC() - 1));
	m_saveData.cycles += 4;
}

/**
 * LD r, (HL) -> pc:4,ss:3
 * |1|7| The contents of (HL) are loaded into register B/C/D/E/H/L/A
 */
void CPUInst::LDrHL(uint8_t reg) {
	m_saveData.cycles += 4;
	m_saveData.registers.SetRegm(reg, ReadMemory(m_saveData.registers.GetHL()));
	m_saveData.registers.IncPC();
}

/**
 * LD r, (ii + *) -> pc:4,pc+1:4,pc+2:3,pc+2:1 x 5,ii+n:3
 * |3|19| Loads the value pointed to by IX/IY plus * into register B/C/D/E/H/L/A
 */
void CPUInst::LDrXXd(uint8_t reg, uint8_t reg2) {
	m_saveData.cycles += 4;
	m_saveData.registers.SetRegm(reg, ReadMemory(m_saveData.registers.GetRegss(reg2) + (int8_t) ReadMemory(m_saveData.registers.GetPC() + 2)));
	m_saveData.registers.IncPC(3);
	m_saveData.cycles += 9;
}

/**
 * LD (ss), r -> pc:4,ss:3
 * |1|7| The contents of register B/C/D/E/H/L/A are loaded into (BC/DE/HL).
 */
void CPUInst::LDssr(uint16_t offset, uint8_t value) {
	m_saveData.cycles += 4;
	WriteMemory(offset, value);
	m_saveData.registers.IncPC();
}

/**
 * LD (ii + *), r -> pc:4,pc+1:4,pc+2:3,pc+2:1 x 5,ii+n:3
 * |3|19| Stores register B/C/D/E/H/L/A to the memory location pointed to by IX/IY plus *.
 */
void CPUInst::LDXXdr(uint8_t xx, uint8_t reg) {
	m_saveData.cycles += 4;
	uint16_t x = m_saveData.registers.GetRegss(xx);
	uint16_t offset = x + ((int8_t) ReadMemory(m_saveData.registers.GetPC() + 2));
	WriteMemory(offset, m_saveData.registers.GetRegm(reg));
	m_saveData.registers.IncPC(3);
	m_saveData.cycles += 9;
}

/**
 * LD (ii + *), *
 * |4|19| Stores * to the memory location pointed to by IX/IY plus *.
 */
void CPUInst::LDXXdn(uint8_t xx) {
	m_saveData.cycles += 4;
	uint16_t pc = m_saveData.registers.GetPC();
	uint8_t n = ReadMemory(pc + 3);
	uint16_t offset = m_saveData.registers.GetRegss(xx) + ((int8_t) ReadMemory(pc + 2));
	WriteMemory(offset, n);
	m_saveData.registers.IncPC(4);
	m_saveData.cycles += 6;
}

/******************************************************************************/
/***************************** 16-Bit Load Group ******************************/
/******************************************************************************/

/**
 * LD reg, **
 * |3|10| Loads ** into register BC/DE/HL/SP.
 */
void CPUInst::LDddnn(uint8_t reg) {
	m_saveData.cycles += 4;
	m_saveData.registers.IncPC(3);
	uint16_t pc = m_saveData.registers.GetPC();
	m_saveData.registers.SetRegss(reg, (ReadMemory(pc - 1) << 8) | ReadMemory(pc - 2));
}

/**
 * LD reg, **
 * |4|14| Loads ** into register IX/IY.
 */
void CPUInst::LDddnnX(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(4);
	uint16_t pc = m_saveData.registers.GetPC();
	m_saveData.registers.SetRegss(reg, (ReadMemory(pc - 1) << 8) | ReadMemory(pc - 2));
}

/**
 * LD dd, (**) -> pc:4,pc+1:4,pc+2:3,pc+3:3,nn:3,nn+1:3
 * |4|20| Loads the value pointed to by ** into register BC/DE/HL/SP/IX/IY
 */
void CPUInst::LDdd_nn(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(4);
	uint16_t pc = m_saveData.registers.GetPC();
	Word offset;
	offset.L = ReadMemory(pc - 2);
	offset.H = ReadMemory(pc - 1);
	Word data;
	data.L = ReadMemory(offset.W);
	data.H = ReadMemory(offset.W + 1);
	m_saveData.registers.SetRegss(reg, data.W);
}

/**
 * LD HL, (**) -> pc:4,pc+1:3,pc+2:3,nn:3,nn+1:3
 * |3|16| Loads the value pointed to by ** into HL
 */
void CPUInst::LDHL_nn() {
	m_saveData.cycles += 4;
	m_saveData.registers.IncPC(3);
	uint16_t pc = m_saveData.registers.GetPC();
	Word offset;
	offset.L = ReadMemory(pc - 2);
	offset.H = ReadMemory(pc - 1);
	Word data;
	data.L = ReadMemory(offset.W);
	data.H = ReadMemory(offset.W + 1);
	m_saveData.registers.SetHL(data.W);
}

/**
 * LD (**), dd -> pc:4,pc+1:4,pc+2:3,pc+3:3,nn:3,nn+1:3
 * |4|20| Stores register BC/DE/HL/SP/IX/IY into the memory location pointed to by **
 */
void CPUInst::LDnndd(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(4);
	uint16_t pc = m_saveData.registers.GetPC();
	Word value;
	value.W = m_saveData.registers.GetRegss(reg);
	uint16_t offset = (ReadMemory(pc - 1) << 8) | ReadMemory(pc - 2);
	WriteMemory(offset, value.L);
	WriteMemory(offset + 1, value.H);
}

/**
 * LD (**), hl -> pc:4,pc+1:3,pc+2:3,nn:3,nn+1:3
 * |3|16| Stores HL into the memory location pointed to by **
 */
void CPUInst::LDHLdd() {
	m_saveData.cycles += 4;
	m_saveData.registers.IncPC(3);
	uint16_t pc = m_saveData.registers.GetPC();
	Word value;
	value.W = m_saveData.registers.GetRegss(Reg_HL);
	uint16_t offset = (ReadMemory(pc - 1) << 8) | ReadMemory(pc - 2);
	WriteMemory(offset, value.L);
	WriteMemory(offset + 1, value.H);
}

/**
 * PUSH ss -> pc:4,ir:1,sp-1:3,sp-2:3
 * |1|11| sp is decremented and b is stored into the memory location pointed to by sp. sp is decremented again and c is stored into the memory location pointed to by sp.
 */
void CPUInst::PUSH16(uint8_t reg) {
	m_saveData.cycles += 5;
	m_saveData.registers.IncPC();
	Word value;
	value.W = m_saveData.registers.GetRegss(reg);
	uint16_t sp = m_saveData.registers.GetSP();
	WriteMemory(sp - 1, value.H);
	WriteMemory(sp - 2, value.L);
	m_saveData.registers.SetSP(sp - 2);
}

/**
 * PUSH ii
 * |2|15| sp is decremented and ixh is stored into the memory location pointed to by sp. sp is decremented again and ixl is stored into the memory location pointed to by sp.
 */
void CPUInst::PUSH16X(uint8_t reg) {
	m_saveData.cycles += 9;
	m_saveData.registers.IncPC(2);
	Word value;
	value.W = m_saveData.registers.GetRegss(reg);
	uint16_t sp = m_saveData.registers.GetSP();
	WriteMemory(sp - 1, value.H);
	WriteMemory(sp - 2, value.L);
	m_saveData.registers.SetSP(sp - 2);
}

/**
 * POP ss -> pc:4,sp:3,sp+1:3
 * |1|10| The memory location pointed to by sp is stored into c and sp is incremented. The memory location pointed to by sp is stored into b and sp is incremented again.
 */
void CPUInst::POP16(uint8_t reg) {
	m_saveData.cycles += 4;
	m_saveData.registers.IncPC();
	uint16_t sp = m_saveData.registers.GetSP();
	Word value;
	value.H = ReadMemory(sp + 1);
	value.L = ReadMemory(sp);
	m_saveData.registers.SetRegss(reg, value.W);
	m_saveData.registers.SetSP(sp + 2);
}

/**
 * POP ii
 * |2|14| The memory location pointed to by sp is stored into ixl and sp is incremented. The memory location pointed to by sp is stored into ixh and sp is incremented again.
 */
void CPUInst::POP16X(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);
	uint16_t sp = m_saveData.registers.GetSP();
	Word value;
	value.H = ReadMemory(sp + 1);
	value.L = ReadMemory(sp);
	m_saveData.registers.SetRegss(reg, value.W);
	m_saveData.registers.SetSP(sp + 2);
}

/**
 * |1|6|  Loads the value of HL into SP.
 * |1|4|  Loads the value of HL into PC.
 * |2|8|  Loads the value of IX/IY into PC.
 * |2|10| Loads the value of IX/IY into SP.
 */
void CPUInst::LDtofrom(uint8_t to, uint16_t value, uint8_t cycles, uint8_t size) {
	m_saveData.cycles += cycles;
	m_saveData.registers.IncPC(size);
	m_saveData.registers.SetRegss(to, value);
}

/******************************************************************************/
/***************** Exchange, Block Transfer, and Search Group *****************/
/******************************************************************************/

/**
 * EX (SP), HL -> pc:4,sp:3,sp+1:3,sp+1:1,sp+1(write):3,sp(write):3,sp(write):1 x 2
 * |1|19| Exchanges (SP) with L, and (ss1+1) with H.
 */
void CPUInst::EX_SPHL() {
	m_saveData.cycles += 4;
	m_saveData.registers.IncPC();

	uint16_t ss = m_saveData.registers.GetSP();
	Word aux;
	aux.H = ReadMemory(ss + 1);
	aux.L = ReadMemory(ss);

	m_saveData.cycles += 1;

	Word value2;
	value2.W = m_saveData.registers.GetHL();

	WriteMemory(ss + 1, value2.H);
	WriteMemory(ss, value2.L);
	m_saveData.registers.SetHL(aux.W);

	m_saveData.cycles += 2;
}

/**
 * EX (SP), reg -> pc:4,sp:3,sp+1:3,sp+1:1,sp+1(write):3,sp(write):3,sp(write):1 x 2
 * |2|23| Exchanges (SP) with the IXl/IYl, and (SP+1) with the IXh/IYh.
 */
void CPUInst::EX_ssX(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint16_t ss = m_saveData.registers.GetSP();
	Word aux;
	aux.H = ReadMemory(ss + 1);
	aux.L = ReadMemory(ss);

	m_saveData.cycles += 1;

	Word value2;
	value2.W = m_saveData.registers.GetRegss(reg);

	WriteMemory(ss + 1, value2.H);
	WriteMemory(ss, value2.L);
	m_saveData.registers.SetRegss(reg, aux.W);

	m_saveData.cycles += 2;
}

/**
 * LDI -> pc:4,pc+1:4,hl:3,de:3,de:1 x 2,[de:1 x 5]
 * |2|16| Transfers a byte of data from the memory location pointed to by hl to the memory location pointed to by de. Then hl and de are incremented and bc is decremented.
 */
void CPUInst::LDI() {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint8_t dataHL = ReadMemory(m_saveData.registers.GetHL());
	uint8_t valueFlag = dataHL + m_saveData.registers.GetA();

	WriteMemory(m_saveData.registers.GetDE(), dataHL);

	m_saveData.registers.SetHL(m_saveData.registers.GetHL() + 1);
	m_saveData.registers.SetDE(m_saveData.registers.GetDE() + 1);
	uint16_t value = m_saveData.registers.GetBC() - 1;
	m_saveData.registers.SetBC(value);

	m_saveData.registers.SetF(((valueFlag & 2) ? Flag_F5 : 0) | ((valueFlag & 8) ? Flag_F3 : 0) | ((value != 0) ? Flag_V : 0) |
							(m_saveData.registers.GetF() & (Flag_S | Flag_Z | Flag_C)));

	m_saveData.cycles += 2;
}

/**
 * LDIR -> pc:4,pc+1:4,hl:3,de:3,de:1 x 2,[de:1 x 5]
 * |2|21/16| Transfers a byte of data from the memory location pointed to by hl to the memory location pointed to by de. Then hl and de are incremented and bc is decremented. If bc
 * is not zero, this operation is repeated. Interrupts can trigger while this instruction is processing.
 */
void CPUInst::LDIR() {
	m_saveData.cycles += 8;

	uint16_t hl = m_saveData.registers.GetHL();
	uint16_t de = m_saveData.registers.GetDE();
	uint16_t bc = m_saveData.registers.GetBC() - 1;
	uint8_t value = ReadMemory(hl);
	uint8_t valueFlag = value + m_saveData.registers.GetA();

	WriteMemory(de, value);
	m_saveData.registers.SetHL(hl + 1);
	m_saveData.registers.SetDE(de + 1);
	m_saveData.registers.SetBC(bc);

	m_saveData.registers.SetF(((valueFlag & 2) ? Flag_F5 : 0) | ((valueFlag & 8) ? Flag_F3 : 0) | (m_saveData.registers.GetF() & (Flag_S | Flag_Z | Flag_C)));

	// TODO: Repasar el orden de los ciclos, no estoy seguro
	if (bc == 0) {
		m_saveData.registers.IncPC(2);
		m_saveData.cycles += 2;
	} else
		m_saveData.cycles += 7;
}

/**
 * LDD -> pc:4,pc+1:4,hl:3,de:3,de:1 x 2
 * |2|16| Transfers a byte of data from the memory location pointed to by hl to the memory location pointed to by de. Then hl, de, and bc are decremented.
 */
void CPUInst::LDD() {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint16_t HL = m_saveData.registers.GetHL();
	uint16_t DE = m_saveData.registers.GetDE();
	uint16_t BC = m_saveData.registers.GetBC() - 1;
	uint8_t value = ReadMemory(HL);
	uint8_t valueFlag = value + m_saveData.registers.GetA();

	WriteMemory(DE, value);

	m_saveData.registers.SetHL(HL - 1);
	m_saveData.registers.SetDE(DE - 1);
	m_saveData.registers.SetBC(BC);

	m_saveData.registers.SetF(((valueFlag & 2) ? Flag_F5 : 0) | ((valueFlag & 8) ? Flag_F3 : 0) | ((BC != 0) ? Flag_P : 0) |
							(m_saveData.registers.GetF() & (Flag_S | Flag_Z | Flag_C)));

	m_saveData.cycles += 2;
}

/**
 * LDDR -> pc:4,pc+1:4,hl:3,de:3,de:1 x 2,[de:1 x 5]
 * |2|21/16| Transfers a byte of data from the memory location pointed to by hl to the memory location pointed to by de. Then hl, de, and bc are decremented. If bc is not zero,
 * this operation is repeated. Interrupts can trigger while this instruction is processing.
 */
void CPUInst::LDDR() {
	m_saveData.cycles += 8;
	uint16_t HL = m_saveData.registers.GetHL();
	uint16_t DE = m_saveData.registers.GetDE();
	uint16_t BC = m_saveData.registers.GetBC() - 1;
	uint8_t value = ReadMemory(HL);
	uint8_t valueFlag = value + m_saveData.registers.GetA();

	WriteMemory(DE, value);

	m_saveData.registers.SetHL(HL - 1);
	m_saveData.registers.SetDE(DE - 1);
	m_saveData.registers.SetBC(BC);

	m_saveData.registers.SetF(((valueFlag & 2) ? Flag_F5 : 0) | ((valueFlag & 8) ? Flag_F3 : 0) | (m_saveData.registers.GetF() & (Flag_S | Flag_Z | Flag_C)));

	if (BC == 0) {
		m_saveData.registers.IncPC(2);
		m_saveData.cycles += 2;
	} else {
		m_saveData.cycles += 7;
	}
}

/**
 * CPI -> pc:4,pc+1:4,hl:3,hl:1 x 5
 * |2|16| Compares the value of the memory location pointed to by hl with a. Then hl is incremented and bc is decremented.
 */
void CPUInst::CPI() {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint16_t HL = m_saveData.registers.GetHL();
	uint8_t b = ReadMemory(HL);
	uint8_t old = m_saveData.registers.GetA();
	uint8_t value = old - b;
	uint16_t BC = m_saveData.registers.GetBC() - 1;

	m_saveData.registers.SetHL(HL + 1);
	m_saveData.registers.SetBC(BC);

	int newH = (value & 0xF) > (old & 0xF);
	m_saveData.registers.SetF(ZS_Flags[value] | (newH ? Flag_H : 0) | (((value - newH) & 2) ? Flag_F5 : 0) | (((value - newH) & 8) ? Flag_F3 : 0) | ((BC != 0) ? Flag_V : 0) |
							Flag_N | (m_saveData.registers.GetF() & Flag_C));

	m_saveData.cycles += 5;
}

/**
 * CPIR -> pc:4,pc+1:4,hl:3,hl:1 x 5,[hl:1 x 5]
 * |2|21/16| Compares the value of the memory location pointed to by hl with a. Then hl is incremented and bc is decremented. If bc is not zero and z is not set, this operation is
 * repeated. Interrupts can trigger while this instruction is processing.
 */
void CPUInst::CPIR() {
	m_saveData.cycles += 8;

	uint16_t HL = m_saveData.registers.GetHL();
	uint8_t b = ReadMemory(HL);
	uint8_t old = m_saveData.registers.GetA();
	uint8_t value = old - b;
	uint16_t BC = m_saveData.registers.GetBC() - 1;

	m_saveData.registers.SetHL(HL + 1);
	m_saveData.registers.SetBC(BC);

	int newH = (value & 0xF) > (old & 0xF);
	m_saveData.registers.SetF(ZS_Flags[value] | (newH ? Flag_H : 0) | (((value - newH) & 2) ? Flag_F5 : 0) | (((value - newH) & 8) ? Flag_F3 : 0) | ((BC != 0) ? Flag_V : 0) |
							Flag_N | (m_saveData.registers.GetF() & Flag_C));

	if ((BC != 0) && (value != 0)) {
		m_saveData.cycles += 10;
	} else {
		m_saveData.registers.IncPC(2);
		m_saveData.cycles += 5;
	}
}

/**
 * CPD -> pc:4,pc+1:4,hl:3,hl:1 x 5
 * |2|16| Compares the value of the memory location pointed to by hl with a. Then hl and bc are decremented.
 */
void CPUInst::CPD() {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint16_t HL = m_saveData.registers.GetHL();
	uint8_t b = ReadMemory(HL);
	uint8_t old = m_saveData.registers.GetA();
	uint8_t value = old - b;
	uint16_t BC = m_saveData.registers.GetBC() - 1;

	m_saveData.registers.SetHL(HL - 1);
	m_saveData.registers.SetBC(BC);

	int newH = (value & 0xF) > (old & 0xF);
	m_saveData.registers.SetF(ZS_Flags[value] | (newH ? Flag_H : 0) | (((value - newH) & 2) ? Flag_F5 : 0) | (((value - newH) & 8) ? Flag_F3 : 0) | ((BC != 0) ? Flag_V : 0) |
							Flag_N | (m_saveData.registers.GetF() & Flag_C));

	m_saveData.cycles += 5;
}

/**
 * CPDR -> pc:4,pc+1:4,hl:3,hl:1 x 5,[hl:1 x 5]
 * |2|21/16| Compares the value of the memory location pointed to by hl with a. Then hl and bc are decremented. If bc is not zero and z is not set, this operation is repeated.
 * Interrupts can trigger while this instruction is processing.
 */
void CPUInst::CPDR() {
	m_saveData.cycles += 8;

	uint16_t HL = m_saveData.registers.GetHL();
	uint8_t b = ReadMemory(HL);
	uint8_t old = m_saveData.registers.GetA();
	uint8_t value = old - b;
	uint16_t BC = m_saveData.registers.GetBC() - 1;

	m_saveData.registers.SetHL(HL - 1);
	m_saveData.registers.SetBC(BC);

	int newH = (value & 0xF) > (old & 0xF);
	m_saveData.registers.SetF(ZS_Flags[value] | (newH ? Flag_H : 0) | (((value - newH) & 2) ? Flag_F5 : 0) | (((value - newH) & 8) ? Flag_F3 : 0) | ((BC != 0) ? Flag_V : 0) |
							Flag_N | (m_saveData.registers.GetF() & Flag_C));

	if ((BC != 0) && (value != 0)) {
		m_saveData.cycles += 10;
	} else {
		m_saveData.registers.IncPC(2);
		m_saveData.cycles += 5;
	}
}

/******************************************************************************/
/*************************** 8-Bit Arithmetic Group ***************************/
/******************************************************************************/

/**
 * |1|4| Adds valueb to a.
 */
void CPUInst::ADD(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t A = m_saveData.registers.GetA();
	Word w;
	w.W = ((uint16_t) A) + ((uint16_t) b);

	m_saveData.registers.SetF((w.L & (Flag_F3 | Flag_F5)) | ZS_Flags[w.L] | ((A ^ b ^ w.L) & Flag_H) | ((((~(A ^ b)) & (b ^ w.L)) & 0x80) ? Flag_V : 0) | w.H);

	m_saveData.registers.SetA(w.L);

	m_saveData.registers.IncPC(size);
	m_saveData.cycles += cycles;
}

/**
 * |1|4| Adds l and the carry flag to a.
 */
void CPUInst::ADC(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t A = m_saveData.registers.GetA();
	Word w;
	w.W = ((uint16_t) A) + ((uint16_t) b) + (m_saveData.registers.GetF() & Flag_C);

	m_saveData.registers.SetF((w.L & (Flag_F3 | Flag_F5)) | ZS_Flags[w.L] | ((A ^ b ^ w.L) & Flag_H) | ((((~(A ^ b)) & (b ^ w.L)) & 0x80) ? Flag_V : 0) | w.H);

	m_saveData.registers.SetA(w.L);

	m_saveData.registers.IncPC(size);
	m_saveData.cycles += cycles;
}

/**
 * |1|4| Subtracts reg from a.
 */
void CPUInst::SUB(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t A = m_saveData.registers.GetA();
	int16_t pvalue = ((int16_t) A) - ((int16_t) b);
	uint8_t value = pvalue;

	m_saveData.registers.SetA(value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | ZS_Flags[value] | (((A ^ b ^ value) & 0x10) ? Flag_H : 0) | (((A ^ b) & (A ^ value) & 0x80) ? Flag_V : 0) |
							((pvalue < 0) ? Flag_C : 0) | Flag_N);

	m_saveData.registers.IncPC(size);
	m_saveData.cycles += cycles;
}

/**
 * |1|4| Subtracts e and the carry flag from a.
 */
void CPUInst::SBC(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t A = m_saveData.registers.GetA();
	int16_t pvalue = ((int16_t) A) - ((int16_t) b);
	uint8_t value = pvalue;

	if (m_saveData.registers.GetF() & Flag_C) {
		value--;
		pvalue--;
	}

	m_saveData.registers.SetA(value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | ZS_Flags[value] | (((A ^ b ^ value) & 0x10) ? Flag_H : 0) | (((A ^ b) & (A ^ value) & 0x80) ? Flag_V : 0) |
							((pvalue < 0) ? Flag_C : 0) | Flag_N);

	m_saveData.registers.IncPC(size);
	m_saveData.cycles += cycles;
}

/**
 * |1|4|Bitwise AND on a with valueb.
 */
void CPUInst::AND(uint8_t valueb, uint8_t cycles, uint8_t size) {
	uint8_t value = m_saveData.registers.GetA() & valueb;

	m_saveData.registers.SetA(value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | Flag_H);

	m_saveData.registers.IncPC(size);
	m_saveData.cycles += cycles;
}

/**
 * |1|4| Bitwise OR on a with valueb
 */
void CPUInst::OR(uint8_t valueb, uint8_t cycles, uint8_t size) {
	uint8_t value = m_saveData.registers.GetA() | valueb;

	m_saveData.registers.SetA(value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value]);

	m_saveData.registers.IncPC(size);
	m_saveData.cycles += cycles;
}

/**
 * |1/2|4/7| Bitwise XOR on a with b.
 */
void CPUInst::XOR(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t value = m_saveData.registers.GetA() ^ b;

	m_saveData.registers.SetA(value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value]);

	m_saveData.registers.IncPC(size);
	m_saveData.cycles += cycles;
}

/**
 * |1|4| Subtracts b from a and affects flags according to the result. a is not modified.
 */
void CPUInst::CP(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t A = m_saveData.registers.GetA();
	uint8_t value = A - b;
	uint16_t pvalue = (uint16_t) A - (uint16_t) b;

	m_saveData.registers.SetF((b & (Flag_F3 | Flag_F5)) | ZS_Flags[value] | (((A ^ b ^ value) & 0x10) ? Flag_H : 0) | Flag_N | ((pvalue & 0x100) ? Flag_C : 0) |
							(((A ^ b) & (A ^ value) & 0x80) ? Flag_V : 0));

	m_saveData.registers.IncPC(size);
	m_saveData.cycles += cycles;
}

/**
 * |1|4| Adds one to reg
 */
void CPUInst::INCr(uint8_t reg, uint8_t cycles, uint8_t size) {
	uint8_t old = m_saveData.registers.GetRegm(reg);
	uint8_t value = old + 1;

	m_saveData.registers.SetRegm(reg, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | ZS_Flags[value] | ((value & 0xF) ? 0 : Flag_H) | ((old == 0x7F) ? Flag_V : 0) | (m_saveData.registers.GetF() & Flag_C));

	m_saveData.registers.IncPC(size);
	m_saveData.cycles += cycles;
}

/**
 * |1|4| Subtracts one from m
 */
void CPUInst::DECm(uint8_t reg, uint8_t cycles, uint8_t size) {
	uint8_t old = m_saveData.registers.GetRegm(reg);
	uint8_t value = old - 1;

	m_saveData.registers.SetRegm(reg, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | ZS_Flags[value] | (((value & 0xF) == 0xF) ? Flag_H : 0) | ((old == 0x80) ? Flag_V : 0) | Flag_N |
							(m_saveData.registers.GetF() & Flag_C));

	m_saveData.registers.IncPC(size);
	m_saveData.cycles += cycles;
}

/**
 * DEC (HL) -> pc:4,hl:3,hl:1,hl(write):3
 * |1|11| Subtracts one from (hl).
 */
void CPUInst::DECHL() {
	m_saveData.cycles += 4;
	m_saveData.registers.IncPC();

	uint16_t HL = m_saveData.registers.GetHL();
	uint8_t old = ReadMemory(HL);
	uint8_t value = old - 1;

	m_saveData.cycles++;

	WriteMemory(HL, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | ZS_Flags[value] | (((value & 0xF) == 0xF) ? Flag_H : 0) | ((old == 0x80) ? Flag_V : 0) | Flag_N |
							(m_saveData.registers.GetF() & Flag_C));
}

/**
 * INC (HL) -> pc:4,hl:3,hl:1,hl(write):3
 * |1|11| Adds one to (hl).
 */
void CPUInst::INCHL() {
	m_saveData.cycles += 4;
	m_saveData.registers.IncPC();

	uint8_t old = ReadMemory(m_saveData.registers.GetHL());
	uint8_t value = old + 1;

	m_saveData.cycles++;

	WriteMemory(m_saveData.registers.GetHL(), value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | ZS_Flags[value] | ((value & 0xF) ? 0 : Flag_H) | ((old == 0x7F) ? Flag_V : 0) | (m_saveData.registers.GetF() & Flag_C));
}

/**
 * INC (ii + n) -> c:4,pc+1:4,pc+2:3,pc+2:1 x 5,ii+n:3,ii+n:1,ii+n(write):3
 * |3|23| Adds one to the memory location pointed to by IX/IY plus *.
 */
void CPUInst::INCXXd(uint8_t xx) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(3);

	uint16_t pc = m_saveData.registers.GetPC();
	uint16_t offset = m_saveData.registers.GetRegss(xx) + ((int8_t) ReadMemory(pc - 1));

	m_saveData.cycles += 5;

	uint8_t old = ReadMemory(offset);
	uint8_t value = old + 1;

	m_saveData.cycles++;

	WriteMemory(offset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | ZS_Flags[value] | ((value & 0xF) ? 0 : Flag_H) | ((old == 0x7F) ? Flag_V : 0) | (m_saveData.registers.GetF() & Flag_C));
}

/**
 * DEC (ii + n) -> c:4,pc+1:4,pc+2:3,pc+2:1 x 5,ii+n:3,ii+n:1,ii+n(write):3
 * |3|23| Subtracts one from the memory location pointed to by ix plus *.
 */
void CPUInst::DECXXd(uint8_t xx) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(3);

	uint16_t pc = m_saveData.registers.GetPC();
	uint16_t offset = m_saveData.registers.GetRegss(xx) + ((int8_t) ReadMemory(pc - 1));

	m_saveData.cycles += 5;

	uint8_t old = ReadMemory(offset);
	uint8_t value = old - 1;

	m_saveData.cycles++;

	WriteMemory(offset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | ZS_Flags[value] | (((value & 0xF) == 0xF) ? Flag_H : 0) | ((old == 0x80) ? Flag_V : 0) | Flag_N |
							(m_saveData.registers.GetF() & Flag_C));
}

/******************************************************************************/
/************** General-Purpose Arithmetic and CPU Control Group **************/
/******************************************************************************/

/**
 * |1|4| Adjusts a for BCD addition and subtraction operations.
 */
void CPUInst::DAA() {
	uint8_t A = m_saveData.registers.GetA();
	uint8_t tmp = A;
	bool N = m_saveData.registers.GetF() & Flag_N;
	bool H = m_saveData.registers.GetF() & Flag_H;
	bool C = m_saveData.registers.GetF() & Flag_C;

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

	m_saveData.registers.SetA(tmp);

	m_saveData.registers.SetF((tmp & (Flag_F3 | Flag_F5)) | (((A ^ tmp) & 0x10) ? Flag_H : 0) | PZS_Flags[tmp] | ((C || A > 0x99) ? Flag_C : 0) |
							(m_saveData.registers.GetF() & Flag_N));

	m_saveData.registers.IncPC();
	m_saveData.cycles += 4;
}

/**
 * |1|4| The contents of a are inverted (one's complement).
 */
void CPUInst::CPL() {
	uint8_t A = ~m_saveData.registers.GetA();
	m_saveData.registers.SetA(A);

	m_saveData.registers.SetF((A & (Flag_F3 | Flag_F5)) | Flag_N | Flag_H | (m_saveData.registers.GetF() & (Flag_C | Flag_P | Flag_Z | Flag_S)));

	m_saveData.registers.IncPC();
	m_saveData.cycles += 4;
}

/**
 * |2|8| The contents of a are negated (two's complement). Operation is the same as subtracting a from zero.
 */
void CPUInst::NEG() {
	uint8_t A = m_saveData.registers.GetA();
	uint8_t value = 0 - A;
	m_saveData.registers.SetA(value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | ZS_Flags[value] | (((A ^ value) & 0x10) ? Flag_H : 0) | ((A == 0x80) ? Flag_V : 0) | ((A != 0) ? Flag_C : 0) | Flag_N);

	m_saveData.registers.IncPC(2);
	m_saveData.cycles += 8;
}

/**
 * |1|4| Inverts the carry flag.
 */
void CPUInst::CCF() {
	bool carry = m_saveData.registers.GetF() & Flag_C;
	uint8_t A = m_saveData.registers.GetA();

	m_saveData.registers.SetF((A & (Flag_F3 | Flag_F5)) | (carry ? Flag_H : Flag_C) | (m_saveData.registers.GetF() & (Flag_P | Flag_Z | Flag_S)));

	m_saveData.registers.IncPC();
	m_saveData.cycles += 4;
}

/**
 * |1|4| Sets the carry flag.
 */
void CPUInst::SCF() {
	uint8_t A = m_saveData.registers.GetA();

	m_saveData.registers.SetF((A & (Flag_F3 | Flag_F5)) | Flag_C | (m_saveData.registers.GetF() & (Flag_P | Flag_Z | Flag_S)));

	m_saveData.registers.IncPC();
	m_saveData.cycles += 4;
}

/******************************************************************************/
/*************************** 16-Bit Arithmetic Group **************************/
/******************************************************************************/

/**
 * |2|15| Adds ss and the carry flag to hl.
 */
void CPUInst::ADCHLss(uint8_t reg) {
	uint16_t hl = m_saveData.registers.GetHL();
	uint16_t b = m_saveData.registers.GetRegss(reg);
	Word w;
	w.W = hl + b + (m_saveData.registers.GetF() & Flag_C);

	m_saveData.registers.SetHL(w.W);

	m_saveData.registers.SetF(((w.W & Flag_F3H) ? Flag_F3 : 0) | ((w.W & Flag_F5H) ? Flag_F5 : 0) | (w.H & Flag_S) | ((w.W == 0) ? Flag_Z : 0) |
							(((hl ^ b ^ w.W) & 0x1000) ? Flag_H : 0) | (((~(hl ^ b)) & (b ^ w.W) & 0x8000) ? Flag_V : 0) |
							((((uint32_t) hl + (uint32_t) b + (uint32_t) (m_saveData.registers.GetF() & Flag_C)) & 0x10000) ? Flag_C : 0));

	m_saveData.registers.IncPC(2);
	m_saveData.cycles += 15;
}

/**
 * |2|15| Subtracts reg and the carry flag from hl.
 */
void CPUInst::SBCHLss(uint8_t reg) {
	uint16_t hl = m_saveData.registers.GetHL();
	uint16_t b = m_saveData.registers.GetRegss(reg);
	uint16_t value = hl - b;

	if (m_saveData.registers.GetF() & Flag_C)
		value--;

	m_saveData.registers.SetHL(value);

	m_saveData.registers.SetF(((value & Flag_F3H) ? Flag_F3 : 0) | ((value & Flag_F5H) ? Flag_F5 : 0) | ((value & 0x8000) ? Flag_S : 0) | ((value == 0) ? Flag_Z : 0) |
							(((value & 0xFFF) > (hl & 0xFFF)) ? Flag_H : 0) | (((hl ^ b) & (hl ^ value) & 0x8000) ? Flag_V : 0) | Flag_N | ((value > hl) ? Flag_C : 0));

	m_saveData.registers.IncPC(2);
	m_saveData.cycles += 15;
}

/**
 * |2|15| The value of pp is added to XX.
 */
void CPUInst::ADDXXpp(uint8_t XX, uint16_t reg2, uint8_t cycles, uint8_t size) {
	uint16_t reg1 = m_saveData.registers.GetRegss(XX);
	uint32_t value = (uint32_t) reg1 + (uint32_t) reg2;

	m_saveData.registers.SetRegss(XX, value);

	m_saveData.registers.SetF((value & Flag_F3H ? Flag_F3 : 0) | (value & Flag_F5H ? Flag_F5 : 0) | (((reg1 ^ reg2 ^ ((uint16_t) value)) & 0x1000) ? Flag_H : 0) |
							((value & 0x10000) ? Flag_C : 0) | (m_saveData.registers.GetF() & (Flag_Z | Flag_S | Flag_P)));

	m_saveData.registers.IncPC(size);
	m_saveData.cycles += cycles;
}

/**
 * |1|6| Adds one to reg
 */
void CPUInst::INCss(uint8_t reg, uint8_t cycles, uint8_t size) {
	m_saveData.registers.SetRegss(reg, m_saveData.registers.GetRegss(reg) + 1);
	m_saveData.registers.IncPC(size);
	m_saveData.cycles += cycles;
}

/**
 * |1|6| Subtracts one from ss
 */
void CPUInst::DECss(uint8_t reg, uint8_t cycles, uint8_t size) {
	m_saveData.registers.SetRegss(reg, m_saveData.registers.GetRegss(reg) - 1);
	m_saveData.registers.IncPC(size);
	m_saveData.cycles += cycles;
}

/******************************************************************************/
/*************************** Rotate and Shift Group ***************************/
/******************************************************************************/

/**
 * |1|4| The contents of a are rotated left one bit position. The contents of bit 7 are copied to the carry flag and bit 0.
 */
void CPUInst::RLCA() {
	uint8_t old = m_saveData.registers.GetA();
	uint8_t value = (old << 1);
	if (old & 0x80)
		value |= 1;

	m_saveData.registers.SetA(value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | ((old & 0x80) ? Flag_C : 0) | (m_saveData.registers.GetF() & (Flag_Z | Flag_S | Flag_P)));

	m_saveData.registers.IncPC();
	m_saveData.cycles += 4;
}

/**
 * |1|4| The contents of a are rotated left one bit position. The contents of bit 7 are copied to the carry flag and the previous contents of the carry flag are copied to bit 0.
 */
void CPUInst::RLA() {
	uint8_t old = m_saveData.registers.GetA();
	uint8_t value = (old << 1);
	if (m_saveData.registers.GetF() & Flag_C)
		value |= 1;

	m_saveData.registers.SetA(value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | ((old & 0x80) ? Flag_C : 0) | (m_saveData.registers.GetF() & (Flag_Z | Flag_S | Flag_P)));

	m_saveData.registers.IncPC();
	m_saveData.cycles += 4;
}

/**
 * |1|4| The contents of a are rotated right one bit position. The contents of bit 0 are copied to the carry flag and bit 7.
 */
void CPUInst::RRCA() {
	uint8_t old = m_saveData.registers.GetA();
	uint8_t value = (old >> 1);
	if (old & 0x01)
		value |= 0x80;

	m_saveData.registers.SetA(value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | ((old & 0x01) ? Flag_C : 0) | (m_saveData.registers.GetF() & (Flag_Z | Flag_S | Flag_P)));

	m_saveData.registers.IncPC();
	m_saveData.cycles += 4;
}

/**
 * |1|4| The contents of a are rotated right one bit position. The contents of bit 0 are copied to the carry flag and the previous contents of the carry flag are copied to bit 7.
 */
void CPUInst::RRA() {
	uint8_t old = m_saveData.registers.GetA();
	uint8_t value = (old >> 1);
	if (m_saveData.registers.GetF() & Flag_C)
		value |= 0x80;

	m_saveData.registers.SetA(value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | ((old & 0x01) ? Flag_C : 0) | (m_saveData.registers.GetF() & (Flag_Z | Flag_S | Flag_P)));

	m_saveData.registers.IncPC();
	m_saveData.cycles += 4;
}

/**
 * |2|8| The contents of b are rotated left one bit position. The contents of bit 7 are copied to the carry flag and bit 0.
 */
void CPUInst::RLC(uint8_t reg) {
	uint8_t old = m_saveData.registers.GetRegm(reg);
	uint8_t value = (old << 1);
	if (old & 0x80)
		value |= 1;

	m_saveData.registers.SetRegm(reg, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x80) ? Flag_C : 0));

	m_saveData.registers.IncPC(2);
	m_saveData.cycles += 8;
}

/**
 * RLC (HL)
 * |2|15| The contents of (hl) are rotated left one bit position. The contents of bit 7 are copied to the carry flag and bit 0.
 */
void CPUInst::RLC_HL() {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint16_t offset = m_saveData.registers.GetHL();
	uint8_t old = ReadMemory(offset);
	uint8_t value = (old << 1);
	if (old & 0x80)
		value |= 1;

	m_saveData.cycles++;

	WriteMemory(offset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x80) ? Flag_C : 0));
}

/**
 * TODO: Revisar
 * |4|23| The contents of the memory location pointed to by ix plus * are rotated left one bit position. The contents of bit 7 are copied to the carry flag and bit 0.
 */
void CPUInst::RLCXXd(uint8_t reg) {
	m_saveData.cycles += 14;
	m_saveData.registers.IncPC(4);

	uint16_t XX = m_saveData.registers.GetRegss(reg);
	uint16_t finalOffset = XX + ((int8_t) ReadMemory(m_saveData.registers.GetPC() - 2));
	uint8_t old = ReadMemory(finalOffset);

	uint8_t value = (old << 1);
	if (old & 0x80)
		value |= 1;

	WriteMemory(finalOffset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x80) ? Flag_C : 0));
}

/**
 * TODO: Revisar
 * RL
 * |2|8| The contents of b are rotated left one bit position. The contents of bit 7 are copied to the carry flag and the previous contents of the carry flag are copied to bit 0.
 */
void CPUInst::RL(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);
	uint8_t old = m_saveData.registers.GetRegm(reg);
	uint8_t value = (old << 1);
	if (m_saveData.registers.GetF() & Flag_C)
		value |= 1;

	m_saveData.registers.SetRegm(reg, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x80) ? Flag_C : 0));
}

/**
 * TODO: Revisar
 * RL (HL)
 * |2|15| The contents of (hl) are rotated left one bit position. The contents of bit 7 are copied to the carry flag and the previous contents of the carry flag are copied to bit
 * 0.
 */
void CPUInst::RL_HL() {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint16_t offset = m_saveData.registers.GetHL();
	uint8_t old = ReadMemory(offset);
	uint8_t value = (old << 1);
	if (m_saveData.registers.GetF() & Flag_C)
		value |= 1;

	m_saveData.cycles++;

	WriteMemory(offset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x80) ? Flag_C : 0));
}

/**
 * TODO: Revisar
 * |4|23| The contents of the memory location pointed to by ix plus * are rotated left one bit position. The contents of bit 7 are copied to the carry flag and the previous
 * contents of the carry flag are copied to bit 0.
 */
void CPUInst::RLXXd(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(4);

	uint16_t XX = m_saveData.registers.GetRegss(reg);
	uint16_t finalOffset = XX + ((int8_t) ReadMemory(m_saveData.registers.GetPC() - 2));

	m_saveData.cycles += 5;

	uint8_t old = ReadMemory(finalOffset);

	uint8_t value = (old << 1);
	if (m_saveData.registers.GetF() & Flag_C)
		value |= 1;

	m_saveData.cycles++;

	WriteMemory(finalOffset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x80) ? Flag_C : 0));
}

/**
 * |2|8| The contents of b are rotated right one bit position. The contents of bit 0 are copied to the carry flag and bit 7.
 */
void CPUInst::RRC(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint8_t old = m_saveData.registers.GetRegm(reg);
	uint8_t value = (old >> 1);
	if (old & 0x01)
		value |= 0x80;

	m_saveData.registers.SetRegm(reg, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x01) ? Flag_C : 0));
}

/**
 * TODO: Revisar
 * RRC (HL)
 * |2|15| The contents of (hl) are rotated right one bit position. The contents of bit 0 are copied to the carry flag and bit 7.
 */
void CPUInst::RRC_HL() {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint16_t offset = m_saveData.registers.GetHL();
	uint8_t old = ReadMemory(offset);
	uint8_t value = (old >> 1);
	if (old & 0x01)
		value |= 0x80;

	m_saveData.cycles++;

	WriteMemory(offset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x01) ? Flag_C : 0));
}

/**
 * TODO: Revisar
 * |4|23| The contents of the memory location pointed to by ix plus * are rotated right one bit position. The contents of bit 0 are copied to the carry flag and bit 7.
 */
void CPUInst::RRCXXd(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(4);

	uint16_t XX = m_saveData.registers.GetRegss(reg);
	uint16_t finalOffset = XX + ((int8_t) ReadMemory(m_saveData.registers.GetPC() - 2));

	m_saveData.cycles += 5;

	uint8_t old = ReadMemory(finalOffset);

	uint8_t value = (old >> 1);
	if (old & 0x01)
		value |= 0x80;

	m_saveData.cycles++;

	WriteMemory(finalOffset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x01) ? Flag_C : 0));
}

/**
 * |2|8| The contents of b are rotated right one bit position. The contents of bit 0 are copied to the carry flag and the previous contents of the carry flag are copied to bit 7.
 */
void CPUInst::RR(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);
	uint8_t old = m_saveData.registers.GetRegm(reg);
	uint8_t value = (old >> 1);
	if (m_saveData.registers.GetF() & Flag_C)
		value |= 0x80;

	m_saveData.registers.SetRegm(reg, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x01) ? Flag_C : 0));
}

/**
 * RR (HL)
 * |2|15| The contents of (hl) are rotated right one bit position. The contents of bit 0 are copied to the carry flag and the previous contents of the carry flag are copied to
 * bit 7.
 */
void CPUInst::RR_HL() {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint16_t offset = m_saveData.registers.GetHL();
	uint8_t old = ReadMemory(offset);
	uint8_t value = (old >> 1);
	if (m_saveData.registers.GetF() & Flag_C)
		value |= 0x80;

	m_saveData.cycles++;

	WriteMemory(offset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x01) ? Flag_C : 0));
}

/**
 * TODO: Revisar
 * |4|23| The contents of the memory location pointed to by ix plus * are rotated right one bit position. The contents of bit 0 are copied to the carry flag and the previous
 * contents of the carry flag are copied to bit 7.
 */
void CPUInst::RRXXd(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(4);

	uint16_t XX = m_saveData.registers.GetRegss(reg);
	uint16_t finalOffset = XX + ((int8_t) ReadMemory(m_saveData.registers.GetPC() - 2));

	m_saveData.cycles += 5;

	uint8_t old = ReadMemory(finalOffset);

	uint8_t value = (old >> 1);
	if (m_saveData.registers.GetF() & Flag_C)
		value |= 0x80;

	m_saveData.cycles++;

	WriteMemory(finalOffset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x01) ? Flag_C : 0));
}

/**
 * |2|8| The contents of b are shifted left one bit position. The contents of bit 7 are copied to the carry flag and a zero is put into bit 0.
 */
void CPUInst::SLA(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);
	uint8_t old = m_saveData.registers.GetRegm(reg);
	uint8_t value = old << 1;

	m_saveData.registers.SetRegm(reg, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x80) ? Flag_C : 0));
}

/**
 * TODO: Revisar
 * SLA (HL)
 * |2|15| The contents of (hl) are shifted left one bit position. The contents of bit 7 are copied to the carry flag and a zero is put into bit 0.
 */
void CPUInst::SLA_HL() {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint16_t offset = m_saveData.registers.GetHL();
	uint8_t old = ReadMemory(offset);
	uint8_t value = old << 1;

	m_saveData.cycles++;

	WriteMemory(offset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x80) ? Flag_C : 0));
}

/**
 * TODO: Revisar
 * |4|23| The contents of the memory location pointed to by ix plus * are shifted left one bit position. The contents of bit 7 are copied to the carry flag and a zero is put into
 * bit 0.
 */
void CPUInst::SLAXXd(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(4);

	uint16_t XX = m_saveData.registers.GetRegss(reg);
	uint16_t finalOffset = XX + ((int8_t) ReadMemory(m_saveData.registers.GetPC() - 2));

	m_saveData.cycles += 5;

	uint8_t old = ReadMemory(finalOffset);

	uint8_t value = old << 1;

	m_saveData.cycles++;

	WriteMemory(finalOffset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x80) ? Flag_C : 0));
}

/**
 * |2|8| The contents of b are shifted right one bit position. The contents of bit 0 are copied to the carry flag and the previous contents of bit 7 are unchanged.
 */
void CPUInst::SRA(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint8_t old = m_saveData.registers.GetRegm(reg);
	uint8_t value = (old & 0x80) | (old >> 1);

	m_saveData.registers.SetRegm(reg, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x01) ? Flag_C : 0));
}

/**
 * TODO: Revisar
 * SRA (HL)
 * |2|15| The contents of (hl) are shifted right one bit position. The contents of bit 0 are copied to the carry flag and the previous contents of bit 7 are unchanged.
 */
void CPUInst::SRA_HL() {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint16_t offset = m_saveData.registers.GetHL();
	uint8_t old = ReadMemory(offset);
	uint8_t value = (old & 0x80) | (old >> 1);

	m_saveData.cycles++;

	WriteMemory(offset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x01) ? Flag_C : 0));
}

/**
 * TODO: Revisar
 * |4|23| The contents of the memory location pointed to by ix plus * are shifted right one bit position. The contents of bit 0 are copied to the carry flag and the previous
 * contents of bit 7 are unchanged.
 */
void CPUInst::SRAXXd(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(4);

	uint16_t XX = m_saveData.registers.GetRegss(reg);
	uint16_t finalOffset = XX + ((int8_t) ReadMemory(m_saveData.registers.GetPC() - 2));

	m_saveData.cycles += 5;

	uint8_t old = ReadMemory(finalOffset);
	uint8_t value = (old & 0x80) | (old >> 1);

	m_saveData.cycles++;

	WriteMemory(finalOffset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x01) ? Flag_C : 0));
}

/**
 * |2|8| The contents of b are shifted left one bit position. The contents of bit 7 are put into the carry flag and a one is put into bit 0.
 */
void CPUInst::SLL(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint8_t old = m_saveData.registers.GetRegm(reg);
	uint8_t value = (old << 1) | 1;

	m_saveData.registers.SetRegm(reg, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x80) ? Flag_C : 0));
}

/**
 * SLL (HL)
 * |2|15| The contents of (hl) are shifted left one bit position. The contents of bit 7 are put into the carry flag and a one is put into bit 0.
 */
void CPUInst::SLL_HL() {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint16_t offset = m_saveData.registers.GetHL();
	uint8_t old = ReadMemory(offset);
	uint8_t value = (old << 1) | 1;

	m_saveData.cycles++;

	WriteMemory(offset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x80) ? Flag_C : 0));
}

/**
 * TODO: Revisar
 * |4|23| The contents of the memory location pointed to by ix plus * are shifted left one bit position. The contents of bit 7 are put into the carry flag and a one is put into bit
 * 0.
 */
void CPUInst::SLLXXd(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(4);

	uint16_t XX = m_saveData.registers.GetRegss(reg);
	uint16_t finalOffset = XX + ((int8_t) ReadMemory(m_saveData.registers.GetPC() - 2));

	m_saveData.cycles += 5;

	uint8_t old = ReadMemory(finalOffset);

	uint8_t value = (old << 1) | 1;

	m_saveData.cycles++;

	WriteMemory(finalOffset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x80) ? Flag_C : 0));
}

/**
 * |2|8| The contents of b are shifted right one bit position. The contents of bit 0 are copied to the carry flag and a zero is put into bit 7.
 */
void CPUInst::SRL(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint8_t old = m_saveData.registers.GetRegm(reg);
	uint8_t value = old >> 1;

	m_saveData.registers.SetRegm(reg, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x01) ? Flag_C : 0));
}

/**
 * TODO: Revisar
 * |2|15| The contents of (hl) are shifted right one bit position. The contents of bit 0 are copied to the carry flag and a zero is put into bit 7.
 */
void CPUInst::SRL_HL() {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint16_t offset = m_saveData.registers.GetHL();
	uint8_t old = ReadMemory(offset);
	uint8_t value = old >> 1;

	m_saveData.cycles++;

	WriteMemory(offset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x01) ? Flag_C : 0));
}

/**
 * TODO: Revisar
 * |4|23| The contents of the memory location pointed to by ix plus * are shifted right one bit position. The contents of bit 0 are copied to the carry flag and a zero is put into
 * bit 7.
 */
void CPUInst::SRLXXd(uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(4);

	uint16_t XX = m_saveData.registers.GetRegss(reg);
	uint16_t finalOffset = XX + ((int8_t) ReadMemory(m_saveData.registers.GetPC() - 2));

	m_saveData.cycles += 5;

	uint8_t old = ReadMemory(finalOffset);
	uint8_t value = old >> 1;

	m_saveData.cycles++;

	WriteMemory(finalOffset, value);

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | PZS_Flags[value] | ((old & 0x01) ? Flag_C : 0));
}

/**
 * RLD -> pc:4,pc+1:4,hl:3,hl:1 x 4,hl(write):3
 * |2|18| The contents of the low-order nibble of (hl) are copied to the high-order nibble of (hl). The previous contents are copied to the low-order nibble of a. The previous
 * contents are copied to the low-order nibble of (hl).
 */
void CPUInst::RLD() {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint16_t offset = m_saveData.registers.GetHL();
	uint8_t HL = ReadMemory(offset);
	uint8_t A = m_saveData.registers.GetA();
	uint8_t valueA = (A & 0xF0) | (HL >> 4);
	uint8_t valueHL = (HL << 4) | (A & 0x0F);

	m_saveData.registers.SetA(valueA);

	m_saveData.cycles += 4;

	WriteMemory(offset, valueHL);

	m_saveData.registers.SetF((valueA & (Flag_F3 | Flag_F5)) | PZS_Flags[valueA] | (m_saveData.registers.GetF() & Flag_C));
}

/**
 * RRD -> pc:4,pc+1:4,hl:3,hl:1 x 4,hl(write):3
 * |2|18| The contents of the low-order nibble of (hl) are copied to the low-order nibble of a. The previous contents are copied to the high-order nibble of (hl). The previous
 * contents are copied to the low-order nibble of (hl).
 */
void CPUInst::RRD() {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint16_t offset = m_saveData.registers.GetHL();
	uint8_t HL = ReadMemory(offset);
	uint8_t A = m_saveData.registers.GetA();
	uint8_t valueA = (A & 0xF0) | (HL & 0x0F);
	uint8_t valueHL = (A << 4) | (HL >> 4);

	m_saveData.registers.SetA(valueA);

	m_saveData.cycles += 4;

	WriteMemory(offset, valueHL);

	m_saveData.registers.SetF((valueA & (Flag_F3 | Flag_F5)) | PZS_Flags[valueA] | (m_saveData.registers.GetF() & Flag_C));
}

/******************************************************************************/
/*********************** Bit Set, Reset, and Test Group ***********************/
/******************************************************************************/

/**
 * BIT b, r -> pc:4,pc+1:4
 * |2|8| Tests bit compare of value.
 */
void CPUInst::BIT(uint8_t valueb, uint8_t compare) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint8_t value = valueb & compare;

	m_saveData.registers.SetF((value & (Flag_F3 | Flag_F5)) | Flag_H | PZS_Flags[value] | (m_saveData.registers.GetF() & Flag_C));
}

/**
 * BIT b, (HL) -> pc:4,pc+1:4,hl:3,hl:1
 * |2|12| Tests bit compare of (HL).
 */
void CPUInst::BITHL(uint8_t compare) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint16_t HL = m_saveData.registers.GetHL();
	uint8_t valueb = ReadMemory(HL);
	uint8_t value = valueb & compare;

	m_saveData.cycles++;

	m_saveData.registers.SetF(((HL & Flag_F3H) ? Flag_F3 : 0) | ((HL & Flag_F5H) ? Flag_F5 : 0) | Flag_H | PZS_Flags[value] | (m_saveData.registers.GetF() & Flag_C));
}

/**
 * TODO: Revisar
 * pc:4,pc+1:4,pc+2:3,pc+3:3,pc+3:1 x 2,ii+n:3,ii+n:1
 * |4|20| Tests bit 'bit' of the memory location pointed to by ss plus *.
 */
void CPUInst::BITbssd(uint8_t bit, uint8_t reg) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(4);

	uint8_t d = ReadMemory(m_saveData.registers.GetPC() - 2);

	m_saveData.cycles += 5;

	uint16_t offset = m_saveData.registers.GetRegss(reg) + ((int8_t) d);
	uint8_t value = ReadMemory(offset) & bit;

	m_saveData.cycles++;

	m_saveData.registers.SetF(((offset & Flag_F3H) ? Flag_F3 : 0) | ((offset & Flag_F5H) ? Flag_F5 : 0) | Flag_H | PZS_Flags[value] | (m_saveData.registers.GetF() & Flag_C));
}

/**
 * |2|8| Sets bit X of reg.
 */
void CPUInst::SET(uint8_t reg, uint8_t bit) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);
	m_saveData.registers.SetRegm(reg, m_saveData.registers.GetRegm(reg) | bit);
}

/**
 * SET b, (HL) -> pc:4,pc+1:4,hl:3,hl:1,hl(write):3
 * |2|15| Sets bit X of (HL).
 */
void CPUInst::SETHL(uint8_t bit) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);
	uint16_t offset = m_saveData.registers.GetHL();
	uint8_t value = ReadMemory(offset) | bit;
	m_saveData.cycles++;
	WriteMemory(offset, value);
}

/**
 * pc:4,pc+1:4,pc+2:3,pc+3:3,pc+3:1 x 2,ii+n:3,ii+n:1,ii+n(write):3
 * |4|23| Sets bit 'bit' of the memory location pointed to by ss plus *
 */
void CPUInst::SETbssd(uint8_t bit, uint8_t reg) {
	m_saveData.cycles += 11;
	m_saveData.registers.IncPC(4);

	uint16_t pc = m_saveData.registers.GetPC();
	uint8_t d = ReadMemory(pc - 2);

	uint16_t offset = m_saveData.registers.GetRegss(reg) + ((int8_t) d);

	m_saveData.cycles += 2;

	uint8_t value = ReadMemory(offset) | bit;
	m_saveData.cycles++;
	WriteMemory(offset, value);
}

/**
 * |2|8| Resets bit X of reg.
 */
void CPUInst::RES(uint8_t reg, uint8_t bit) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);
	m_saveData.registers.SetRegm(reg, m_saveData.registers.GetRegm(reg) & ~bit);
}

/**
 * RES b,(HL) -> pc:4,pc+1:4,hl:3,hl:1,hl(write):3
 * |2|15| Resets bit X of (HL).
 */
void CPUInst::RESHL(uint8_t bit) {
	m_saveData.cycles += 8;
	m_saveData.registers.IncPC(2);

	uint16_t offset = m_saveData.registers.GetHL();
	uint8_t value = ReadMemory(offset) & ~bit;
	m_saveData.cycles++;
	WriteMemory(offset, value);
}

/**
 * RES b,(ii+n) -> pc:4,pc+1:4,pc+2:3,pc+3:3,pc+3:1 x 2,ii+n:3,ii+n:1,ii+n(write):3
 * |4|23| Resets bit 'bit' of the memory location pointed to by ss plus *.
 */
void CPUInst::RESbssd(uint8_t bit, uint8_t reg) {
	m_saveData.cycles += 11;
	m_saveData.registers.IncPC(4);

	uint16_t pc = m_saveData.registers.GetPC();
	uint8_t d = ReadMemory(pc - 2);

	uint16_t offset = m_saveData.registers.GetRegss(reg) + ((int8_t) d);

	m_saveData.cycles += 2;

	uint8_t value = ReadMemory(offset) & ~bit;
	m_saveData.cycles++;
	WriteMemory(offset, value);
}

/******************************************************************************/
/********************************* Jump Group *********************************/
/******************************************************************************/

/**
 * |3|10| If condition cc is true, ** is copied to pc.
 */
void CPUInst::JPccnn(bool cc) {
	m_saveData.cycles += 4;
	uint16_t pc = m_saveData.registers.GetPC();

	Word offset;
	offset.H = ReadMemory(pc + 2);
	offset.L = ReadMemory(pc + 1);

	if (cc)
		m_saveData.registers.SetPC(offset.W);
	else
		m_saveData.registers.IncPC(3);
}

/**
 * pc:4,pc+1:3,[pc+1:1 x 5]
 * |2|12/7| If condition cc is true, the signed value * is added to pc.
 * The jump is measured from the start of the instruction opcode.
 */
void CPUInst::JR(bool cc) {
	m_saveData.cycles += 4;
	int8_t offset = ReadMemory(m_saveData.registers.GetPC() + 1);

	if (cc) {
		m_saveData.registers.IncPC(offset + 2);
		m_saveData.cycles += 5;
	} else
		m_saveData.registers.IncPC(2);
}

/******************************************************************************/
/*************************** Call And Return Group ****************************/
/******************************************************************************/

/**
 * RET -> pc:4,sp:3,sp+1:3
 * |1|10| The top stack entry is popped into pc.
 */
void CPUInst::RET() {
	m_saveData.cycles += 4;
	uint16_t sp = m_saveData.registers.GetSP();
	Word pc;
	pc.H = ReadMemory(sp + 1);
	pc.L = ReadMemory(sp);

	m_saveData.inInterrupt = false;

	m_saveData.registers.SetSP(sp + 2);
	m_saveData.registers.SetPC(pc.W);
}

/**
 * RET cc -> pc:4,ir:1,[sp:3,sp+1:3]
 * |1|11/5| If condition cc is true, the top stack entry is popped into pc.
 */
void CPUInst::RETcc(bool cc) {
	m_saveData.cycles += 5;
	if (cc) {
		uint16_t sp = m_saveData.registers.GetSP();
		Word pc;
		pc.H = ReadMemory(sp + 1);
		pc.L = ReadMemory(sp);
		m_saveData.registers.SetSP(sp + 2);
		m_saveData.registers.SetPC(pc.W);
	} else
		m_saveData.registers.IncPC();
}

/**
 * RST n -> pc:4,ir:1,sp-1:3,sp-2:3
 * |1|11| The current pc value plus one is pushed onto the stack, then is loaded with ph.
 */
void CPUInst::RSTp(uint8_t p) {
	m_saveData.cycles += 5;
	Word pc;
	pc.W = m_saveData.registers.GetPC() + 1;
	uint16_t sp = m_saveData.registers.GetSP() - 2;
	m_saveData.registers.SetSP(sp);
	WriteMemory(sp + 1, pc.H);
	WriteMemory(sp, pc.L);

	m_saveData.registers.SetPC(p);
}

/**
 * CALL ** -> pc:4,pc+1:3,pc+2:3,[pc+2:1,sp-1:3,sp-2:3]
 * |3|17| The current pc value plus three is pushed onto the stack, then is loaded with **
 */
void CPUInst::CALLnn() {
	m_saveData.cycles += 4;
	Word pc;
	pc.W = m_saveData.registers.GetPC() + 3;
	uint16_t sp = m_saveData.registers.GetSP() - 2;
	m_saveData.registers.SetSP(sp);
	WriteMemory(sp, pc.L);
	WriteMemory(sp + 1, pc.H);
	m_saveData.cycles++;
	m_saveData.registers.SetPC((ReadMemory(pc.W - 1) << 8) | ReadMemory(pc.W - 2));
}

/**
 * CALL cc,nn -> pc:4,pc+1:3,pc+2:3,[pc+2:1,sp-1:3,sp-2:3]
 * |3|17/10| If condition cc is true, the current pc value plus three is pushed onto the stack, then is loaded with **.
 */
void CPUInst::CALLccnn(bool cc) {
	if (cc)
		CALLnn();
	else {
		m_saveData.registers.IncPC(3);
		m_saveData.cycles += 10;
	}
}

void CPUInst::CallInterrupt(uint16_t offset) {
	m_saveData.cycles += 5;

	if (m_saveData.isSuspended) {
		m_saveData.registers.IncPC();
		m_saveData.isSuspended = false;
	}

	Word pc;
	pc.W = m_saveData.registers.GetPC();
	uint16_t sp = m_saveData.registers.GetSP() - 2;
	m_saveData.registers.SetSP(sp);
	WriteMemory(sp, pc.L);
	WriteMemory(sp + 1, pc.H);
	m_saveData.registers.SetPC(offset);
}

/**
 * RETI -> pc:4,pc+1:4,sp:3,sp+1:3
 * |2|14| Used at the end of a maskable interrupt service routine. The top stack entry is popped into pc, and signals an I/O device that the interrupt has finished, allowing nested
 * interrupts (not a consideration on the TI).
 */
void CPUInst::RETI() {
	m_saveData.cycles += 8;

	uint16_t sp = m_saveData.registers.GetSP();
	Word pc;
	pc.H = ReadMemory(sp + 1);
	pc.L = ReadMemory(sp);
	m_saveData.registers.SetSP(sp + 2);
	m_saveData.registers.SetPC(pc.W);
	m_saveData.registers.SetIFF1(m_saveData.registers.GetIFF2());
}

/**
 * RETN -> pc:4,pc+1:4,sp:3,sp+1:3
 * |2|14| Used at the end of a non-maskable interrupt service routine (located at $0066) to pop the top stack entry into PC. The value of IFF2 is copied to IFF1 so that maskable
 * interrupts are allowed to continue as before. NMIs are not enabled on the TI.
 */
void CPUInst::RETN() {
	m_saveData.cycles += 8;

	uint16_t sp = m_saveData.registers.GetSP();
	Word pc;
	pc.H = ReadMemory(sp + 1);
	pc.L = ReadMemory(sp);
	m_saveData.registers.SetSP(sp + 2);
	m_saveData.registers.SetPC(pc.W);
	m_saveData.registers.SetIFF1(m_saveData.registers.GetIFF2());
}

/******************************************************************************/
/*************************** Input and Output Group ***************************/
/******************************************************************************/

// |2|12| A byte from port c is written to reg.
void CPUInst::INrC(uint8_t reg) {
	uint8_t C = m_saveData.registers.GetC();

	m_saveData.addressBus.L = C;
	m_saveData.addressBus.H = m_saveData.registers.GetB();

	//	printf("INrC\n");
	uint8_t data = ReadPort(C);

	m_saveData.registers.SetRegm(reg, data);

	m_saveData.registers.SetF(PZS_Flags[data] | (m_saveData.registers.GetF() & Flag_C));

	m_saveData.registers.IncPC(2);
	m_saveData.cycles += 12;
}

// |2|16| A byte from port c is written to the memory location pointed to by hl. Then hl is incremented and b is decremented.
void CPUInst::INI() {
	uint16_t HL = m_saveData.registers.GetHL();
	uint8_t B = m_saveData.registers.GetB();
	uint8_t C = m_saveData.registers.GetC();

	m_saveData.addressBus.L = C;
	m_saveData.addressBus.H = B;
	//	printf("INI\n");
	WriteMemory(HL, ReadPort(C));
	m_saveData.addressBus.W = HL;
	B = B - 1;

	m_saveData.registers.SetHL(HL + 1);
	m_saveData.registers.SetB(B);
	m_saveData.registers.SetFFlag(Flag_Z, B == 0);
	m_saveData.registers.SetFFlag(Flag_N, true);

	m_saveData.registers.IncPC(2);
	m_saveData.cycles += 13; // 16 - 3
}

/**
 * pc:4,pc+1:3,IO
 * |2|11| The value of a is written to port *.
 */
void CPUInst::OUTnA() {
	m_saveData.cycles += 4;
	uint8_t n = ReadMemory(m_saveData.registers.GetPC() + 1);
	uint8_t data = m_saveData.registers.GetA();

	m_saveData.addressBus.L = n;
	m_saveData.addressBus.H = data;
	//	printf("OUTnA\n");
	WritePort(n, data);

	m_saveData.registers.IncPC(2);
	m_saveData.cycles += 4;
}

// |2|12| The value of reg is written to port c.
void CPUInst::OUTC(uint8_t value) {
	uint8_t C = m_saveData.registers.GetC();
	uint8_t B = m_saveData.registers.GetB();

	m_saveData.addressBus.L = C;
	m_saveData.addressBus.H = B;
	//	printf("OUTC\n");
	WritePort(C, value);

	m_saveData.registers.IncPC(2);
	m_saveData.cycles += 12;
}

/**
 * OUTD -> pc:4,pc+1:4,ir:1,hl:3,IO,[bc:1 x 5]
 * |2|16| A byte from the memory location pointed to by hl is written to port c. Then hl is incremented and b is decremented.
 */
void CPUInst::OUTI() {
	m_saveData.cycles += 9;
	m_saveData.registers.IncPC(2);

	uint8_t C = m_saveData.registers.GetC();
	uint8_t B = m_saveData.registers.GetB() - 1;
	uint16_t HL = m_saveData.registers.GetHL();
	uint8_t value = ReadMemory(HL);

	//	printf("OUTI\n");
	WritePort(C, value);
	m_saveData.addressBus.L = C;
	m_saveData.addressBus.H = B;

	m_saveData.registers.SetHL(HL + 1);
	m_saveData.registers.SetB(B);
	m_saveData.registers.SetFFlag(Flag_Z, B == 0);
	m_saveData.registers.SetFFlag(Flag_N, true);

	m_saveData.cycles += 4;
}

/**
 * OUTD -> pc:4,pc+1:4,ir:1,hl:3,IO,[bc:1 x 5]
 * |2|16| A byte from the memory location pointed to by hl is written to port c. Then hl and b are decremented.
 */
void CPUInst::OUTD() {
	m_saveData.cycles += 9;
	m_saveData.registers.IncPC(2);

	uint8_t B = m_saveData.registers.GetB() - 1;
	uint8_t C = m_saveData.registers.GetC();
	uint16_t HL = m_saveData.registers.GetHL();

	//	printf("OUTD\n");
	WritePort(C, ReadMemory(HL));
	m_saveData.addressBus.L = C;
	m_saveData.addressBus.H = B;

	m_saveData.registers.SetHL(HL - 1);
	m_saveData.registers.SetB(B);
	m_saveData.registers.SetFFlag(Flag_Z, B == 0);
	m_saveData.registers.SetFFlag(Flag_N, true);

	m_saveData.cycles += 4;
}

int CPUInst::GetSaveSize() {
	return sizeof(CPUInst::saveData);
}

void CPUInst::LoadState(uint8_t *data) {
	memcpy(&m_saveData, data, sizeof(CPUInst::saveData));
}

void CPUInst::SaveState(uint8_t *data) {
	memcpy(data, &m_saveData, sizeof(CPUInst::saveData));
}

void CPUInst::SetWriteMemoryCB(void (*fun)(uint16_t, uint8_t, void *), void *data) {
	m_writeMemoryCB = fun;
	m_writeMemoryDataCB = data;
}

void CPUInst::SetReadMemoryCB(uint8_t (*fun)(uint16_t, void *), void *data) {
	m_readMemoryCB = fun;
	m_readMemoryDataCB = data;
}

void CPUInst::SetWritePortCB(void (*fun)(uint8_t, uint8_t, void *), void *data) {
	m_writePortCB = fun;
	m_writePortDataCB = data;
}

void CPUInst::SetReadPortCB(uint8_t (*fun)(uint8_t, void *), void *data) {
	m_readPortCB = fun;
	m_readPortDataCB = data;
}
