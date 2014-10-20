/*
 * awui/Emulation/MasterSystem/CPUInst.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "CPUInst.h"

#include <awui/Emulation/MasterSystem/Ports.h>
#include <awui/Emulation/MasterSystem/Ram.h>
#include <awui/Emulation/MasterSystem/Registers.h>
#include <awui/Emulation/MasterSystem/Rom.h>
#include <assert.h>

using namespace awui::Emulation::MasterSystem;

// Setting a bit:
//   number |= 1 << x;
//
// Clearing a bit:
//   number &= ~(1 << x);
//
// Toggling a bit:
//   number ^= 1 << x;
//
// Checking a bit:
//   bit = number & (1 << x);

static bool PARITYEVEN(uint8_t value) {
    bool parity = false;
    while (value) {
        parity = !parity;
        value = value & (value - 1);
    }

    return parity;
}

CPUInst::CPUInst() {
	this->_ports = new Ports();
	this->_ram = new Ram(8192);
	this->_registers = new Registers();
	this->_rom = new Rom(4096);
}

CPUInst::~CPUInst() {
	delete this->_rom;
	delete this->_registers;
	delete this->_ram;
	delete this->_ports;
}

void CPUInst::Reset() {
	this->_cycles = 0;
	this->_registers->Clear();
}

void CPUInst::WriteMemory(uint16_t pos, uint8_t value) {
	// RAM
	if (pos >= 0xC000) {
		// RAM or RAM (mirror)
		if (pos < 0xE000)
			this->_ram->WriteByte(pos - 0xC000, value);
		else
			this->_ram->WriteByte(pos - 0xE000, value);
	} else {
		this->_rom->WriteByte(pos, value);
	}
}

uint8_t CPUInst::ReadMemory(uint16_t pos) {
	// RAM
	if (pos >= 0xC000) {
		// RAM or RAM (mirror)
		if (pos < 0xE000)
			return this->_ram->ReadByte(pos - 0xC000);
		else
			return this->_ram->ReadByte(pos - 0xE000);
	}

	return this->_rom->ReadByte(pos);
}

/******************************************************************************/
/****************************** 8-Bit Load Group ******************************/
/******************************************************************************/

// |1|4| The contents of reg2 are loaded into reg1
void CPUInst::LDrr(uint8_t reg1, uint8_t reg2, uint8_t size) {
	this->_registers->SetRegm(reg1, this->_registers->GetRegm(reg2));
	this->_registers->IncPC(size);
	this->_cycles += 4;
}

// |2|7| Loads * into reg
void CPUInst::LDrn(uint8_t reg) {
	this->_registers->SetRegm(reg, this->ReadMemory(this->_registers->GetPC() + 1));
	this->_registers->IncPC(2);
	this->_cycles += 7;
}

// |1|7| The contents of (hl) are loaded into reg
void CPUInst::LDrHL(uint8_t reg) {
	this->_registers->SetRegm(reg, this->ReadMemory(this->_registers->GetHL()));
	this->_registers->IncPC();
	this->_cycles += 7;
}

// |3|19| Loads the value pointed to by ix plus * into reg
void CPUInst::LDrXXd(uint8_t reg, uint8_t reg2) {
	this->_registers->SetRegm(reg, this->ReadMemory(this->_registers->GetRegss(reg2) + this->ReadMemory(this->_registers->GetPC() + 2)));
	this->_registers->IncPC(3);
	this->_cycles += 19;
}

// |1|7| The contents of reg are loaded into (ss).
void CPUInst::LDssr(uint8_t reg, uint8_t ss) {
	this->WriteMemory(this->_registers->GetRegss(ss), this->_registers->GetRegm(reg));
	this->_registers->IncPC();
	this->_cycles += 7;
}

// |3|19| Stores reg to the memory location pointed to by xx plus *.
void CPUInst::LDXXdr(uint8_t xx, uint8_t reg) {
	uint16_t x = this->_registers->GetRegss(xx);
	uint16_t offset = x + this->ReadMemory(this->_registers->GetPC() + 2);
	this->WriteMemory(offset, this->_registers->GetRegm(reg));
	this->_registers->IncPC(3);
	this->_cycles += 19;
}

// |4|19| Stores * to the memory location pointed to by xx plus *.
void CPUInst::LDXXdn(uint8_t xx) {
	uint16_t x = this->_registers->GetRegss(xx);
	uint16_t offset = x + this->ReadMemory(this->_registers->GetPC() + 2);
	uint16_t n = this->ReadMemory(this->_registers->GetPC() + 3);
	this->WriteMemory(offset, n);
	this->_registers->IncPC(4);
	this->_cycles += 19;
}

/******************************************************************************/
/***************************** 16-Bit Load Group ******************************/
/******************************************************************************/

