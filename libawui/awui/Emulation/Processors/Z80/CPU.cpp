/**
 * awui/Emulation/Processors/Z80/CPU.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "CPU.h"

// #define SLOW
#include <string>
// #include <string.h>
// #define NUMOPCODES

using namespace awui::Emulation;
using namespace awui::Emulation::Processors::Z80;

#ifdef NUMOPCODES
static bool opcodes[OxNOTIMPLEMENTED];
static uint64_t opcodesT[OxNOTIMPLEMENTED];
#endif

CPU::CPU() {
	m_saveData.addressBus.W = 0;

	m_showLog = false;
	m_showNotImplemented = true;

	m_saveData.inInterrupt = false;
	m_saveData.isSuspended = false;
	m_saveData.isEndlessLoop = false;
	m_saveData.afterEI = false;

#ifdef NUMOPCODES
	for (int i = 0; i < OxNOTIMPLEMENTED; i++) {
		opcodes[i] = false;
		opcodesT[i] = 0;
	}
#endif

	Reset();
}

CPU::~CPU() {
}

void CPU::Reset() {
	CPUInst::Reset();
}

void CPU::RunOpcode() {
	int64_t tStatesOld = m_saveData.cycles;
	m_saveData.afterEI = false;

	uint16_t pc = m_saveData.registers.GetPC();

	uint8_t opcode1 = ReadMemory(pc);
	uint8_t opcode2;

	m_opcode.SetByte1(opcode1);

#ifdef SLOW
	char logLine[255];
	char logCode[255];
	char logAux[255];

	sprintf(logLine, "%.4X", pc);
	sprintf(logCode, "%.2X ", opcode1);
#endif

	if ((opcode1 == 0xCB) || (opcode1 == 0xDD) || (opcode1 == 0xED) || (opcode1 == 0xFD)) {
		opcode2 = ReadMemory(pc + 1);
		m_opcode.SetByte2(opcode2);

#ifdef SLOW
		sprintf(logAux, "%.2X ", opcode2);
		strcat(logCode, logAux);
#endif

		if (((opcode1 == 0xDD) || (opcode1 == 0xFD)) && (opcode2 == 0xCB)) {
			uint8_t opcode4 = ReadMemory(pc + 3);
			m_opcode.SetByte4(opcode4);

#ifdef SLOW
			sprintf(logAux, "%.2X ", opcode4);
			strcat(logCode, logAux);
		} else {
			sprintf(logAux, "   ");
			strcat(logCode, logAux);
#endif
		}
	} else {
#ifdef SLOW
		sprintf(logAux, "      ");
		strcat(logCode, logAux);
#endif
	}

	uint16_t opcodeEnum = m_opcode.Decode();
	uint8_t advance = m_opcode.GetAdvance();
	if (advance != 0) {
		// Prefijo DD/FD sin efecto: solo consume su ciclo M1 (4 ciclos). Hay que deshacer
		// lo que han sumado las lecturas de la instrucción hechas arriba
		m_saveData.cycles = tStatesOld + 4;
		m_saveData.registers.IncPC(advance);
		return;
	}

#ifdef SLOW
	if (m_showLog) {
#ifdef NUMOPCODES
		opcodes[opcodeEnum] = true;
#endif
		printf("\n");
		printf("%s: ", logLine);
		printf("%s", logCode);
		m_opcode.ShowLogOpcode(this, opcodeEnum);
		/*
				printf(" ");
				printf("\n");
				printf("AF: %.4X  ", m_saveData.registers.GetAF());
				printf("BC: %.4X  ", m_saveData.registers.GetBC());
				printf("DE: %.4X  ", m_saveData.registers.GetDE());
				printf("HL: %.4X  ", m_saveData.registers.GetHL());
				printf("IX: %.4X  ", m_saveData.registers.GetIX());
				printf("IY: %.4X  ", m_saveData.registers.GetIY());
				printf("\n");
				printf("PC: %.4X  ", pc);
				printf("SP: %.4X  ", m_saveData.registers.GetSP());
				printf("\n");
				printf("\n");
		*/
		fflush(stdout);
	}
#endif

#ifdef NUMOPCODES
	opcodesT[opcodeEnum]++;