// |3|10| Loads ** into reg
void CPUInst::LDddnn(uint8_t reg, uint8_t size) {
	uint16_t pc = this->_registers->GetPC();
	this->_registers->SetRegss(reg, (this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1));
	this->_registers->IncPC(size);
	this->_cycles += 10;
}

// |4|20| Loads the value pointed to by ** into reg.
void CPUInst::LDdd_nn(uint8_t reg) {
	uint16_t pc = this->_registers->GetPC();
	uint16_t offset = (this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1);
	this->_registers->SetRegss(reg, (this->ReadMemory(offset + 1) << 8) | this->ReadMemory(offset));
	this->_registers->IncPC(4);
	this->_cycles += 20;
}

// |4|20| Stores reg into the memory location pointed to by **
void CPUInst::LDnndd(uint8_t reg, uint8_t cycles, uint8_t size) {
	uint16_t pc = this->_registers->GetPC();
	uint16_t word = this->_registers->GetRegss(reg);
	uint8_t high = word >> 8;
	uint8_t low = word;
	uint16_t offset = (this->ReadMemory(pc + size - 1) << 8) | this->ReadMemory(pc + size - 2);
	this->WriteMemory(offset, low);
	this->WriteMemory(offset + 1, high);
	this->_registers->IncPC(size);
	this->_cycles += cycles;
}

void CPUInst::PUSH16(uint8_t reg, uint8_t cycles, uint8_t size) {
	uint16_t value = this->_registers->GetRegss(reg);
	uint8_t high = value >> 8;
	uint8_t low = (uint8_t) value;
	uint16_t sp = this->_registers->GetSP();
	this->WriteMemory(sp - 1, high);
	this->WriteMemory(sp - 2, low);
	this->_registers->SetSP(sp - 2);
	this->_registers->IncPC(size);
	this->_cycles += cycles;
}

void CPUInst::POP16(uint8_t reg, uint8_t cycles, uint8_t size) {
	uint16_t sp = this->_registers->GetSP();
	uint16_t value = (this->ReadMemory(sp + 1) << 8) | this->ReadMemory(sp);
	this->_registers->SetRegss(reg, value);
	this->_registers->SetSP(sp + 2);
	this->_registers->IncPC(size);
	this->_cycles += cycles;
}

void CPUInst::LDSPr(uint8_t reg, uint8_t cycles, uint8_t size) {
	this->_registers->SetSP(this->_registers->GetRegss(reg));
	this->_registers->IncPC(size);
	this->_cycles += cycles;
}

/******************************************************************************/
/***************** Exchange, Block Transfer, and Search Group *****************/
/******************************************************************************/

// |2|16| Transfers a byte of data from the memory location pointed to by hl to the memory location pointed to by de. Then hl and de are incremented and bc is decremented.
void CPUInst::LDI() {
	uint8_t dataHL = this->ReadMemory(this->_registers->GetHL());
	this->WriteMemory(this->_registers->GetDE(), dataHL);
	this->_registers->SetHL(this->_registers->GetHL() + 1);
	this->_registers->SetDE(this->_registers->GetDE() + 1);
	uint16_t value = this->_registers->GetBC() - 1;
	this->_registers->SetBC(value);
	this->_registers->SetFFlag(FFlag_H, false);
	this->_registers->SetFFlag(FFlag_PV, value != 0);
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->IncPC(2);
	this->_cycles += 16;
}

// |2|16| Compares the value of the memory location pointed to by hl with a. Then hl is incremented and bc is decremented.
void CPUInst::CPI() {
	uint16_t HL = this->_registers->GetHL();
	uint8_t b = this->ReadMemory(HL);
	uint8_t old = this->_registers->GetA();
	uint8_t value = old - b;
	int16_t pvalue = ((int8_t) old) - ((int8_t) b);
	uint16_t BC = this->_registers->GetBC() - 1;

	this->_registers->SetHL(HL + 1);
	this->_registers->SetBC(BC);

	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, (value & 0xF) > (old & 0xF));
	this->_registers->SetFFlag(FFlag_PV, BC != 0);
	this->_registers->SetFFlag(FFlag_N, true);
	this->_registers->IncPC(2);
	this->_cycles += 16;
}

/******************************************************************************/
/*************************** 8-Bit Arithmetic Group ***************************/
/******************************************************************************/

// |1|4| Adds valueb to a.
void CPUInst::ADD(uint8_t valueb, uint8_t cycles, uint8_t size) {
	uint8_t old = this->_registers->GetA();
	uint8_t value = old + valueb;
	int16_t pvalue = ((int8_t) old) + ((int8_t) valueb);

	this->_registers->SetA(value);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, (value & 0xF) < (old & 0xF));
	this->_registers->SetFFlag(FFlag_PV, pvalue > 127 || pvalue < -128);
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, value < old);
	this->_registers->IncPC(size);
	this->_cycles += cycles;
}

// |1|4| Adds l and the carry flag to a.
void CPUInst::ADC(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t old = this->_registers->GetA();
	uint8_t value = old + b;
	int16_t pvalue = ((int8_t) old) + ((int8_t) b);

	if (this->_registers->GetF() & FFlag_C) {
		value++;
		pvalue++;
	}

	this->_registers->SetA(value);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, (value & 0xF) < (old & 0xF));
	this->_registers->SetFFlag(FFlag_PV, pvalue > 127 || pvalue < -128);
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, value < old);
	this->_registers->IncPC(size);
	this->_cycles += cycles;
}

// |1|4| Subtracts reg from a.
void CPUInst::SUB(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t old = this->_registers->GetA();
	uint8_t value = old - b;
	int16_t pvalue = ((int8_t) old) - ((int8_t) b);

	this->_registers->SetA(value);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, (value & 0xF) > (old & 0xF));
	this->_registers->SetFFlag(FFlag_PV, pvalue > 127 || pvalue < -128);
	this->_registers->SetFFlag(FFlag_N, true);
	this->_registers->SetFFlag(FFlag_C, value > old);
	this->_registers->IncPC(size);
	this->_cycles += cycles;
}

// |1|4| Subtracts e and the carry flag from a.
void CPUInst::SBC(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t old = this->_registers->GetA();
	uint8_t value = old - b;
	int16_t pvalue = ((int8_t) old) - ((int8_t) b);

	if (this->_registers->GetF() & FFlag_C) {
		value--;
		pvalue--;
	}

	this->_registers->SetA(value);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, (value & 0xF) > (old & 0xF));
	this->_registers->SetFFlag(FFlag_PV, pvalue > 127 || pvalue < -128);
	this->_registers->SetFFlag(FFlag_N, true);
	this->_registers->SetFFlag(FFlag_C, value > old);
	this->_registers->IncPC(size);
	this->_cycles += cycles;
}

// |1|4|Bitwise AND on a with valueb.
void CPUInst::AND(uint8_t valueb, uint8_t cycles, uint8_t size) {
	uint8_t value = this->_registers->GetA() & valueb;
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, true);
	this->_registers->SetFFlag(FFlag_PV, PARITYEVEN(value));
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, false);
	this->_registers->SetA(value);
	this->_registers->IncPC(size);
	this->_cycles += cycles;
}

// |1|4| Bitwise OR on a with valueb
void CPUInst::OR(uint8_t valueb, uint8_t cycles, uint8_t size) {
	uint8_t value = this->_registers->GetA() | valueb;
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, false);
	this->_registers->SetFFlag(FFlag_PV, PARITYEVEN(value));
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, false);
	this->_registers->SetA(value);
	this->_registers->IncPC(size);
	this->_cycles += cycles;
}

// |1/2|4/7| Bitwise XOR on a with b.
void CPUInst::XOR(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t value = this->_registers->GetA() ^ b;
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, false);
	this->_registers->SetFFlag(FFlag_PV, PARITYEVEN(value));
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, false);
	this->_registers->SetA(value);
	this->_registers->IncPC(size);
	this->_cycles += cycles;
}

void CPUInst::CP(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t old = this->_registers->GetA();
	uint8_t value = old - b;
	int16_t pvalue = ((int8_t) old) - ((int8_t) b);

	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, (value & 0xF) > (old & 0xF));
	this->_registers->SetFFlag(FFlag_PV, pvalue > 127 || pvalue < -128);
	this->_registers->SetFFlag(FFlag_N, true);
	this->_registers->SetFFlag(FFlag_C, value > old);
	this->_registers->IncPC(size);
	this->_cycles += cycles;
}

// |1|4| Adds one to reg
void CPUInst::INCr(uint8_t reg) {
	uint8_t old = this->_registers->GetRegm(reg);
	uint8_t value = old + 1;
	this->_registers->SetRegm(reg, value);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, (value & 0xF) < (old & 0xF));
	this->_registers->SetFFlag(FFlag_PV, old == 0x7F);
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->IncPC();
	this->_cycles += 4;
}

// |1|4| Subtracts one from m
void CPUInst::DECm(uint8_t reg) {
	uint8_t old = this->_registers->GetRegm(reg);
	uint8_t value = old - 1;
	this->_registers->SetRegm(reg, value);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, (value & 0xF) > (old & 0xF));
	this->_registers->SetFFlag(FFlag_PV, old == 0x80);
	this->_registers->SetFFlag(FFlag_N, true);
	this->_registers->IncPC();
	this->_cycles += 4;
}