#endif

	m_saveData.cycles = tStatesOld;

	// http://clrhome.org/table/
	m_saveData.isEndlessLoop = false;
	switch (opcodeEnum) {

			/******************************************************************************/
			/***************************** Main instructions ******************************/
			/******************************************************************************/

		// 00: NOP
		// |1|4| No operation is performed.
		case Ox00:
			m_saveData.cycles += 4;
			m_saveData.registers.IncPC();
			break;

		// 76: HALT
		// |1|4| Suspends CPU operation until an interrupt or reset occurs.
		case Ox76:
			m_saveData.cycles += 4;
			m_saveData.isSuspended = true;
			break;

		// LD dd, nn
		case Ox01:
			LDddnn(Reg_BC);
			break;
		case Ox11:
			LDddnn(Reg_DE);
			break;
		case Ox21:
			LDddnn(Reg_HL);
			break;
		case Ox31:
			LDddnn(Reg_SP);
			break;

		// INC ss
		case Ox03:
			INCss(Reg_BC, 6, 1);
			break;
		case Ox13:
			INCss(Reg_DE, 6, 1);
			break;
		case Ox23:
			INCss(Reg_HL, 6, 1);
			break;
		case Ox33:
			INCss(Reg_SP, 6, 1);
			break;
		case Ox34:
			INCHL();
			break;

		// INC r
		case Ox04:
			INCr(Reg_B, 4, 1);
			break;
		case Ox0C:
			INCr(Reg_C, 4, 1);
			break;
		case Ox14:
			INCr(Reg_D, 4, 1);
			break;
		case Ox1C:
			INCr(Reg_E, 4, 1);
			break;
		case Ox24:
			INCr(Reg_H, 4, 1);
			break;
		case Ox2C:
			INCr(Reg_L, 4, 1);
			break;
		case Ox3C:
			INCr(Reg_A, 4, 1);
			break;

		// ADD HL, s
		case Ox09:
			ADDXXpp(Reg_HL, m_saveData.registers.GetBC(), 11, 1);
			break;
		case Ox19:
			ADDXXpp(Reg_HL, m_saveData.registers.GetDE(), 11, 1);
			break;
		case Ox29:
			ADDXXpp(Reg_HL, m_saveData.registers.GetHL(), 11, 1);
			break;
		case Ox39:
			ADDXXpp(Reg_HL, m_saveData.registers.GetSP(), 11, 1);
			break;

		case Ox07:
			RLCA();
			break;
		case Ox0F:
			RRCA();
			break;
		case Ox17:
			RLA();
			break;
		case Ox1F:
			RRA();
			break;
		case Ox27:
			DAA();
			break;
		case Ox2F:
			CPL();
			break;
		case Ox37:
			SCF();
			break;
		case Ox3F:
			CCF();
			break;

		// DJNZ n -> pc:4,ir:1,pc+1:3,[pc+1:1 x 5]
		// |2|13/8| The b register is decremented, and if not zero, the signed value * is added to pc. The jump is measured from the start of the instruction opcode.
		case Ox10: {
			m_saveData.cycles++;
			uint8_t bDec = m_saveData.registers.GetB() - 1;
			m_saveData.registers.SetB(bDec);
			JR(bDec != 0);
		} break;

		case Ox22:
			LDHLdd();
			break;

		// 36: LD (HL), *
		// |2|10| Loads * into (hl).
		case Ox36:
			m_saveData.cycles += 4;
			WriteMemory(m_saveData.registers.GetHL(), ReadMemory(pc + 1));
			m_saveData.registers.IncPC(2);
			break;

		// DEC X
		case Ox0B:
			DECss(Reg_BC, 6, 1);
			break;
		case Ox1B:
			DECss(Reg_DE, 6, 1);
			break;
		case Ox2B:
			DECss(Reg_HL, 6, 1);
			break;
		case Ox3B:
			DECss(Reg_SP, 6, 1);
			break;

		case Ox05:
			DECm(Reg_B, 4, 1);
			break;
		case Ox0D:
			DECm(Reg_C, 4, 1);
			break;
		case Ox15:
			DECm(Reg_D, 4, 1);
			break;
		case Ox1D:
			DECm(Reg_E, 4, 1);
			break;
		case Ox25:
			DECm(Reg_H, 4, 1);
			break;
		case Ox2D:
			DECm(Reg_L, 4, 1);
			break;
		case Ox35:
			DECHL();
			break;
		case Ox3D:
			DECm(Reg_A, 4, 1);
			break;

		case Ox40:
			LDrr(Reg_B, m_saveData.registers.GetB(), 4, 1);
			break;
		case Ox41:
			LDrr(Reg_B, m_saveData.registers.GetC(), 4, 1);
			break;
		case Ox42:
			LDrr(Reg_B, m_saveData.registers.GetD(), 4, 1);
			break;
		case Ox43:
			LDrr(Reg_B, m_saveData.registers.GetE(), 4, 1);
			break;
		case Ox44:
			LDrr(Reg_B, m_saveData.registers.GetH(), 4, 1);
			break;
		case Ox45:
			LDrr(Reg_B, m_saveData.registers.GetL(), 4, 1);
			break;
		case Ox47:
			LDrr(Reg_B, m_saveData.registers.GetA(), 4, 1);
			break;
		case Ox48:
			LDrr(Reg_C, m_saveData.registers.GetB(), 4, 1);
			break;
		case Ox49:
			LDrr(Reg_C, m_saveData.registers.GetC(), 4, 1);
			break;
		case Ox4A:
			LDrr(Reg_C, m_saveData.registers.GetD(), 4, 1);
			break;
		case Ox4B:
			LDrr(Reg_C, m_saveData.registers.GetE(), 4, 1);
			break;
		case Ox4C:
			LDrr(Reg_C, m_saveData.registers.GetH(), 4, 1);
			break;
		case Ox4D:
			LDrr(Reg_C, m_saveData.registers.GetL(), 4, 1);
			break;
		case Ox4F:
			LDrr(Reg_C, m_saveData.registers.GetA(), 4, 1);
			break;
		case Ox50:
			LDrr(Reg_D, m_saveData.registers.GetB(), 4, 1);
			break;
		case Ox51:
			LDrr(Reg_D, m_saveData.registers.GetC(), 4, 1);
			break;
		case Ox52:
			LDrr(Reg_D, m_saveData.registers.GetD(), 4, 1);
			break;
		case Ox53:
			LDrr(Reg_D, m_saveData.registers.GetE(), 4, 1);
			break;
		case Ox54:
			LDrr(Reg_D, m_saveData.registers.GetH(), 4, 1);
			break;
		case Ox55:
			LDrr(Reg_D, m_saveData.registers.GetL(), 4, 1);
			break;
		case Ox57:
			LDrr(Reg_D, m_saveData.registers.GetA(), 4, 1);
			break;
		case Ox58:
			LDrr(Reg_E, m_saveData.registers.GetB(), 4, 1);
			break;
		case Ox59:
			LDrr(Reg_E, m_saveData.registers.GetC(), 4, 1);
			break;
		case Ox5A:
			LDrr(Reg_E, m_saveData.registers.GetD(), 4, 1);
			break;
		case Ox5B:
			LDrr(Reg_E, m_saveData.registers.GetE(), 4, 1);
			break;
		case Ox5C:
			LDrr(Reg_E, m_saveData.registers.GetH(), 4, 1);
			break;
		case Ox5D:
			LDrr(Reg_E, m_saveData.registers.GetL(), 4, 1);
			break;
		case Ox5F:
			LDrr(Reg_E, m_saveData.registers.GetA(), 4, 1);
			break;
		case Ox60:
			LDrr(Reg_H, m_saveData.registers.GetB(), 4, 1);
			break;
		case Ox61:
			LDrr(Reg_H, m_saveData.registers.GetC(), 4, 1);
			break;
		case Ox62:
			LDrr(Reg_H, m_saveData.registers.GetD(), 4, 1);
			break;
		case Ox63:
			LDrr(Reg_H, m_saveData.registers.GetE(), 4, 1);
			break;
		case Ox64:
			LDrr(Reg_H, m_saveData.registers.GetH(), 4, 1);
			break;
		case Ox65:
			LDrr(Reg_H, m_saveData.registers.GetL(), 4, 1);
			break;
		case Ox67:
			LDrr(Reg_H, m_saveData.registers.GetA(), 4, 1);
			break;
		case Ox68:
			LDrr(Reg_L, m_saveData.registers.GetB(), 4, 1);
			break;
		case Ox69:
			LDrr(Reg_L, m_saveData.registers.GetC(), 4, 1);
			break;
		case Ox6A:
			LDrr(Reg_L, m_saveData.registers.GetD(), 4, 1);
			break;
		case Ox6B:
			LDrr(Reg_L, m_saveData.registers.GetE(), 4, 1);
			break;
		case Ox6C:
			LDrr(Reg_L, m_saveData.registers.GetH(), 4, 1);
			break;
		case Ox6D:
			LDrr(Reg_L, m_saveData.registers.GetL(), 4, 1);
			break;
		case Ox6F:
			LDrr(Reg_L, m_saveData.registers.GetA(), 4, 1);
			break;
		case Ox78:
			LDrr(Reg_A, m_saveData.registers.GetB(), 4, 1);
			break;
		case Ox79:
			LDrr(Reg_A, m_saveData.registers.GetC(), 4, 1);
			break;
		case Ox7A:
			LDrr(Reg_A, m_saveData.registers.GetD(), 4, 1);
			break;
		case Ox7B:
			LDrr(Reg_A, m_saveData.registers.GetE(), 4, 1);
			break;
		case Ox7C:
			LDrr(Reg_A, m_saveData.registers.GetH(), 4, 1);
			break;
		case Ox7D:
			LDrr(Reg_A, m_saveData.registers.GetL(), 4, 1);
			break;
		case Ox7F:
			LDrr(Reg_A, m_saveData.registers.GetA(), 4, 1);
			break;

		// LD r, (HL)
		case Ox46:
			LDrHL(Reg_B);
			break;
		case Ox4E:
			LDrHL(Reg_C);
			break;
		case Ox56:
			LDrHL(Reg_D);
			break;
		case Ox5E:
			LDrHL(Reg_E);
			break;
		case Ox66:
			LDrHL(Reg_H);
			break;
		case Ox6E:
			LDrHL(Reg_L);
			break;
		case Ox7E:
			LDrHL(Reg_A);
			break;

		case OxF9:
			LDtofrom(Reg_SP, m_saveData.registers.GetHL(), 6, 1);
			break;
		case OxE9:
			LDtofrom(Reg_PC, m_saveData.registers.GetHL(), 4, 1);
			break;
		case OxDDE9:
			LDtofrom(Reg_PC, m_saveData.registers.GetIX(), 8, 1);
			break;
		case OxFDE9:
			LDtofrom(Reg_PC, m_saveData.registers.GetIY(), 8, 1);
			break;
		case OxDDF9:
			LDtofrom(Reg_SP, m_saveData.registers.GetIX(), 10, 2);
			break;
		case OxFDF9:
			LDtofrom(Reg_SP, m_saveData.registers.GetIY(), 10, 2);
			break;

		// 28 nn: JR X, *
		// |2|12/7| If condition X is true, the signed value * is added to pc. The jump is measured from the start of the instruction opcode.
		case Ox18:
			JR(true);
			break;
		case Ox20:
			JR(!(m_saveData.registers.GetF() & Flag_Z));
			break;
		case Ox28:
			JR(m_saveData.registers.GetF() & Flag_Z);
			break;
		case Ox30:
			JR(!(m_saveData.registers.GetF() & Flag_C));
			break;
		case Ox38:
			JR(m_saveData.registers.GetF() & Flag_C);
			break;

		// 32 nn: LD **, A
		// |3|13| Stores a into the memory location pointed to by **.
		case Ox32: {
			m_saveData.cycles += 4;
			Word offset;
			offset.H = ReadMemory(pc + 2);
			offset.L = ReadMemory(pc + 1);
			WriteMemory(offset.W, m_saveData.registers.GetA());
			m_saveData.registers.IncPC(3);
		} break;

		// 0A: LD A, (BC)
		// |1|7| Loads the value pointed to by bc into a.
		case Ox0A:
			m_saveData.cycles += 4;
			m_saveData.registers.IncPC();
			m_saveData.registers.SetA(ReadMemory(m_saveData.registers.GetBC()));
			break;

		// 1A: LD A, (DE)
		// |1|7| Loads the value pointed to by de into a.
		case Ox1A:
			m_saveData.cycles += 4;
			m_saveData.registers.IncPC();
			m_saveData.registers.SetA(ReadMemory(m_saveData.registers.GetDE()));
			break;

		// 3A: LD A, (**)
		// |3|13| Loads the value pointed to by ** into a.
		case Ox3A: {
			m_saveData.cycles += 4;
			Word offset;
			offset.H = ReadMemory(pc + 2);
			offset.L = ReadMemory(pc + 1);
			m_saveData.registers.SetA(ReadMemory(offset.W));
			m_saveData.registers.IncPC(3);
		} break;

		// 2A: LD HL, (**)
		case Ox2A:
			LDHL_nn();
			break;

		// LD r, *
		case Ox06:
			LDrn(Reg_B);
			break;
		case Ox0E:
			LDrn(Reg_C);
			break;
		case Ox16:
			LDrn(Reg_D);
			break;
		case Ox1E:
			LDrn(Reg_E);
			break;
		case Ox26:
			LDrn(Reg_H);
			break;
		case Ox2E:
			LDrn(Reg_L);
			break;
		case Ox3E:
			LDrn(Reg_A);
			break;

		// LD (ss), r
		case Ox02:
			LDssr(m_saveData.registers.GetBC(), m_saveData.registers.GetA());
			break;
		case Ox12:
			LDssr(m_saveData.registers.GetDE(), m_saveData.registers.GetA());
			break;
		case Ox70:
			LDssr(m_saveData.registers.GetHL(), m_saveData.registers.GetB());
			break;
		case Ox71:
			LDssr(m_saveData.registers.GetHL(), m_saveData.registers.GetC());
			break;
		case Ox72:
			LDssr(m_saveData.registers.GetHL(), m_saveData.registers.GetD());
			break;
		case Ox73:
			LDssr(m_saveData.registers.GetHL(), m_saveData.registers.GetE());
			break;
		case Ox74:
			LDssr(m_saveData.registers.GetHL(), m_saveData.registers.GetH());
			break;
		case Ox75:
			LDssr(m_saveData.registers.GetHL(), m_saveData.registers.GetL());
			break;
		case Ox77:
			LDssr(m_saveData.registers.GetHL(), m_saveData.registers.GetA());
			break;

		case Ox80:
			ADD(m_saveData.registers.GetB());
			break;
		case Ox81:
			ADD(m_saveData.registers.GetC());
			break;
		case Ox82:
			ADD(m_saveData.registers.GetD());
			break;
		case Ox83:
			ADD(m_saveData.registers.GetE());
			break;
		case Ox84:
			ADD(m_saveData.registers.GetH());
			break;
		case Ox85:
			ADD(m_saveData.registers.GetL());
			break;
		case Ox86:
			ADD(ReadMemory(m_saveData.registers.GetHL()));
			break;
		case Ox87:
			ADD(m_saveData.registers.GetA());
			break;
		case OxC6:
			ADD(ReadMemory(pc + 1), 4, 2);
			break;

		case Ox88:
			ADC(m_saveData.registers.GetB());
			break;
		case Ox89:
			ADC(m_saveData.registers.GetC());
			break;
		case Ox8A:
			ADC(m_saveData.registers.GetD());
			break;
		case Ox8B:
			ADC(m_saveData.registers.GetE());
			break;
		case Ox8C:
			ADC(m_saveData.registers.GetH());
			break;
		case Ox8D:
			ADC(m_saveData.registers.GetL());
			break;
		case Ox8E:
			ADC(ReadMemory(m_saveData.registers.GetHL()));
			break;
		case Ox8F:
			ADC(m_saveData.registers.GetA());
			break;
		case OxCE:
			ADC(ReadMemory(pc + 1), 4, 2);
			break;

		case Ox90:
			SUB(m_saveData.registers.GetB());
			break;
		case Ox91:
			SUB(m_saveData.registers.GetC());
			break;
		case Ox92:
			SUB(m_saveData.registers.GetD());
			break;
		case Ox93:
			SUB(m_saveData.registers.GetE());
			break;
		case Ox94:
			SUB(m_saveData.registers.GetH());
			break;
		case Ox95:
			SUB(m_saveData.registers.GetL());
			break;
		case Ox96:
			SUB(ReadMemory(m_saveData.registers.GetHL()));
			break;
		case Ox97:
			SUB(m_saveData.registers.GetA());
			break;
		case OxD6:
			SUB(ReadMemory(pc + 1), 4, 2);
			break;

		case Ox98:
			SBC(m_saveData.registers.GetB());
			break;
		case Ox99:
			SBC(m_saveData.registers.GetC());
			break;
		case Ox9A:
			SBC(m_saveData.registers.GetD());
			break;
		case Ox9B:
			SBC(m_saveData.registers.GetE());
			break;
		case Ox9C:
			SBC(m_saveData.registers.GetH());
			break;
		case Ox9D:
			SBC(m_saveData.registers.GetL());
			break;
		case Ox9E:
			SBC(ReadMemory(m_saveData.registers.GetHL()));
			break;
		case Ox9F:
			SBC(m_saveData.registers.GetA());
			break;
		case OxDE:
			SBC(ReadMemory(pc + 1), 4, 2);
			break;

		case OxA0:
			AND(m_saveData.registers.GetB());
			break;
		case OxA1:
			AND(m_saveData.registers.GetC());
			break;
		case OxA2:
			AND(m_saveData.registers.GetD());
			break;
		case OxA3:
			AND(m_saveData.registers.GetE());
			break;
		case OxA4:
			AND(m_saveData.registers.GetH());
			break;
		case OxA5:
			AND(m_saveData.registers.GetL());
			break;
		case OxA6:
			AND(ReadMemory(m_saveData.registers.GetHL()));
			break;
		case OxA7:
			AND(m_saveData.registers.GetA());
			break;
		case OxE6:
			AND(ReadMemory(pc + 1), 4, 2);
			break;

		case OxA8:
			XOR(m_saveData.registers.GetB());
			break;
		case OxA9:
			XOR(m_saveData.registers.GetC());
			break;
		case OxAA:
			XOR(m_saveData.registers.GetD());
			break;
		case OxAB:
			XOR(m_saveData.registers.GetE());
			break;
		case OxAC:
			XOR(m_saveData.registers.GetH());
			break;
		case OxAD:
			XOR(m_saveData.registers.GetL());
			break;
		case OxAE:
			XOR(ReadMemory(m_saveData.registers.GetHL()));
			break;
		case OxAF:
			XOR(m_saveData.registers.GetA());
			break;
		case OxEE:
			XOR(ReadMemory(pc + 1), 4, 2);
			break;

		case OxB0:
			OR(m_saveData.registers.GetB());
			break;
		case OxB1:
			OR(m_saveData.registers.GetC());
			break;
		case OxB2:
			OR(m_saveData.registers.GetD());
			break;
		case OxB3:
			OR(m_saveData.registers.GetE());
			break;
		case OxB4:
			OR(m_saveData.registers.GetH());
			break;
		case OxB5:
			OR(m_saveData.registers.GetL());
			break;
		case OxB6:
			OR(ReadMemory(m_saveData.registers.GetHL()));
			break;
		case OxB7:
			OR(m_saveData.registers.GetA());
			break;
		case OxF6:
			OR(ReadMemory(pc + 1), 4, 2);
			break;

		case OxB8:
			CP(m_saveData.registers.GetB());
			break;
		case OxB9:
			CP(m_saveData.registers.GetC());
			break;
		case OxBA:
			CP(m_saveData.registers.GetD());
			break;
		case OxBB:
			CP(m_saveData.registers.GetE());
			break;
		case OxBC:
			CP(m_saveData.registers.GetH());
			break;
		case OxBD:
			CP(m_saveData.registers.GetL());
			break;
		case OxBE:
			CP(ReadMemory(m_saveData.registers.GetHL()));
			break;
		case OxBF:
			CP(m_saveData.registers.GetA());
			break;
		case OxFE:
			CP(ReadMemory(pc + 1), 4, 2);
			break;

		// JP cc, nn
		case OxC2:
			JPccnn(!(m_saveData.registers.GetF() & Flag_Z));
			break;
		case OxCA:
			JPccnn(m_saveData.registers.GetF() & Flag_Z);
			break;
		case OxD2:
			JPccnn(!(m_saveData.registers.GetF() & Flag_C));
			break;
		case OxDA:
			JPccnn(m_saveData.registers.GetF() & Flag_C);
			break;
		case OxE2:
			JPccnn(!(m_saveData.registers.GetF() & Flag_V));
			break;
		case OxEA:
			JPccnn(m_saveData.registers.GetF() & Flag_V);
			break;
		case OxF2:
			JPccnn(!(m_saveData.registers.GetF() & Flag_S));
			break;
		case OxFA:
			JPccnn(m_saveData.registers.GetF() & Flag_S);
			break;

		// C3 nn: JP **
		// |3|10| ** is copied to pc.
		case OxC3: {
			m_saveData.cycles += 4;
			Word offset;
			offset.H = ReadMemory(pc + 2);
			offset.L = ReadMemory(pc + 1);
			if (offset.W == pc)
				m_saveData.isEndlessLoop = true;
			else
				m_saveData.registers.SetPC(offset.W);
		} break;

		case OxC1:
			POP16(Reg_BC);
			break;
		case OxD1:
			POP16(Reg_DE);
			break;
		case OxE1:
			POP16(Reg_HL);
			break;
		case OxF1:
			POP16(Reg_AF);
			break;

		case OxC5:
			PUSH16(Reg_BC);
			break;
		case OxD5:
			PUSH16(Reg_DE);
			break;
		case OxE5:
			PUSH16(Reg_HL);
			break;
		case OxF5:
			PUSH16(Reg_AF);
			break;

		// C9: RET
		// |1|10| The top stack entry is popped into pc.
		case OxC0:
			RETcc(!(m_saveData.registers.GetF() & Flag_Z));
			break;
		case OxC8:
			RETcc((m_saveData.registers.GetF() & Flag_Z));
			break;
		case OxC9:
			RET();
			break;
		case OxD0:
			RETcc(!(m_saveData.registers.GetF() & Flag_C));
			break;
		case OxD8:
			RETcc((m_saveData.registers.GetF() & Flag_C));
			break;
		case OxE0:
			RETcc(!(m_saveData.registers.GetF() & Flag_V));
			break;
		case OxE8:
			RETcc((m_saveData.registers.GetF() & Flag_V));
			break;
		case OxF0:
			RETcc(!(m_saveData.registers.GetF() & Flag_S));
			break;
		case OxF8:
			RETcc((m_saveData.registers.GetF() & Flag_S));
			break;

		// CD nn: CALL **
		case OxC4:
			CALLccnn(!(m_saveData.registers.GetF() & Flag_Z));
			break;
		case OxCC:
			CALLccnn((m_saveData.registers.GetF() & Flag_Z));
			break;
		case OxCD:
			CALLnn();
			break;
		case OxD4:
			CALLccnn(!(m_saveData.registers.GetF() & Flag_C));
			break;
		case OxDC:
			CALLccnn((m_saveData.registers.GetF() & Flag_C));
			break;
		case OxE4:
			CALLccnn(!(m_saveData.registers.GetF() & Flag_V));
			break;
		case OxEC:
			CALLccnn((m_saveData.registers.GetF() & Flag_V));
			break;
		case OxF4:
			CALLccnn(!(m_saveData.registers.GetF() & Flag_S));
			break;
		case OxFC:
			CALLccnn((m_saveData.registers.GetF() & Flag_S));
			break;

		// RST p
		case OxC7:
			RSTp(0x00);
			break;
		case OxCF:
			RSTp(0x08);
			break;
		case OxD7:
			RSTp(0x10);
			break;
		case OxDF:
			RSTp(0x18);
			break;
		case OxE7:
			RSTp(0x20);
			break;
		case OxEF:
			RSTp(0x28);
			break;
		case OxF7:
			RSTp(0x30);
			break;
		case OxFF:
			RSTp(0x38);
			break;

		// D3 *: OUT (*), A
		case OxD3:
			OUTnA();
			break;

		// D9: EXX
		// |1|4| Exchanges the 16-bit contents of bc, de, and hl with bc', de', and hl'.
		case OxD9:
			m_saveData.cycles += 4;
			m_saveData.registers.AlternateBC();
			m_saveData.registers.AlternateDE();
			m_saveData.registers.AlternateHL();
			m_saveData.registers.IncPC();
			break;

		// DB n: IN A, *
		// |2|11| A byte from port * is written to a.
		case OxDB: {
			m_saveData.cycles += 8;
			uint8_t n = ReadMemory(pc + 1);
			m_saveData.addressBus.L = n;
			m_saveData.addressBus.H = m_saveData.registers.GetA();
			//				printf("IN A, *\n");
			m_saveData.registers.SetA(ReadPort(n));
			//				printf("PORT: %d: %X\n", ReadMemory(pc + 1), ReadMemory(pc + 1));
			m_saveData.registers.IncPC(2);
		} break;

		// 08: EX AF, AF'
		// |1|4| Exchanges the 16-bit contents of af and af'.
		case Ox08: {
			m_saveData.cycles += 4;
			m_saveData.registers.IncPC();
			m_saveData.registers.AlternateAF();
		} break;

		// EB: EX DE, HL
		// |1|4| Exchanges the 16-bit contents of de and hl.
		case OxEB: {
			m_saveData.cycles += 4;
			m_saveData.registers.IncPC();
			uint16_t aux = m_saveData.registers.GetDE();
			m_saveData.registers.SetDE(m_saveData.registers.GetHL());
			m_saveData.registers.SetHL(aux);
		} break;

		case OxE3:
			EX_SPHL();
			break;
		case OxDDE3:
			EX_ssX(Reg_IX);
			break;
		case OxFDE3:
			EX_ssX(Reg_IY);
			break;

		// F3 DI
		// |1|4| Resets both interrupt flip-flops, thus prenting maskable interrupts from triggering.
		// I dont know if is completed
		case OxF3:
			m_saveData.cycles += 4;
			m_saveData.registers.IncPC();
			m_saveData.registers.SetIFF1(false);
			m_saveData.registers.SetIFF2(false);
			break;

		// FB EI
		// |1|4| Sets both interrupt flip-flops, thus allowing maskable interrupts to occur. An interrupt will not occur until after the immediatedly following instruction.
		case OxFB:
			m_saveData.cycles += 4;
			m_saveData.registers.IncPC();
			m_saveData.registers.SetIFF1(true);
			m_saveData.registers.SetIFF2(true);
			m_saveData.afterEI = true;
			break;

			/******************************************************************************/
			/************************ Extended instructions (ED) **************************/
			/******************************************************************************/

		case OxED40:
			INrC(Reg_B);
			break;
		case OxED48:
			INrC(Reg_C);
			break;
		case OxED50:
			INrC(Reg_D);
			break;
		case OxED58:
			INrC(Reg_E);
			break;
		case OxED60:
			INrC(Reg_H);
			break;
		case OxED68:
			INrC(Reg_L);
			break;
		case OxED70:
			INrC(Reg_F);
			break;
		case OxED78:
			INrC(Reg_A);
			break;

		case OxED44:
		case OxED4C:
		case OxED54:
		case OxED5C:
		case OxED64:
		case OxED6C:
		case OxED74:
		case OxED7C:
			NEG();
			break;

		case OxED45:
		case OxED55:
		case OxED5D:
		case OxED65:
		case OxED6D:
		case OxED75:
		case OxED7D:
			RETN();
			break;

		case OxED47:
			LDrr(Reg_I, m_saveData.registers.GetA(), 9, 2);
			break;
		case OxED4F:
			LDrr(Reg_R, m_saveData.registers.GetA(), 9, 2);
			break;
		case OxED57:
			LDAri(m_saveData.registers.GetI());
			break;
		case OxED5F:
			LDAri(m_saveData.registers.GetR());
			break;

		case OxED67:
			RRD();
			break;
		case OxED6F:
			RLD();
			break;

		case OxED41:
			OUTC(m_saveData.registers.GetB());
			break;
		case OxED49:
			OUTC(m_saveData.registers.GetC());
			break;
		case OxED51:
			OUTC(m_saveData.registers.GetD());
			break;
		case OxED59:
			OUTC(m_saveData.registers.GetE());
			break;
		case OxED61:
			OUTC(m_saveData.registers.GetH());
			break;
		case OxED69:
			OUTC(m_saveData.registers.GetL());
			break;
		case OxED71:
			OUTC(0);
			break;
		case OxED79:
			OUTC(m_saveData.registers.GetA());
			break;

		// SBC HL, ss
		case OxED42:
			SBCHLss(Reg_BC);
			break;
		case OxED52:
			SBCHLss(Reg_DE);
			break;
		case OxED62:
			SBCHLss(Reg_HL);
			break;
		case OxED72:
			SBCHLss(Reg_SP);
			break;

		// LD (nn), dd
		case OxED43:
			LDnndd(Reg_BC);
			break;
		case OxED53:
			LDnndd(Reg_DE);
			break;
		case OxED63:
			LDnndd(Reg_HL);
			break;
		case OxED73:
			LDnndd(Reg_SP);
			break;

		// LD dd, (nn)
		case OxED4B:
			LDdd_nn(Reg_BC);
			break;
		case OxED5B:
			LDdd_nn(Reg_DE);
			break;
		case OxED6B:
			LDdd_nn(Reg_HL);
			break;
		case OxED7B:
			LDdd_nn(Reg_SP);
			break;

		case OxED4A:
			ADCHLss(Reg_BC);
			break;
		case OxED5A:
			ADCHLss(Reg_DE);
			break;
		case OxED6A:
			ADCHLss(Reg_HL);
			break;
		case OxED7A:
			ADCHLss(Reg_SP);
			break;

		// ED46: IM 0
		// ED66: IM 0
		// |2|8| Sets interrupt mode 0.
		case OxED46:
		case OxED66:
			// 			printf("IM 0\n");
			m_saveData.cycles += 8;
			m_saveData.registers.IncPC(2);
			m_saveData.registers.SetIM(0);
			break;

		// ED56: IM 1
		// ED76: IM 1
		// |2|8| Sets interrupt mode 1.
		case OxED56:
		case OxED76:
			// 			printf("IM 1\n");
			m_saveData.cycles += 8;
			m_saveData.registers.IncPC(2);
			m_saveData.registers.SetIM(1);
			break;

		// ED5E: IM 2
		// ED7E: IM 2
		// |2|8| Sets interrupt mode 1.
		case OxED5E:
		case OxED7E:
			// 			printf("IM 2\n");
			m_saveData.cycles += 8;
			m_saveData.registers.IncPC(2);
			m_saveData.registers.SetIM(2);
			break;

		case OxED4D:
			RETI();
			m_saveData.inInterrupt = false;
			// printf("Sale %d\n", m_vdp->GetLine());
			// m_showLog = false;
			break;

		case OxEDA0:
			LDI();
			break;
		case OxEDB0:
			LDIR();
			break;
		case OxEDA1:
			CPI();
			break;
		case OxEDB1:
			CPIR();
			break;
		case OxEDA3:
			OUTI();
			break;
		case OxEDA8:
			LDD();
			break;
		case OxEDB8:
			LDDR();
			break;
		case OxEDA9:
			CPD();
			break;
		case OxEDB9:
			CPDR();
			break;
		case OxEDAB:
			OUTD();
			break;
		case OxEDA2:
			INI();
			break;

		/**
		 * OTIR -> pc:4,pc+1:4,ir:1,hl:3,IO,[bc:1 x 5]
		 * EDB3: OTIR
		 * |2|21/16| A byte from the memory location pointed to by hl is written to port c.
		 * Then hl is incremented and b is decremented. If b is not zero, this operation is repeated.
		 * Interrupts can trigger while this instruction is processing.
		 */
		case OxEDB3: {
			m_saveData.cycles += 9;
			uint16_t hl = m_saveData.registers.GetHL();
			uint8_t b = m_saveData.registers.GetB() - 1;
			uint8_t c = m_saveData.registers.GetC();
			m_saveData.addressBus.L = c;
			m_saveData.addressBus.H = b;
			//				printf("OTIR\n");
			WritePort(c, ReadMemory(hl));
			//				printf("Address: %.4X\n", m_saveData.addressBus._w);
			m_saveData.registers.SetHL(hl + 1);
			m_saveData.registers.SetB(b);
			m_saveData.registers.SetFFlag(Flag_N, true);
			m_saveData.registers.SetFFlag(Flag_Z, true);
			if (b == 0) {
				m_saveData.registers.IncPC(2);
				m_saveData.cycles += 4;
			} else
				m_saveData.cycles += 9;
		} break;

			/******************************************************************************/
			/*************************** Bit instructions (CB) ****************************/
			/******************************************************************************/

		case OxCB00:
			RLC(Reg_B);
			break;
		case OxCB01:
			RLC(Reg_C);
			break;
		case OxCB02:
			RLC(Reg_D);
			break;
		case OxCB03:
			RLC(Reg_E);
			break;
		case OxCB04:
			RLC(Reg_H);
			break;
		case OxCB05:
			RLC(Reg_L);
			break;
		case OxCB06:
			RLC_HL();
			break;
		case OxCB07:
			RLC(Reg_A);
			break;

		case OxCB08:
			RRC(Reg_B);
			break;
		case OxCB09:
			RRC(Reg_C);
			break;
		case OxCB0A:
			RRC(Reg_D);
			break;
		case OxCB0B:
			RRC(Reg_E);
			break;
		case OxCB0C:
			RRC(Reg_H);
			break;
		case OxCB0D:
			RRC(Reg_L);
			break;
		case OxCB0E:
			RRC_HL();
			break;
		case OxCB0F:
			RRC(Reg_A);
			break;

		case OxCB10:
			RL(Reg_B);
			break;
		case OxCB11:
			RL(Reg_C);
			break;
		case OxCB12:
			RL(Reg_D);
			break;
		case OxCB13:
			RL(Reg_E);
			break;
		case OxCB14:
			RL(Reg_H);
			break;
		case OxCB15:
			RL(Reg_L);
			break;
		case OxCB16:
			RL_HL();
			break;
		case OxCB17:
			RL(Reg_A);
			break;

		case OxCB18:
			RR(Reg_B);
			break;
		case OxCB19:
			RR(Reg_C);
			break;
		case OxCB1A:
			RR(Reg_D);
			break;
		case OxCB1B:
			RR(Reg_E);
			break;
		case OxCB1C:
			RR(Reg_H);
			break;
		case OxCB1D:
			RR(Reg_L);
			break;
		case OxCB1E:
			RR_HL();
			break;
		case OxCB1F:
			RR(Reg_A);
			break;

		case OxCB20:
			SLA(Reg_B);
			break;
		case OxCB21:
			SLA(Reg_C);
			break;
		case OxCB22:
			SLA(Reg_D);
			break;
		case OxCB23:
			SLA(Reg_E);
			break;
		case OxCB24:
			SLA(Reg_H);
			break;
		case OxCB25:
			SLA(Reg_L);
			break;
		case OxCB26:
			SLA_HL();
			break;
		case OxCB27:
			SLA(Reg_A);
			break;

		case OxCB28:
			SRA(Reg_B);
			break;
		case OxCB29:
			SRA(Reg_C);
			break;
		case OxCB2A:
			SRA(Reg_D);
			break;
		case OxCB2B:
			SRA(Reg_E);
			break;
		case OxCB2C:
			SRA(Reg_H);
			break;
		case OxCB2D:
			SRA(Reg_L);
			break;
		case OxCB2E:
			SRA_HL();
			break;
		case OxCB2F:
			SRA(Reg_A);
			break;

		case OxCB30:
			SLL(Reg_B);
			break;
		case OxCB31:
			SLL(Reg_C);
			break;
		case OxCB32:
			SLL(Reg_D);
			break;
		case OxCB33:
			SLL(Reg_E);
			break;
		case OxCB34:
			SLL(Reg_H);
			break;
		case OxCB35:
			SLL(Reg_L);
			break;
		case OxCB36:
			SLL_HL();
			break;
		case OxCB37:
			SLL(Reg_A);
			break;

		case OxCB38:
			SRL(Reg_B);
			break;
		case OxCB39:
			SRL(Reg_C);
			break;
		case OxCB3A:
			SRL(Reg_D);
			break;
		case OxCB3B:
			SRL(Reg_E);
			break;
		case OxCB3C:
			SRL(Reg_H);
			break;
		case OxCB3D:
			SRL(Reg_L);
			break;
		case OxCB3E:
			SRL_HL();
			break;
		case OxCB3F:
			SRL(Reg_A);
			break;

		case OxCB40:
			BIT(m_saveData.registers.GetB(), 0x01);
			break;
		case OxCB41:
			BIT(m_saveData.registers.GetC(), 0x01);
			break;
		case OxCB42:
			BIT(m_saveData.registers.GetD(), 0x01);
			break;
		case OxCB43:
			BIT(m_saveData.registers.GetE(), 0x01);
			break;
		case OxCB44:
			BIT(m_saveData.registers.GetH(), 0x01);
			break;
		case OxCB45:
			BIT(m_saveData.registers.GetL(), 0x01);
			break;
		case OxCB46:
			BITHL(0x01);
			break;
		case OxCB47:
			BIT(m_saveData.registers.GetA(), 0x01);
			break;
		case OxCB48:
			BIT(m_saveData.registers.GetB(), 0x02);
			break;
		case OxCB49:
			BIT(m_saveData.registers.GetC(), 0x02);
			break;
		case OxCB4A:
			BIT(m_saveData.registers.GetD(), 0x02);
			break;
		case OxCB4B:
			BIT(m_saveData.registers.GetE(), 0x02);
			break;
		case OxCB4C:
			BIT(m_saveData.registers.GetH(), 0x02);
			break;
		case OxCB4D:
			BIT(m_saveData.registers.GetL(), 0x02);
			break;
		case OxCB4E:
			BITHL(0x02);
			break;
		case OxCB4F:
			BIT(m_saveData.registers.GetA(), 0x02);
			break;
		case OxCB50:
			BIT(m_saveData.registers.GetB(), 0x04);
			break;
		case OxCB51:
			BIT(m_saveData.registers.GetC(), 0x04);
			break;
		case OxCB52:
			BIT(m_saveData.registers.GetD(), 0x04);
			break;
		case OxCB53:
			BIT(m_saveData.registers.GetE(), 0x04);
			break;
		case OxCB54:
			BIT(m_saveData.registers.GetH(), 0x04);
			break;
		case OxCB55:
			BIT(m_saveData.registers.GetL(), 0x04);
			break;
		case OxCB56:
			BITHL(0x04);
			break;
		case OxCB57:
			BIT(m_saveData.registers.GetA(), 0x04);
			break;
		case OxCB58:
			BIT(m_saveData.registers.GetB(), 0x08);
			break;
		case OxCB59:
			BIT(m_saveData.registers.GetC(), 0x08);
			break;
		case OxCB5A:
			BIT(m_saveData.registers.GetD(), 0x08);
			break;
		case OxCB5B:
			BIT(m_saveData.registers.GetE(), 0x08);
			break;
		case OxCB5C:
			BIT(m_saveData.registers.GetH(), 0x08);
			break;
		case OxCB5D:
			BIT(m_saveData.registers.GetL(), 0x08);
			break;
		case OxCB5E:
			BITHL(0x08);
			break;
		case OxCB5F:
			BIT(m_saveData.registers.GetA(), 0x08);
			break;
		case OxCB60:
			BIT(m_saveData.registers.GetB(), 0x10);
			break;
		case OxCB61:
			BIT(m_saveData.registers.GetC(), 0x10);
			break;
		case OxCB62:
			BIT(m_saveData.registers.GetD(), 0x10);
			break;
		case OxCB63:
			BIT(m_saveData.registers.GetE(), 0x10);
			break;
		case OxCB64:
			BIT(m_saveData.registers.GetH(), 0x10);
			break;
		case OxCB65:
			BIT(m_saveData.registers.GetL(), 0x10);
			break;
		case OxCB66:
			BITHL(0x10);
			break;
		case OxCB67:
			BIT(m_saveData.registers.GetA(), 0x10);
			break;
		case OxCB68:
			BIT(m_saveData.registers.GetB(), 0x20);
			break;
		case OxCB69:
			BIT(m_saveData.registers.GetC(), 0x20);
			break;
		case OxCB6A:
			BIT(m_saveData.registers.GetD(), 0x20);
			break;
		case OxCB6B:
			BIT(m_saveData.registers.GetE(), 0x20);
			break;
		case OxCB6C:
			BIT(m_saveData.registers.GetH(), 0x20);
			break;
		case OxCB6D:
			BIT(m_saveData.registers.GetL(), 0x20);
			break;
		case OxCB6E:
			BITHL(0x20);
			break;
		case OxCB6F:
			BIT(m_saveData.registers.GetA(), 0x20);
			break;
		case OxCB70:
			BIT(m_saveData.registers.GetB(), 0x40);
			break;
		case OxCB71:
			BIT(m_saveData.registers.GetC(), 0x40);
			break;
		case OxCB72:
			BIT(m_saveData.registers.GetD(), 0x40);
			break;
		case OxCB73:
			BIT(m_saveData.registers.GetE(), 0x40);
			break;
		case OxCB74:
			BIT(m_saveData.registers.GetH(), 0x40);
			break;
		case OxCB75:
			BIT(m_saveData.registers.GetL(), 0x40);
			break;
		case OxCB76:
			BITHL(0x40);
			break;
		case OxCB77:
			BIT(m_saveData.registers.GetA(), 0x40);
			break;
		case OxCB78:
			BIT(m_saveData.registers.GetB(), 0x80);
			break;
		case OxCB79:
			BIT(m_saveData.registers.GetC(), 0x80);
			break;
		case OxCB7A:
			BIT(m_saveData.registers.GetD(), 0x80);
			break;
		case OxCB7B:
			BIT(m_saveData.registers.GetE(), 0x80);
			break;
		case OxCB7C:
			BIT(m_saveData.registers.GetH(), 0x80);
			break;
		case OxCB7D:
			BIT(m_saveData.registers.GetL(), 0x80);
			break;
		case OxCB7E:
			BITHL(0x80);
			break;
		case OxCB7F:
			BIT(m_saveData.registers.GetA(), 0x80);
			break;

		case OxCB80:
			RES(Reg_B, 0x01);
			break;
		case OxCB81:
			RES(Reg_C, 0x01);
			break;
		case OxCB82:
			RES(Reg_D, 0x01);
			break;
		case OxCB83:
			RES(Reg_E, 0x01);
			break;
		case OxCB84:
			RES(Reg_H, 0x01);
			break;
		case OxCB85:
			RES(Reg_L, 0x01);
			break;
		case OxCB86:
			RESHL(0x01);
			break;
		case OxCB87:
			RES(Reg_A, 0x01);
			break;
		case OxCB88:
			RES(Reg_B, 0x02);
			break;
		case OxCB89:
			RES(Reg_C, 0x02);
			break;
		case OxCB8A:
			RES(Reg_D, 0x02);
			break;
		case OxCB8B:
			RES(Reg_E, 0x02);
			break;
		case OxCB8C:
			RES(Reg_H, 0x02);
			break;
		case OxCB8D:
			RES(Reg_L, 0x02);
			break;
		case OxCB8E:
			RESHL(0x02);
			break;
		case OxCB8F:
			RES(Reg_A, 0x02);
			break;
		case OxCB90:
			RES(Reg_B, 0x04);
			break;
		case OxCB91:
			RES(Reg_C, 0x04);
			break;
		case OxCB92:
			RES(Reg_D, 0x04);
			break;
		case OxCB93:
			RES(Reg_E, 0x04);
			break;
		case OxCB94:
			RES(Reg_H, 0x04);
			break;
		case OxCB95:
			RES(Reg_L, 0x04);
			break;
		case OxCB96:
			RESHL(0x04);
			break;
		case OxCB97:
			RES(Reg_A, 0x04);
			break;
		case OxCB98:
			RES(Reg_B, 0x08);
			break;
		case OxCB99:
			RES(Reg_C, 0x08);
			break;
		case OxCB9A:
			RES(Reg_D, 0x08);
			break;
		case OxCB9B:
			RES(Reg_E, 0x08);
			break;
		case OxCB9C:
			RES(Reg_H, 0x08);
			break;
		case OxCB9D:
			RES(Reg_L, 0x08);
			break;
		case OxCB9E:
			RESHL(0x08);
			break;
		case OxCB9F:
			RES(Reg_A, 0x08);
			break;
		case OxCBA0:
			RES(Reg_B, 0x10);
			break;
		case OxCBA1:
			RES(Reg_C, 0x10);
			break;
		case OxCBA2:
			RES(Reg_D, 0x10);
			break;
		case OxCBA3:
			RES(Reg_E, 0x10);
			break;
		case OxCBA4:
			RES(Reg_H, 0x10);
			break;
		case OxCBA5:
			RES(Reg_L, 0x10);
			break;
		case OxCBA6:
			RESHL(0x10);
			break;
		case OxCBA7:
			RES(Reg_A, 0x10);
			break;
		case OxCBA8:
			RES(Reg_B, 0x20);
			break;
		case OxCBA9:
			RES(Reg_C, 0x20);
			break;
		case OxCBAA:
			RES(Reg_D, 0x20);
			break;
		case OxCBAB:
			RES(Reg_E, 0x20);
			break;
		case OxCBAC:
			RES(Reg_H, 0x20);
			break;
		case OxCBAD:
			RES(Reg_L, 0x20);
			break;
		case OxCBAE:
			RESHL(0x20);
			break;
		case OxCBAF:
			RES(Reg_A, 0x20);
			break;
		case OxCBB0:
			RES(Reg_B, 0x40);
			break;
		case OxCBB1:
			RES(Reg_C, 0x40);
			break;
		case OxCBB2:
			RES(Reg_D, 0x40);
			break;
		case OxCBB3:
			RES(Reg_E, 0x40);
			break;
		case OxCBB4:
			RES(Reg_H, 0x40);
			break;
		case OxCBB5:
			RES(Reg_L, 0x40);
			break;
		case OxCBB6:
			RESHL(0x40);
			break;
		case OxCBB7:
			RES(Reg_A, 0x40);
			break;
		case OxCBB8:
			RES(Reg_B, 0x80);
			break;
		case OxCBB9:
			RES(Reg_C, 0x80);
			break;
		case OxCBBA:
			RES(Reg_D, 0x80);
			break;
		case OxCBBB:
			RES(Reg_E, 0x80);
			break;
		case OxCBBC:
			RES(Reg_H, 0x80);
			break;
		case OxCBBD:
			RES(Reg_L, 0x80);
			break;
		case OxCBBE:
			RESHL(0x80);
			break;
		case OxCBBF:
			RES(Reg_A, 0x80);
			break;

		case OxCBC0:
			SET(Reg_B, 0x01);
			break;
		case OxCBC1:
			SET(Reg_C, 0x01);
			break;
		case OxCBC2:
			SET(Reg_D, 0x01);
			break;
		case OxCBC3:
			SET(Reg_E, 0x01);
			break;
		case OxCBC4:
			SET(Reg_H, 0x01);
			break;
		case OxCBC5:
			SET(Reg_L, 0x01);
			break;
		case OxCBC6:
			SETHL(0x01);
			break;
		case OxCBC7:
			SET(Reg_A, 0x01);
			break;
		case OxCBC8:
			SET(Reg_B, 0x02);
			break;
		case OxCBC9:
			SET(Reg_C, 0x02);
			break;
		case OxCBCA:
			SET(Reg_D, 0x02);
			break;
		case OxCBCB:
			SET(Reg_E, 0x02);
			break;
		case OxCBCC:
			SET(Reg_H, 0x02);
			break;
		case OxCBCD:
			SET(Reg_L, 0x02);
			break;
		case OxCBCE:
			SETHL(0x02);
			break;
		case OxCBCF:
			SET(Reg_A, 0x02);
			break;
		case OxCBD0:
			SET(Reg_B, 0x04);
			break;
		case OxCBD1:
			SET(Reg_C, 0x04);
			break;
		case OxCBD2:
			SET(Reg_D, 0x04);
			break;
		case OxCBD3:
			SET(Reg_E, 0x04);
			break;
		case OxCBD4:
			SET(Reg_H, 0x04);
			break;
		case OxCBD5:
			SET(Reg_L, 0x04);
			break;
		case OxCBD6:
			SETHL(0x04);
			break;
		case OxCBD7:
			SET(Reg_A, 0x04);
			break;
		case OxCBD8:
			SET(Reg_B, 0x08);
			break;
		case OxCBD9:
			SET(Reg_C, 0x08);
			break;
		case OxCBDA:
			SET(Reg_D, 0x08);
			break;
		case OxCBDB:
			SET(Reg_E, 0x08);
			break;
		case OxCBDC:
			SET(Reg_H, 0x08);
			break;
		case OxCBDD:
			SET(Reg_L, 0x08);
			break;
		case OxCBDE:
			SETHL(0x08);
			break;
		case OxCBDF:
			SET(Reg_A, 0x08);
			break;
		case OxCBE0:
			SET(Reg_B, 0x10);
			break;
		case OxCBE1:
			SET(Reg_C, 0x10);
			break;
		case OxCBE2:
			SET(Reg_D, 0x10);
			break;
		case OxCBE3:
			SET(Reg_E, 0x10);
			break;
		case OxCBE4:
			SET(Reg_H, 0x10);
			break;
		case OxCBE5:
			SET(Reg_L, 0x10);
			break;
		case OxCBE6:
			SETHL(0x10);
			break;
		case OxCBE7:
			SET(Reg_A, 0x10);
			break;
		case OxCBE8:
			SET(Reg_B, 0x20);
			break;
		case OxCBE9:
			SET(Reg_C, 0x20);
			break;
		case OxCBEA:
			SET(Reg_D, 0x20);
			break;
		case OxCBEB:
			SET(Reg_E, 0x20);
			break;
		case OxCBEC:
			SET(Reg_H, 0x20);
			break;
		case OxCBED:
			SET(Reg_L, 0x20);
			break;
		case OxCBEE:
			SETHL(0x20);
			break;
		case OxCBEF:
			SET(Reg_A, 0x20);
			break;
		case OxCBF0:
			SET(Reg_B, 0x40);
			break;
		case OxCBF1:
			SET(Reg_C, 0x40);
			break;
		case OxCBF2:
			SET(Reg_D, 0x40);
			break;
		case OxCBF3:
			SET(Reg_E, 0x40);
			break;
		case OxCBF4:
			SET(Reg_H, 0x40);
			break;
		case OxCBF5:
			SET(Reg_L, 0x40);
			break;
		case OxCBF6:
			SETHL(0x40);
			break;
		case OxCBF7:
			SET(Reg_A, 0x40);
			break;
		case OxCBF8:
			SET(Reg_B, 0x80);
			break;
		case OxCBF9:
			SET(Reg_C, 0x80);
			break;
		case OxCBFA:
			SET(Reg_D, 0x80);
			break;
		case OxCBFB:
			SET(Reg_E, 0x80);
			break;
		case OxCBFC:
			SET(Reg_H, 0x80);
			break;
		case OxCBFD:
			SET(Reg_L, 0x80);
			break;
		case OxCBFE:
			SETHL(0x80);
			break;
		case OxCBFF:
			SET(Reg_A, 0x80);
			break;

			/******************************************************************************/
			/*************************** IX instructions (DD) *****************************/
			/******************************************************************************/

		case OxDD09:
			ADDXXpp(Reg_IX, m_saveData.registers.GetBC(), 15, 2);
			break;
		case OxDD19:
			ADDXXpp(Reg_IX, m_saveData.registers.GetDE(), 15, 2);
			break;
		case OxDD29:
			ADDXXpp(Reg_IX, m_saveData.registers.GetIX(), 15, 2);
			break;
		case OxDD39:
			ADDXXpp(Reg_IX, m_saveData.registers.GetSP(), 15, 2);
			break;

		case OxDD86:
			ADD(ReadMemory(m_saveData.registers.GetIX() + (int8_t) ReadMemory(pc + 2)), 13, 3);
			break;
		case OxDD8E:
			ADC(ReadMemory(m_saveData.registers.GetIX() + (int8_t) ReadMemory(pc + 2)), 13, 3);
			break;
		case OxDD96:
			SUB(ReadMemory(m_saveData.registers.GetIX() + (int8_t) ReadMemory(pc + 2)), 13, 3);
			break;
		case OxDD9E:
			SBC(ReadMemory(m_saveData.registers.GetIX() + (int8_t) ReadMemory(pc + 2)), 13, 3);
			break;
		case OxDDA6:
			AND(ReadMemory(m_saveData.registers.GetIX() + (int8_t) ReadMemory(pc + 2)), 13, 3);
			break;
		case OxDDAE:
			XOR(ReadMemory(m_saveData.registers.GetIX() + (int8_t) ReadMemory(pc + 2)), 13, 3);
			break;
		case OxDDB6:
			OR(ReadMemory(m_saveData.registers.GetIX() + (int8_t) ReadMemory(pc + 2)), 13, 3);
			break;
		case OxDDBE:
			CP(ReadMemory(m_saveData.registers.GetIX() + (int8_t) ReadMemory(pc + 2)), 13, 3);
			break;

		case OxDDE1:
			POP16X(Reg_IX);
			break;
		case OxDDE5:
			PUSH16X(Reg_IX);
			break;
		case OxDD23:
			INCss(Reg_IX, 10, 2);
			break;
		case OxDD2B:
			DECss(Reg_IX, 10, 2);
			break;
		case OxDD34:
			INCXXd(Reg_IX);
			break;
		case OxDD35:
			DECXXd(Reg_IX);
			break;
		case OxDD36:
			LDXXdn(Reg_IX);
			break;

		case OxDD21:
			LDddnnX(Reg_IX);
			break;
		case OxDD22:
			LDnndd(Reg_IX);
			break;
		case OxDD24:
			INCr(Reg_IXH, 8, 2);
			break;
		case OxDD25:
			DECm(Reg_IXH, 8, 2);
			break;
		case OxDD2C:
			INCr(Reg_IXL, 8, 2);
			break;
		case OxDD2D:
			DECm(Reg_IXL, 8, 2);
			break;
		case OxDD2A:
			LDdd_nn(Reg_IX);
			break;

		case OxDD46:
			LDrXXd(Reg_B, Reg_IX);
			break;
		case OxDD4E:
			LDrXXd(Reg_C, Reg_IX);
			break;
		case OxDD56:
			LDrXXd(Reg_D, Reg_IX);
			break;
		case OxDD5E:
			LDrXXd(Reg_E, Reg_IX);
			break;
		case OxDD66:
			LDrXXd(Reg_H, Reg_IX);
			break;
		case OxDD6E:
			LDrXXd(Reg_L, Reg_IX);
			break;
		case OxDD7E:
			LDrXXd(Reg_A, Reg_IX);
			break;

		case OxDD44:
			LDrr(Reg_B, m_saveData.registers.GetIXH(), 8, 2);
			break;
		case OxDD45:
			LDrr(Reg_B, m_saveData.registers.GetIXL(), 8, 2);
			break;
		case OxDD4C:
			LDrr(Reg_C, m_saveData.registers.GetIXH(), 8, 2);
			break;
		case OxDD4D:
			LDrr(Reg_C, m_saveData.registers.GetIXL(), 8, 2);
			break;
		case OxDD54:
			LDrr(Reg_D, m_saveData.registers.GetIXH(), 8, 2);
			break;
		case OxDD55:
			LDrr(Reg_D, m_saveData.registers.GetIXL(), 8, 2);
			break;
		case OxDD5C:
			LDrr(Reg_E, m_saveData.registers.GetIXH(), 8, 2);
			break;
		case OxDD5D:
			LDrr(Reg_E, m_saveData.registers.GetIXL(), 8, 2);
			break;
		case OxDD60:
			LDrr(Reg_IXH, m_saveData.registers.GetB(), 8, 2);
			break;
		case OxDD61:
			LDrr(Reg_IXH, m_saveData.registers.GetC(), 8, 2);
			break;
		case OxDD62:
			LDrr(Reg_IXH, m_saveData.registers.GetD(), 8, 2);
			break;
		case OxDD63:
			LDrr(Reg_IXH, m_saveData.registers.GetE(), 8, 2);
			break;
		case OxDD64:
			LDrr(Reg_IXH, m_saveData.registers.GetIXH(), 8, 2);
			break;
		case OxDD65:
			LDrr(Reg_IXH, m_saveData.registers.GetIXL(), 8, 2);
			break;
		case OxDD67:
			LDrr(Reg_IXH, m_saveData.registers.GetA(), 8, 2);
			break;
		case OxDD68:
			LDrr(Reg_IXL, m_saveData.registers.GetB(), 8, 2);
			break;
		case OxDD69:
			LDrr(Reg_IXL, m_saveData.registers.GetC(), 8, 2);
			break;
		case OxDD6A:
			LDrr(Reg_IXL, m_saveData.registers.GetD(), 8, 2);
			break;
		case OxDD6B:
			LDrr(Reg_IXL, m_saveData.registers.GetE(), 8, 2);
			break;
		case OxDD6C:
			LDrr(Reg_IXL, m_saveData.registers.GetIXH(), 8, 2);
			break;
		case OxDD6D:
			LDrr(Reg_IXL, m_saveData.registers.GetIXL(), 8, 2);
			break;
		case OxDD6F:
			LDrr(Reg_IXL, m_saveData.registers.GetA(), 8, 2);
			break;
		case OxDD7C:
			LDrr(Reg_A, m_saveData.registers.GetIXH(), 8, 2);
			break;
		case OxDD7D:
			LDrr(Reg_A, m_saveData.registers.GetIXL(), 8, 2);
			break;

		case OxDD70:
			LDXXdr(Reg_IX, Reg_B);
			break;
		case OxDD71:
			LDXXdr(Reg_IX, Reg_C);
			break;
		case OxDD72:
			LDXXdr(Reg_IX, Reg_D);
			break;
		case OxDD73:
			LDXXdr(Reg_IX, Reg_E);
			break;
		case OxDD74:
			LDXXdr(Reg_IX, Reg_H);
			break;
		case OxDD75:
			LDXXdr(Reg_IX, Reg_L);
			break;
		case OxDD77:
			LDXXdr(Reg_IX, Reg_A);
			break;

		case OxDD26:
			LDriin(Reg_IXH);
			break;
		case OxDD2E:
			LDriin(Reg_IXL);
			break;

		case OxDD84:
			ADD(m_saveData.registers.GetIXH(), 8, 2);
			break;
		case OxDD85:
			ADD(m_saveData.registers.GetIXL(), 8, 2);
			break;
		case OxDD8C:
			ADC(m_saveData.registers.GetIXH(), 8, 2);
			break;
		case OxDD8D:
			ADC(m_saveData.registers.GetIXL(), 8, 2);
			break;
		case OxDD94:
			SUB(m_saveData.registers.GetIXH(), 8, 2);
			break;
		case OxDD95:
			SUB(m_saveData.registers.GetIXL(), 8, 2);
			break;
		case OxDD9C:
			SBC(m_saveData.registers.GetIXH(), 8, 2);
			break;
		case OxDD9D:
			SBC(m_saveData.registers.GetIXL(), 8, 2);
			break;
		case OxDDA4:
			AND(m_saveData.registers.GetIXH(), 8, 2);
			break;
		case OxDDA5:
			AND(m_saveData.registers.GetIXL(), 8, 2);
			break;
		case OxDDAC:
			XOR(m_saveData.registers.GetIXH(), 8, 2);
			break;
		case OxDDAD:
			XOR(m_saveData.registers.GetIXL(), 8, 2);
			break;
		case OxDDB4:
			OR(m_saveData.registers.GetIXH(), 8, 2);
			break;
		case OxDDB5:
			OR(m_saveData.registers.GetIXL(), 8, 2);
			break;
		case OxDDBC:
			CP(m_saveData.registers.GetIXH(), 8, 2);
			break;
		case OxDDBD:
			CP(m_saveData.registers.GetIXL(), 8, 2);
			break;

			/******************************************************************************/
			/************************* IX bit instructions (DDCB) *************************/
			/******************************************************************************/

		case OxDDCBnn06:
			RLCXXd(Reg_IX);
			break;
		case OxDDCBnn0E:
			RRCXXd(Reg_IX);
			break;
		case OxDDCBnn16:
			RLXXd(Reg_IX);
			break;
		case OxDDCBnn1E:
			RRXXd(Reg_IX);
			break;
		case OxDDCBnn26:
			SLAXXd(Reg_IX);
			break;
		case OxDDCBnn2E:
			SRAXXd(Reg_IX);
			break;
		case OxDDCBnn36:
			SLLXXd(Reg_IX);
			break;
		case OxDDCBnn3E:
			SRLXXd(Reg_IX);
			break;

		case OxDDCBnn40:
		case OxDDCBnn41:
		case OxDDCBnn42:
		case OxDDCBnn43:
		case OxDDCBnn44:
		case OxDDCBnn45:
		case OxDDCBnn47:
		case OxDDCBnn46:
			BITbssd(0x01, Reg_IX);
			break;

		case OxDDCBnn48:
		case OxDDCBnn49:
		case OxDDCBnn4A:
		case OxDDCBnn4B:
		case OxDDCBnn4C:
		case OxDDCBnn4D:
		case OxDDCBnn4F:
		case OxDDCBnn4E:
			BITbssd(0x02, Reg_IX);
			break;

		case OxDDCBnn50:
		case OxDDCBnn51:
		case OxDDCBnn52:
		case OxDDCBnn53:
		case OxDDCBnn54:
		case OxDDCBnn55:
		case OxDDCBnn57:
		case OxDDCBnn56:
			BITbssd(0x04, Reg_IX);
			break;

		case OxDDCBnn58:
		case OxDDCBnn59:
		case OxDDCBnn5A:
		case OxDDCBnn5B:
		case OxDDCBnn5C:
		case OxDDCBnn5D:
		case OxDDCBnn5F:
		case OxDDCBnn5E:
			BITbssd(0x08, Reg_IX);
			break;

		case OxDDCBnn60:
		case OxDDCBnn61:
		case OxDDCBnn62:
		case OxDDCBnn63:
		case OxDDCBnn64:
		case OxDDCBnn65:
		case OxDDCBnn67:
		case OxDDCBnn66:
			BITbssd(0x10, Reg_IX);
			break;

		case OxDDCBnn68:
		case OxDDCBnn69:
		case OxDDCBnn6A:
		case OxDDCBnn6B:
		case OxDDCBnn6C:
		case OxDDCBnn6D:
		case OxDDCBnn6F:
		case OxDDCBnn6E:
			BITbssd(0x20, Reg_IX);
			break;

		case OxDDCBnn70:
		case OxDDCBnn71:
		case OxDDCBnn72:
		case OxDDCBnn73:
		case OxDDCBnn74:
		case OxDDCBnn75:
		case OxDDCBnn77:
		case OxDDCBnn76:
			BITbssd(0x40, Reg_IX);
			break;

		case OxDDCBnn78:
		case OxDDCBnn79:
		case OxDDCBnn7A:
		case OxDDCBnn7B:
		case OxDDCBnn7C:
		case OxDDCBnn7D:
		case OxDDCBnn7F:
		case OxDDCBnn7E:
			BITbssd(0x80, Reg_IX);
			break;

		case OxDDCBnn86:
			RESbssd(0x01, Reg_IX);
			break;
		case OxDDCBnn8E:
			RESbssd(0x02, Reg_IX);
			break;
		case OxDDCBnn96:
			RESbssd(0x04, Reg_IX);
			break;
		case OxDDCBnn9E:
			RESbssd(0x08, Reg_IX);
			break;
		case OxDDCBnnA6:
			RESbssd(0x10, Reg_IX);
			break;
		case OxDDCBnnAE:
			RESbssd(0x20, Reg_IX);
			break;
		case OxDDCBnnB6:
			RESbssd(0x40, Reg_IX);
			break;
		case OxDDCBnnBE:
			RESbssd(0x80, Reg_IX);
			break;

		case OxDDCBnnC6:
			SETbssd(0x01, Reg_IX);
			break;
		case OxDDCBnnCE:
			SETbssd(0x02, Reg_IX);
			break;
		case OxDDCBnnD6:
			SETbssd(0x04, Reg_IX);
			break;
		case OxDDCBnnDE:
			SETbssd(0x08, Reg_IX);
			break;
		case OxDDCBnnE6:
			SETbssd(0x10, Reg_IX);
			break;
		case OxDDCBnnEE:
			SETbssd(0x20, Reg_IX);
			break;
		case OxDDCBnnF6:
			SETbssd(0x40, Reg_IX);
			break;
		case OxDDCBnnFE:
			SETbssd(0x80, Reg_IX);
			break;

			/******************************************************************************/
			/*************************** IY instructions (FD) *****************************/
			/******************************************************************************/

		case OxFD09:
			ADDXXpp(Reg_IY, m_saveData.registers.GetBC(), 15, 2);
			break;
		case OxFD19:
			ADDXXpp(Reg_IY, m_saveData.registers.GetDE(), 15, 2);
			break;
		case OxFD29:
			ADDXXpp(Reg_IY, m_saveData.registers.GetIY(), 15, 2);
			break;
		case OxFD39:
			ADDXXpp(Reg_IY, m_saveData.registers.GetSP(), 15, 2);
			break;

		case OxFD86:
			ADD(ReadMemory(m_saveData.registers.GetIY() + (int8_t) ReadMemory(pc + 2)), 13, 3);
			break;
		case OxFD8E:
			ADC(ReadMemory(m_saveData.registers.GetIY() + (int8_t) ReadMemory(pc + 2)), 13, 3);
			break;
		case OxFD96:
			SUB(ReadMemory(m_saveData.registers.GetIY() + (int8_t) ReadMemory(pc + 2)), 13, 3);
			break;
		case OxFD9E:
			SBC(ReadMemory(m_saveData.registers.GetIY() + (int8_t) ReadMemory(pc + 2)), 13, 3);
			break;
		case OxFDA6:
			AND(ReadMemory(m_saveData.registers.GetIY() + (int8_t) ReadMemory(pc + 2)), 13, 3);
			break;
		case OxFDAE:
			XOR(ReadMemory(m_saveData.registers.GetIY() + (int8_t) ReadMemory(pc + 2)), 13, 3);
			break;
		case OxFDB6:
			OR(ReadMemory(m_saveData.registers.GetIY() + (int8_t) ReadMemory(pc + 2)), 13, 3);
			break;
		case OxFDBE:
			CP(ReadMemory(m_saveData.registers.GetIY() + (int8_t) ReadMemory(pc + 2)), 13, 3);
			break;

		case OxFDE1:
			POP16X(Reg_IY);
			break;
		case OxFDE5:
			PUSH16X(Reg_IY);
			break;
		case OxFD23:
			INCss(Reg_IY, 10, 2);
			break;
		case OxFD2B:
			DECss(Reg_IY, 10, 2);
			break;
		case OxFD34:
			INCXXd(Reg_IY);
			break;
		case OxFD35:
			DECXXd(Reg_IY);
			break;
		case OxFD36:
			LDXXdn(Reg_IY);
			break;

		case OxFD21:
			LDddnnX(Reg_IY);
			break;
		case OxFD22:
			LDnndd(Reg_IY);
			break;
		case OxFD24:
			INCr(Reg_IYH, 8, 2);
			break;
		case OxFD25:
			DECm(Reg_IYH, 8, 2);
			break;
		case OxFD2C:
			INCr(Reg_IYL, 8, 2);
			break;
		case OxFD2D:
			DECm(Reg_IYL, 8, 2);
			break;
		case OxFD2A:
			LDdd_nn(Reg_IY);
			break;

		case OxFD46:
			LDrXXd(Reg_B, Reg_IY);
			break;
		case OxFD4E:
			LDrXXd(Reg_C, Reg_IY);
			break;
		case OxFD56:
			LDrXXd(Reg_D, Reg_IY);
			break;
		case OxFD5E:
			LDrXXd(Reg_E, Reg_IY);
			break;
		case OxFD66:
			LDrXXd(Reg_H, Reg_IY);
			break;
		case OxFD6E:
			LDrXXd(Reg_L, Reg_IY);
			break;
		case OxFD7E:
			LDrXXd(Reg_A, Reg_IY);
			break;

		case OxFD44:
			LDrr(Reg_B, m_saveData.registers.GetIYH(), 8, 2);
			break;
		case OxFD45:
			LDrr(Reg_B, m_saveData.registers.GetIYL(), 8, 2);
			break;
		case OxFD4C:
			LDrr(Reg_C, m_saveData.registers.GetIYH(), 8, 2);
			break;
		case OxFD4D:
			LDrr(Reg_C, m_saveData.registers.GetIYL(), 8, 2);
			break;
		case OxFD54:
			LDrr(Reg_D, m_saveData.registers.GetIYH(), 8, 2);
			break;
		case OxFD55:
			LDrr(Reg_D, m_saveData.registers.GetIYL(), 8, 2);
			break;
		case OxFD5C:
			LDrr(Reg_E, m_saveData.registers.GetIYH(), 8, 2);
			break;
		case OxFD5D:
			LDrr(Reg_E, m_saveData.registers.GetIYL(), 8, 2);
			break;
		case OxFD60:
			LDrr(Reg_IYH, m_saveData.registers.GetB(), 8, 2);
			break;
		case OxFD61:
			LDrr(Reg_IYH, m_saveData.registers.GetC(), 8, 2);
			break;
		case OxFD62:
			LDrr(Reg_IYH, m_saveData.registers.GetD(), 8, 2);
			break;
		case OxFD63:
			LDrr(Reg_IYH, m_saveData.registers.GetE(), 8, 2);
			break;
		case OxFD64:
			LDrr(Reg_IYH, m_saveData.registers.GetIYH(), 8, 2);
			break;
		case OxFD65:
			LDrr(Reg_IYH, m_saveData.registers.GetIYL(), 8, 2);
			break;
		case OxFD67:
			LDrr(Reg_IYH, m_saveData.registers.GetA(), 8, 2);
			break;
		case OxFD68:
			LDrr(Reg_IYL, m_saveData.registers.GetB(), 8, 2);
			break;
		case OxFD69:
			LDrr(Reg_IYL, m_saveData.registers.GetC(), 8, 2);
			break;
		case OxFD6A:
			LDrr(Reg_IYL, m_saveData.registers.GetD(), 8, 2);
			break;
		case OxFD6B:
			LDrr(Reg_IYL, m_saveData.registers.GetE(), 8, 2);
			break;
		case OxFD6C:
			LDrr(Reg_IYL, m_saveData.registers.GetIYH(), 8, 2);
			break;
		case OxFD6D:
			LDrr(Reg_IYL, m_saveData.registers.GetIYL(), 8, 2);
			break;
		case OxFD6F:
			LDrr(Reg_IYL, m_saveData.registers.GetA(), 8, 2);
			break;
		case OxFD7C:
			LDrr(Reg_A, m_saveData.registers.GetIYH(), 8, 2);
			break;
		case OxFD7D:
			LDrr(Reg_A, m_saveData.registers.GetIYL(), 8, 2);
			break;

		case OxFD70:
			LDXXdr(Reg_IY, Reg_B);
			break;
		case OxFD71:
			LDXXdr(Reg_IY, Reg_C);
			break;
		case OxFD72:
			LDXXdr(Reg_IY, Reg_D);
			break;
		case OxFD73:
			LDXXdr(Reg_IY, Reg_E);
			break;
		case OxFD74:
			LDXXdr(Reg_IY, Reg_H);
			break;
		case OxFD75:
			LDXXdr(Reg_IY, Reg_L);
			break;
		case OxFD77:
			LDXXdr(Reg_IY, Reg_A);
			break;

		case OxFD26:
			LDriin(Reg_IYH);
			break;
		case OxFD2E:
			LDriin(Reg_IYL);
			break;

		case OxFD84:
			ADD(m_saveData.registers.GetIYH(), 8, 2);
			break;
		case OxFD85:
			ADD(m_saveData.registers.GetIYL(), 8, 2);
			break;
		case OxFD8C:
			ADC(m_saveData.registers.GetIYH(), 8, 2);
			break;
		case OxFD8D:
			ADC(m_saveData.registers.GetIYL(), 8, 2);
			break;
		case OxFD94:
			SUB(m_saveData.registers.GetIYH(), 8, 2);
			break;
		case OxFD95:
			SUB(m_saveData.registers.GetIYL(), 8, 2);
			break;
		case OxFD9C:
			SBC(m_saveData.registers.GetIYH(), 8, 2);
			break;
		case OxFD9D:
			SBC(m_saveData.registers.GetIYL(), 8, 2);
			break;
		case OxFDA4:
			AND(m_saveData.registers.GetIYH(), 8, 2);
			break;
		case OxFDA5:
			AND(m_saveData.registers.GetIYL(), 8, 2);
			break;
		case OxFDAC:
			XOR(m_saveData.registers.GetIYH(), 8, 2);
			break;
		case OxFDAD:
			XOR(m_saveData.registers.GetIYL(), 8, 2);
			break;
		case OxFDB4:
			OR(m_saveData.registers.GetIYH(), 8, 2);
			break;
		case OxFDB5:
			OR(m_saveData.registers.GetIYL(), 8, 2);
			break;
		case OxFDBC:
			CP(m_saveData.registers.GetIYH(), 8, 2);
			break;
		case OxFDBD:
			CP(m_saveData.registers.GetIYL(), 8, 2);
			break;

			/******************************************************************************/
			/************************* IY bit instructions (FDCB) *************************/
			/******************************************************************************/

		case OxFDCBnn06:
			RLCXXd(Reg_IY);
			break;
		case OxFDCBnn0E:
			RRCXXd(Reg_IY);
			break;
		case OxFDCBnn16:
			RLXXd(Reg_IY);
			break;
		case OxFDCBnn1E:
			RRXXd(Reg_IY);
			break;
		case OxFDCBnn26:
			SLAXXd(Reg_IY);
			break;
		case OxFDCBnn2E:
			SRAXXd(Reg_IY);
			break;
		case OxFDCBnn36:
			SLLXXd(Reg_IY);
			break;
		case OxFDCBnn3E:
			SRLXXd(Reg_IY);
			break;

		case OxFDCBnn40:
		case OxFDCBnn41:
		case OxFDCBnn42:
		case OxFDCBnn43:
		case OxFDCBnn44:
		case OxFDCBnn45:
		case OxFDCBnn47:
		case OxFDCBnn46:
			BITbssd(0x01, Reg_IY);
			break;

		case OxFDCBnn48:
		case OxFDCBnn49:
		case OxFDCBnn4A:
		case OxFDCBnn4B:
		case OxFDCBnn4C:
		case OxFDCBnn4D:
		case OxFDCBnn4F:
		case OxFDCBnn4E:
			BITbssd(0x02, Reg_IY);
			break;

		case OxFDCBnn50:
		case OxFDCBnn51:
		case OxFDCBnn52:
		case OxFDCBnn53:
		case OxFDCBnn54:
		case OxFDCBnn55:
		case OxFDCBnn57:
		case OxFDCBnn56:
			BITbssd(0x04, Reg_IY);
			break;

		case OxFDCBnn58:
		case OxFDCBnn59:
		case OxFDCBnn5A:
		case OxFDCBnn5B:
		case OxFDCBnn5C:
		case OxFDCBnn5D:
		case OxFDCBnn5F:
		case OxFDCBnn5E:
			BITbssd(0x08, Reg_IY);
			break;

		case OxFDCBnn60:
		case OxFDCBnn61:
		case OxFDCBnn62:
		case OxFDCBnn63:
		case OxFDCBnn64:
		case OxFDCBnn65:
		case OxFDCBnn67:
		case OxFDCBnn66:
			BITbssd(0x10, Reg_IY);
			break;

		case OxFDCBnn68:
		case OxFDCBnn69:
		case OxFDCBnn6A:
		case OxFDCBnn6B:
		case OxFDCBnn6C:
		case OxFDCBnn6D:
		case OxFDCBnn6F:
		case OxFDCBnn6E:
			BITbssd(0x20, Reg_IY);
			break;

		case OxFDCBnn70:
		case OxFDCBnn71:
		case OxFDCBnn72:
		case OxFDCBnn73:
		case OxFDCBnn74:
		case OxFDCBnn75:
		case OxFDCBnn77:
		case OxFDCBnn76:
			BITbssd(0x40, Reg_IY);
			break;

		case OxFDCBnn78:
		case OxFDCBnn79:
		case OxFDCBnn7A:
		case OxFDCBnn7B:
		case OxFDCBnn7C:
		case OxFDCBnn7D:
		case OxFDCBnn7F:
		case OxFDCBnn7E:
			BITbssd(0x80, Reg_IY);
			break;

		case OxFDCBnn86:
			RESbssd(0x01, Reg_IY);
			break;
		case OxFDCBnn8E:
			RESbssd(0x02, Reg_IY);
			break;
		case OxFDCBnn96:
			RESbssd(0x04, Reg_IY);
			break;
		case OxFDCBnn9E:
			RESbssd(0x08, Reg_IY);
			break;
		case OxFDCBnnA6:
			RESbssd(0x10, Reg_IY);
			break;
		case OxFDCBnnAE:
			RESbssd(0x20, Reg_IY);
			break;
		case OxFDCBnnB6:
			RESbssd(0x40, Reg_IY);
			break;
		case OxFDCBnnBE:
			RESbssd(0x80, Reg_IY);
			break;

		case OxFDCBnnC6:
			SETbssd(0x01, Reg_IY);
			break;
		case OxFDCBnnCE:
			SETbssd(0x02, Reg_IY);
			break;
		case OxFDCBnnD6:
			SETbssd(0x04, Reg_IY);
			break;
		case OxFDCBnnDE:
			SETbssd(0x08, Reg_IY);
			break;
		case OxFDCBnnE6:
			SETbssd(0x10, Reg_IY);
			break;
		case OxFDCBnnEE:
			SETbssd(0x20, Reg_IY);
			break;
		case OxFDCBnnF6:
			SETbssd(0x40, Reg_IY);
			break;
		case OxFDCBnnFE:
			SETbssd(0x80, Reg_IY);
			break;

		default: {
			//			bool normal = false;
			if ((opcode1 == 0xDD) || (opcode1 == 0xFD)) {
				/*
								switch (opcode2) {
									case 0x88:
										normal = true;
										printf("%s: %s (Not implemented: Executing opcode %.2X)\n", logLine, logCode, opcode2);
										m_saveData.registers.IncPC();
										break;
								}
				*/
			}

			//			if (!normal) {
			m_saveData.cycles += 12; // 71400;
			if (m_showNotImplemented) {
#ifdef SLOW
				printf("(SP = %.4X) ", m_saveData.registers.GetSP());
				printf("%s: ", logLine);
				printf("%s ", logCode);
				m_opcode.ShowLogOpcode(this, opcodeEnum);
#endif
				printf(" (Not implemented)\n");
				fflush(stdout);
				m_showNotImplemented = false;
			}
			//			}

			//			assert(0);
			break;
		}
	}

	// Actualizando R
	uint8_t r = m_saveData.registers.GetR();
	r = (r & 0x80) | ((r + 1) & 0x7F);
	m_saveData.registers.SetR(r);

	//	printf("%s\n", logCode);
}

Word CPU::GetAddressBus() const {
	return m_saveData.addressBus;
}

void CPU::SetAddressBus(Word data) {
	m_saveData.addressBus = data;
}

bool CPU::IsEndlessLoop() const {
	return m_saveData.isEndlessLoop;
}

void CPU::PrintLog() {
#ifdef NUMOPCODES
	printf("\n");

	for (int i = 0; i < OxNOTIMPLEMENTED; i++) {
		//		if (opcodesT[i] == 0)
		//			continue;

		m_saveData.registers.SetPC(0);
		printf("%ld : ", opcodesT[i]);
		printf("%d : ", i);
		m_opcode.ShowLogOpcode(this, i);
		printf("\n");
	}

	printf("\n");
#endif
}