// |1|11| Subtracts one from (hl)
void CPUInst::DECHL() {
	uint8_t old = this->ReadMemory(this->_registers->GetHL());
	uint8_t value = old - 1;
	this->WriteMemory(this->_registers->GetHL(), value);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, (value & 0xF) > (old & 0xF));
	this->_registers->SetFFlag(FFlag_PV, old == 0x80);
	this->_registers->SetFFlag(FFlag_N, true);
	this->_registers->IncPC();
	this->_cycles += 11;
}

// |1|11| Adds one to (hl).
void CPUInst::INCHL() {
	uint8_t old = this->ReadMemory(this->_registers->GetHL());
	uint8_t value = old + 1;
	this->WriteMemory(this->_registers->GetHL(), value);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, (value & 0xF) < (old & 0xF));
	this->_registers->SetFFlag(FFlag_PV, old == 0x7F);
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->IncPC();
	this->_cycles += 11;
}

// |3|23| Adds one to the memory location pointed to by ix plus *.
void CPUInst::INCXXd(uint8_t xx) {
	uint16_t pc = this->_registers->GetPC();
	uint16_t offset = this->_registers->GetRegss(xx) + this->ReadMemory(pc + 2);
	uint8_t old = this->ReadMemory(offset);
	uint8_t value = old + 1;
	this->WriteMemory(offset, value);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, (value & 0xF) < (old & 0xF));
	this->_registers->SetFFlag(FFlag_PV, old == 0x7F);
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->IncPC(3);
	this->_cycles += 23;
}

// |3|23| Subtracts one from the memory location pointed to by ix plus *.
void CPUInst::DECXXd(uint8_t xx) {
	uint16_t pc = this->_registers->GetPC();
	uint16_t offset = this->_registers->GetRegss(xx) + this->ReadMemory(pc + 2);
	uint8_t old = this->ReadMemory(offset);
	uint8_t value = old - 1;
	this->WriteMemory(offset, value);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, (value & 0xF) > (old & 0xF));
	this->_registers->SetFFlag(FFlag_PV, old == 0x80);
	this->_registers->SetFFlag(FFlag_N, true);
	this->_registers->IncPC(3);
	this->_cycles += 23;
}

/******************************************************************************/
/************** General-Purpose Arithmetic and CPU Control Group **************/
/******************************************************************************/

// |1|4| Adjusts a for BCD addition and subtraction operations.
void CPUInst::DAA() {
	bool N = this->_registers->GetF() & FFlag_N;
	bool C = this->_registers->GetF() & FFlag_C;
	bool H = this->_registers->GetF() & FFlag_H;
	uint8_t A = this->_registers->GetA();
	uint8_t AH = A >> 4;
	uint8_t AL = A & 0xF;
	uint8_t correction = 0x00;

	do {
		if (!N) {
			if (!C) {
				if (!H) {
					if ((AH <= 0x9) && (AL <= 0x9)) {
						// correction = 0x00;
						// C = false;
						break;
					}
					if ((AH <= 0x8) && (AL >= 0xA)) {
						correction = 0x06;
						// C = false;
						break;
					}
					if ((AH >= 0xA) && (AL <= 0x9)) {
						correction = 0x60;
						C = true;
						break;
					}
					if ((AH >= 0x9) && (AL >= 0xA)) {
						correction = 0x66;
						C = true;
						break;
					}
				} else {
					if ((AH <= 0x9) && (AL <= 0x3)) {
						correction = 0x06;
						// C = false;
						break;
					}
					if ((AH >= 0xA) && (AL <= 0x3)) {
						correction = 0x66;
						C = true;
						break;
					}
				}
			} else {
				if (!H) {
					if ((AH <= 0x2) && (AL <= 0x9)) {
						correction = 0x60;
						// C = true;
						break;
					}
					if ((AH <= 0x2) && (AL >= 0xA)) {
						correction = 0x66;
						// C = true;
						break;
					}
				} else {
					if ((AH <= 0x3) && (AL <= 0x3)) {
						correction = 0x66;
						// C = true;
						break;
					}
				}
			}
		} else {
			if (!C) {
				if (!H) {
					if ((AH <= 0x9) && (AL <= 0x9)) {
						// correction = 0x00;
						// C = false;
						break;
					}
				} else {
					if ((AH <= 0x8) && (AL >= 0x6)) {
						correction = 0xFA;
						// C = false;
						break;
					}
				}
			} else {
				if (!H) {
					if ((AH >= 0x7) && (AL <= 0x9)) {
						correction = 0xA0;
						// C = true;
						break;
					}
				} else {
					if ((AH >= 0x6) && (AL >= 0x6)) {
						correction = 0x9A;
						// C = true;
						break;
					}
				}
			}
		}
	} while (false);

	this->_registers->SetFFlag(FFlag_C, C);
	uint8_t value = A + correction;
	this->_registers->SetA(value);

	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_H, (A xor value) & 0x10);
	this->_registers->SetFFlag(FFlag_PV, PARITYEVEN(value));
	this->_registers->IncPC();
	this->_cycles += 4;
}

// |1|4| The contents of a are inverted (one's complement).
void CPUInst::CPL() {
	this->_registers->SetA(~this->_registers->GetA());
	this->_registers->SetFFlag(FFlag_H, true);
	this->_registers->SetFFlag(FFlag_N, true);
	this->_registers->IncPC();
	this->_cycles += 4;
}

// |2|8| The contents of a are negated (two's complement). Operation is the same as subtracting a from zero.
void CPUInst::NEG() {
	uint8_t old = this->_registers->GetA();
	uint8_t value = (0xFF - old) + 1;
	this->_registers->SetA(value);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_PV, (old & 0xF) != (value & 0xF));
	this->_registers->SetFFlag(FFlag_PV, old == 0x80);
	this->_registers->SetFFlag(FFlag_N, true);
	this->_registers->SetFFlag(FFlag_C, old != 0);
	this->_registers->IncPC(2);
	this->_cycles += 8;
}

// |1|4| Inverts the carry flag.
void CPUInst::CCF() {
	this->_registers->SetFFlag(FFlag_H, this->_registers->GetF() & FFlag_C);
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, !(this->_registers->GetF() & FFlag_C));
	this->_registers->IncPC();
	this->_cycles += 4;
}

// |1|4| Sets the carry flag.
void CPUInst::SCF() {
	this->_registers->SetFFlag(FFlag_H, false);
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, true);
	this->_registers->IncPC();
	this->_cycles += 4;
}

/******************************************************************************/
/*************************** 16-Bit Arithmetic Group **************************/
/******************************************************************************/

// |1|11| The value of reg is added to hl.
void CPUInst::ADDHLss(uint8_t reg) {
	uint16_t old = this->_registers->GetHL();
	uint16_t value = old + this->_registers->GetRegss(reg);
	this->_registers->SetHL(value);
	this->_registers->SetFFlag(FFlag_H, (value & 0xFFF) < (old & 0xFFF));
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, value < old);
	this->_registers->IncPC();
	this->_cycles += 11;
}

// |2|15| Adds ss and the carry flag to hl.
void CPUInst::ADCHLss(uint8_t reg) {
	uint16_t old = this->_registers->GetHL();
	uint16_t b = this->_registers->GetRegss(reg);
	uint16_t value = old + b;
	int32_t pvalue = ((int16_t) old) + ((int16_t) b);
	if (this->_registers->GetF() & FFlag_C) {
		value++;
		pvalue++;
	}

	this->_registers->SetHL(value);
	this->_registers->SetFFlag(FFlag_S, value & 0x8000);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, (value & 0xFFF) < (old & 0xFFF));
	this->_registers->SetFFlag(FFlag_PV, pvalue > 32767 || pvalue < -32768);
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, value < old);
	this->_registers->IncPC(2);
	this->_cycles += 15;
}

// |2|15| Subtracts reg and the carry flag from hl.
void CPUInst::SBCHLss(uint8_t reg) {
	uint16_t old = this->_registers->GetHL();
	uint16_t b = this->_registers->GetRegss(reg);
	uint16_t value = old - b;
	int32_t pvalue = ((int16_t) old) - ((int16_t) b);

	if (this->_registers->GetF() & FFlag_C) {
		value--;
		pvalue--;
	}

	this->_registers->SetHL(value);
	this->_registers->SetFFlag(FFlag_S, value & 0x8000);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, (value & 0xFFF) > (old & 0xFFF));
	this->_registers->SetFFlag(FFlag_PV, pvalue > 32767 || pvalue < -32768);
	this->_registers->SetFFlag(FFlag_N, true);
	this->_registers->SetFFlag(FFlag_C, value > old);
	this->_registers->IncPC(2);
	this->_cycles += 15;
}

// |2|15| The value of pp is added to XX.
void CPUInst::ADDXXpp(uint8_t XX, uint8_t pp) {
	uint16_t old = this->_registers->GetRegss(XX);
	uint16_t value = old + this->_registers->GetRegss(pp);
	this->_registers->SetRegss(XX, value);
	this->_registers->SetFFlag(FFlag_H, (value & 0xFFF) < (old & 0xFFF));
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, value < old);
	this->_registers->IncPC(2);
	this->_cycles += 15;
}

// |1|6| Adds one to reg
void CPUInst::INCss(uint8_t reg, uint8_t cycles, uint8_t size) {
	this->_registers->SetRegss(reg, this->_registers->GetRegss(reg) + 1);
	this->_registers->IncPC(size);
	this->_cycles += cycles;
}

// |1|6| Subtracts one from ss
void CPUInst::DECss(uint8_t reg) {
	this->_registers->SetRegss(reg, this->_registers->GetRegss(reg)  - 1);
	this->_registers->IncPC();
	this->_cycles += 6;
}

/******************************************************************************/
/*************************** Rotate and Shift Group ***************************/
/******************************************************************************/

// |1|4| The contents of a are rotated left one bit position. The contents of bit 7 are copied to the carry flag and bit 0.
void CPUInst::RLCA() {
	uint8_t old = this->_registers->GetA();
	this->_registers->SetFFlag(FFlag_C, old & 0x80);
	uint8_t value = (old << 1) | bool(this->_registers->GetF() & FFlag_C);
	this->_registers->SetA(value);
	this->_registers->SetFFlag(FFlag_H, false);
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, old & 0x80);
	this->_registers->IncPC();
	this->_cycles += 4;
}

// |1|4| The contents of a are rotated left one bit position. The contents of bit 7 are copied to the carry flag and the previous contents of the carry flag are copied to bit 0.
void CPUInst::RLA() {
	uint8_t old = this->_registers->GetA();
	uint8_t value = (old << 1) | bool(this->_registers->GetF() & FFlag_C);
	this->_registers->SetA(value);
	this->_registers->SetFFlag(FFlag_H, false);
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, old & 0x80);
	this->_registers->IncPC();
	this->_cycles += 4;
}

// |1|4| The contents of a are rotated right one bit position. The contents of bit 0 are copied to the carry flag and bit 7.
void CPUInst::RRCA() {
	uint8_t old = this->_registers->GetA();
	this->_registers->SetFFlag(FFlag_C, old & 0x01);
	uint8_t value = (old >> 1) | (0x80 & bool(this->_registers->GetF() & FFlag_C));
	this->_registers->SetA(value);
	this->_registers->SetFFlag(FFlag_H, false);
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, old & 0x01);
	this->_registers->IncPC();
	this->_cycles += 4;
}

// |1|4| The contents of a are rotated right one bit position. The contents of bit 0 are copied to the carry flag and the previous contents of the carry flag are copied to bit 7.
void CPUInst::RRA() {
	uint8_t old = this->_registers->GetA();
	uint8_t value = (old >> 1) | (0x80 & bool(this->_registers->GetF() & FFlag_C));
	this->_registers->SetA(value);
	this->_registers->SetFFlag(FFlag_H, false);
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, old & 0x01);
	this->_registers->IncPC();
	this->_cycles += 4;
}

// |2|8| The contents of b are rotated left one bit position. The contents of bit 7 are copied to the carry flag and the previous contents of the carry flag are copied to bit 0.
void CPUInst::RL(uint8_t reg) {
	uint8_t old = this->_registers->GetRegm(reg);
	uint8_t value = (old << 1) | bool(this->_registers->GetF() & FFlag_C);
	this->_registers->SetRegm(reg, value);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, false);
	this->_registers->SetFFlag(FFlag_PV, PARITYEVEN(value));
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, old & 0x80);
	this->_registers->IncPC(2);
	this->_cycles += 8;
}

// |2|8| The contents of d are rotated right one bit position. The contents of bit 0 are copied to the carry flag and the previous contents of the carry flag are copied to bit 7.
void CPUInst::RR(uint8_t reg) {
	uint8_t old = this->_registers->GetRegm(reg);
	uint8_t value = (old >> 1) | (bool(this->_registers->GetF() & FFlag_C) * 0x80);
	this->_registers->SetRegm(reg, value);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, false);
	this->_registers->SetFFlag(FFlag_PV, PARITYEVEN(value));
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, old & 0x01);
	this->_registers->IncPC(2);
	this->_cycles += 2;
}

// |2|8| The contents of b are shifted left one bit position. The contents of bit 7 are copied to the carry flag and a zero is put into bit 0.
void CPUInst::SLA(uint8_t reg) {
	uint8_t old = this->_registers->GetRegm(reg);
	uint8_t value = old << 1;
	this->_registers->SetRegm(reg, value);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, false);
	this->_registers->SetFFlag(FFlag_PV, PARITYEVEN(value));
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, old & 0x80);
	this->_registers->IncPC(2);
	this->_cycles += 8;
}

// |2|8| The contents of a are shifted right one bit position. The contents of bit 0 are copied to the carry flag and the previous contents of bit 7 are unchanged.
void CPUInst::SRA(uint8_t reg) {
	uint8_t old = this->_registers->GetRegm(reg);
	uint8_t value =  (old & 0x80) | (old >> 1);
	this->_registers->SetRegm(reg, value);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, false);
	this->_registers->SetFFlag(FFlag_PV, PARITYEVEN(value));
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, old & 0x01);
	this->_registers->IncPC(2);
	this->_cycles += 8;
}

void CPUInst::SRL(uint8_t reg) {
	uint8_t old = this->_registers->GetRegm(reg);
	uint8_t value =  old >> 1;
	this->_registers->SetRegm(reg, value);
	this->_registers->SetFFlag(FFlag_S, false);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, false);
	this->_registers->SetFFlag(FFlag_PV, PARITYEVEN(value));
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, old & 0x01);
	this->_registers->IncPC(2);
	this->_cycles += 8;
}

/******************************************************************************/
/*********************** Bit Set, Reset, and Test Group ***********************/
/******************************************************************************/

// |2|8| Tests bit compare of value.
void CPUInst::BIT(uint8_t value, uint8_t compare, uint8_t cycles) {
	this->_registers->SetFFlag(FFlag_Z, !(value & compare));
	this->_registers->SetFFlag(FFlag_H, true);
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->IncPC(2);
	this->_cycles += cycles;
//	printf("%.2x: %.2x\n", value, this->_registers->GetF());
}

// |2|8| Sets bit X of reg.
void CPUInst::SET(uint8_t reg, uint8_t bit) {
	this->_registers->SetRegm(reg, this->_registers->GetRegm(reg) | bit);
	this->_registers->IncPC(2);
	this->_cycles += 8;
}

// |2|15| Sets bit X of (HL).
void CPUInst::SETHL(uint8_t bit) {
	uint16_t offset = this->_registers->GetHL();
	this->WriteMemory(offset, this->ReadMemory(offset) | bit);
	this->_registers->IncPC(2);
	this->_cycles += 15;
}

// |2|8| Resets bit X of reg.
void CPUInst::RES(uint8_t reg, uint8_t bit) {
	this->_registers->SetRegm(reg, this->_registers->GetRegm(reg) & ~bit);
	this->_registers->IncPC(2);
	this->_cycles += 8;
}

// |2|15| Resets bit X of (HL).
void CPUInst::RESHL(uint8_t bit) {
	uint16_t offset = this->_registers->GetHL();
	this->WriteMemory(offset, this->ReadMemory(offset) & ~bit);
	this->_registers->IncPC(2);
	this->_cycles += 15;
}

// |4|20| Tests bit 'bit' of the memory location pointed to by ss plus *.
void CPUInst::BITbssd(uint8_t bit, uint8_t reg, uint8_t d) {
	uint16_t offset = this->_registers->GetRegss(reg) + d;
	uint8_t value = this->ReadMemory(offset);
	this->_registers->SetFFlag(FFlag_Z, !(value & bit));
	this->_registers->SetFFlag(FFlag_H, true);
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->IncPC(4);
	this->_cycles += 20;
}

// |4|23| Resets bit 'bit' of the memory location pointed to by ss plus *.
void CPUInst::RESETbssd(uint8_t bit, uint8_t reg, uint8_t d) {
	uint16_t offset = this->_registers->GetRegss(reg) + d;
	this->WriteMemory(offset, this->ReadMemory(offset) & ~bit);
	this->_registers->IncPC(4);
	this->_cycles += 23;
}

// |4|23| Sets bit 'bit' of the memory location pointed to by ss plus *.
void CPUInst::SETbssd(uint8_t bit, uint8_t reg, uint8_t d) {
	uint16_t offset = this->_registers->GetRegss(reg) + d;
	this->WriteMemory(offset, this->ReadMemory(offset) | bit);
	this->_registers->IncPC(4);
	this->_cycles += 23;
}

/******************************************************************************/
/********************************* Jump Group *********************************/
/******************************************************************************/

// |3|10| If condition cc is true, ** is copied to pc.
void CPUInst::JPccnn(bool cc) {
	if (cc) {
		uint16_t pc = this->_registers->GetPC();
		this->_registers->SetPC((this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1));
	} else
		this->_registers->IncPC(3);

	this->_cycles += 10;
}

// |2|12/7| If condition cc is true, the signed value * is added to pc.
// The jump is measured from the start of the instruction opcode.
void CPUInst::JR(bool cc) {
	int8_t value = this->ReadMemory(this->_registers->GetPC() + 1);
	if (cc) {
		this->_registers->IncPC(value + 2);
		this->_cycles += 12;
	} else {
		this->_registers->IncPC(2);
		this->_cycles += 7;
	}
}

/******************************************************************************/
/*************************** Call And Return Group ****************************/
/******************************************************************************/

// |1|11/5| If condition cc is true, the top stack entry is popped into pc.
void CPUInst::RET(bool cc, uint8_t cycles) {
	if (cc) {
		uint16_t sp = this->_registers->GetSP();
		uint16_t pc = this->ReadMemory(sp);
		pc |= (this->ReadMemory(sp + 1) << 8);
		this->_registers->SetSP(sp + 2);
		this->_registers->SetPC(pc);
		this->_cycles += cycles;
		this->_registers->SetInterruptsEnabled(true);
	} else {
		this->_registers->IncPC();
		this->_cycles += 5;
	}
}

// |1|11| The current pc value plus one is pushed onto the stack, then is loaded with ph.
void CPUInst::RSTp(uint8_t p) {
	uint16_t pc = this->_registers->GetPC() + 1;
	uint16_t sp = this->_registers->GetSP() - 2;
	this->_registers->SetSP(sp);
	this->WriteMemory(sp, pc & 0xFF);
	this->WriteMemory(sp + 1, (pc >> 8) & 0xFF);
	this->_cycles += 11;
	this->_registers->SetPC(p);
}

// |3|17| The current pc value plus three is pushed onto the stack, then is loaded with **.
void CPUInst::CALLnn() {
	uint16_t pc = this->_registers->GetPC() + 3;
	uint16_t sp = this->_registers->GetSP() - 2;
	this->_registers->SetSP(sp);
	this->WriteMemory(sp, pc & 0xFF);
	this->WriteMemory(sp + 1, (pc >> 8) & 0xFF);
	this->_cycles += 17;
	this->_registers->SetPC((this->ReadMemory(pc - 1) << 8) | this->ReadMemory(pc - 2));
}

// |3|17/10|If condition cc is true, the current pc value plus three is pushed onto the stack, then is loaded with **.
void CPUInst::CALLccnn(bool cc) {
	if (cc)
		CALLnn();
	else {
		this->_registers->IncPC(3);
		this->_cycles += 10;
	}
}

void CPUInst::CallInterrupt() {
	this->_registers->SetInterruptsEnabled(false);
	uint16_t pc = this->_registers->GetPC();
	uint16_t sp = this->_registers->GetSP() - 2;
	this->_registers->SetSP(sp);
	this->WriteMemory(sp, pc & 0xFF);
	this->WriteMemory(sp + 1, (pc >> 8) & 0xFF);
	this->_cycles += 11;
	this->_registers->SetPC(0x0038);
}

void CPUInst::RETI() {
	uint16_t sp = this->_registers->GetSP();
	uint16_t pc = this->ReadMemory(sp);
	pc |= (this->ReadMemory(sp + 1) << 8);
	this->_registers->SetSP(sp + 2);
	this->_registers->SetPC(pc);
	this->_cycles += 10;
	this->_registers->SetInterruptsEnabled(true);
}

/******************************************************************************/
/*************************** Input and Output Group ***************************/
/******************************************************************************/

// |2|11| The value of a is written to port *.
void CPUInst::OUTnA() {
	uint8_t port = this->ReadMemory(this->_registers->GetPC() + 1);
	uint8_t data = this->_registers->GetA();
	this->_addressBus._l = port;
	this->_addressBus._h = data;
	// printf("Address: %.4X\n", this->_addressBus._w);
	this->_ports->WriteByte(port, data);
	this->_registers->IncPC(2);
	this->_cycles += 11;
}

// |2|12| The value of reg is written to port c.
void CPUInst::OUTCr(uint8_t reg) {
	uint8_t C = this->_registers->GetC();
	uint8_t B = this->_registers->GetB();
	uint8_t r = this->_registers->GetRegm(reg);
	this->_addressBus._l = C;
	this->_addressBus._h = B;
	// printf("Address: %.4X\n", this->_addressBus._w);
	this->_ports->WriteByte(C, r);
	this->_registers->IncPC(2);
	this->_cycles += 12;
}

// |2|12| Outputs a zero to port c.
void CPUInst::OUTC() {
	uint8_t C = this->_registers->GetC();
/*
	uint8_t C = this->_registers->GetC();
	uint8_t B = this->_registers->GetB();
	this->_addressBus._l = C;
	this->_addressBus._h = B;
	// printf("Address: %.4X\n", this->_addressBus._w);
*/
	this->_ports->WriteByte(C, 0);
	this->_registers->IncPC(2);
	this->_cycles += 12;
}

// |2|16| A byte from the memory location pointed to by hl is written to port c. Then hl is incremented and b is decremented.
void CPUInst::OUTI() {
	uint8_t C = this->_registers->GetC();
	uint8_t B = this->_registers->GetB();
	uint16_t HL = this->_registers->GetHL();
	uint8_t value = this->ReadMemory(HL);
	this->_ports->WriteByte(C, value);
	this->_registers->SetHL(HL + 1);
	this->_registers->SetB(B - 1);
	this->_registers->SetFFlag(FFlag_N, true);
	this->_registers->SetFFlag(FFlag_Z, ((B - 1) == 0));

	this->_registers->IncPC(2);
	this->_cycles += 16;
}