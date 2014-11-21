/*
 * awui/Emulation/MasterSystem/CPU.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "CPU.h"

#include <assert.h>
#include <string.h>
#include <awui/Console.h>
#include <awui/Emulation/MasterSystem/Ports.h>
#include <awui/Emulation/MasterSystem/Registers.h>
#include <awui/Emulation/MasterSystem/Rom.h>
#include <awui/Emulation/MasterSystem/VDP.h>

//#define SLOW
//#define NUMOPCODES

using namespace awui::Emulation;
using namespace awui::Emulation::MasterSystem;

static bool opcodes[OxNOTIMPLEMENTED];

#ifdef NUMOPCODES
static uint64_t opcodesT[OxNOTIMPLEMENTED];
#endif

CPU::CPU() : _opcode(this) {
	this->_vdp = new VDP(this);
	this->_addressBus._w = 0;
	this->_frame = 0;
	this->_oldFrame = 0;
	this->_showLog = false;
	this->_showLogInt = false;
	this->_showNotImplemented = true;
	this->_inInterrupt = false;
	this->_isHalted = false;
	for (int i = 0; i < OxNOTIMPLEMENTED; i++) {
		opcodes[i] = false;
#ifdef NUMOPCODES
		opcodesT[i] = 0;
#endif
	}

	this->_ports->SetVDP(this->_vdp);

	this->Reset();
}

CPU::~CPU() {
	delete this->_vdp;
}

void CPU::Reset() {
	this->_vdp->Reset();

	CPUInst::Reset();
}

void CPU::LoadRom(const String file) {
	this->_rom->LoadRom(file);
}

void CPU::CheckInterrupts() {
	if (!this->_registers->GetIFF1() || !this->_registers->GetIFF2() || !this->_registers->GetInterruptsEnabled())
		return;

	if (this->_vdp->GetInterrupt()) {
//		printf("Entra %d\n", this->_vdp->GetLine());
		this->_inInterrupt = true;
		this->CallInterrupt();
	}
}

// http://www.smspower.org/forums/viewtopic.php?p=69680
// 53693175 / (15 * 228 * 262) ~ 59.922743404 frames per second for NTSC
// 53203424 / (15 * 228 * 313) ~ 49.7014591858 frames per second for PAL
void CPU::OnTick() {
	double fps = this->_vdp->GetNTSC() ? 59.922743404f : 49.7014591858f;
	// Entiendo que debe ser 3.57
	double speed = this->_vdp->GetNTSC() ? 3.579545f : 3.5468949f;
	this->_frame += fps / 59.922743404f; // Refresco de awui

	if ((int) this->_frame == (int) this->_oldFrame)
		return;

	this->_oldFrame = this->_frame;

	double iters = (speed * 1000000.0f) / fps;
	double itersVDP = this->_vdp->GetTotalWidth() * this->_vdp->GetTotalHeight();

	bool vsync = false;
	int vdpCount = 0;
	double vdpIters = 0;

	int realIters = 0;
	for (int i = 0; i < iters; i++) {
		int64_t oldCycles = this->_cycles;
		this->RunOpcode();
		double times = (this->_cycles - oldCycles);
		i = i + times - 1;
		vdpIters += times * (itersVDP / iters);
		if (!vsync) {
			for (; vdpCount < vdpIters; vdpCount++) {
				if (vsync) continue;
				vsync = this->_vdp->OnTick(realIters);
				this->CheckInterrupts();
			}
		}
		realIters++;
	}

//	printf("%d %d\n", realIters, vsync);

	while (!vsync) {
		vsync = this->_vdp->OnTick(realIters);
		this->CheckInterrupts();
	}
}

void CPU::RunOpcode() {
	uint16_t pc = this->_registers->GetPC();

	uint8_t opcode1 = this->ReadMemory(pc);
	uint8_t opcode2;

	this->_opcode.SetByte1(opcode1);

#ifdef SLOW
	char logLine[255];
	char logCode[255];
	char logAux[255];

	sprintf(logLine, "%.4X", pc);
	sprintf(logCode, "%.2X ", opcode1);
#endif

	if ((opcode1 == 0xCB) || (opcode1 == 0xDD) || (opcode1 == 0xED) || (opcode1 == 0xFD)) {
		opcode2 = this->ReadMemory(pc + 1);
		this->_opcode.SetByte2(opcode2);

#ifdef SLOW
		sprintf(logAux, "%.2X ", opcode2);
		strcat(logCode, logAux);
#endif

		if (((opcode1 == 0xDD) || (opcode1 == 0xFD)) && (opcode2 == 0xCB)) {
			uint8_t opcode4 = this->ReadMemory(pc + 3);
			this->_opcode.SetByte4(opcode4);

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

	uint16_t opcodeEnum = this->_opcode.GetEnum();
	uint8_t advance = this->_opcode.GetAdvance();
	if (advance != 0) {
		this->_registers->IncPC(advance);
		return;
	}

#ifdef SLOW
//	if ((pc == 0xc418 && !opcodes[opcodeEnum]) || (this->_showLog && !this->_inInterrupt) || (this->_showLogInt && this->_inInterrupt)) {
	if ((this->_showLog && !this->_inInterrupt) || (this->_showLogInt && this->_inInterrupt)) {
		opcodes[opcodeEnum] = true;
		printf("\n");
		printf("%s: ", logLine);
		printf("%s", logCode);
		this->_opcode.ShowLogOpcode(opcodeEnum);
		printf(" ");
		// printf("\n");
		// printf("AF: %.4X  ", this->_registers->GetAF());
		// printf("BC: %.4X  ", this->_registers->GetBC());
		// printf("DE: %.4X  ", this->_registers->GetDE());
		// printf("HL: %.4X  ", this->_registers->GetHL());
		// printf("IX: %.4X  ", this->_registers->GetIX());
		// printf("IY: %.4X  ", this->_registers->GetIY());
		// printf("\n");
		// printf("PC: %.4X  ", pc);
		// printf("SP: %.4X  ", this->_registers->GetSP());
		// printf("\n");
		// printf("\n");
		// fflush(stdout);
	}
#endif

#ifdef NUMOPCODES
	opcodesT[opcodeEnum]++;
#endif

	// http://clrhome.org/table/
	this->_isHalted = false;
	switch (opcodeEnum) {

/******************************************************************************/
/***************************** Main instructions ******************************/
/******************************************************************************/

		// 00: NOP
		// |1|4| No operation is performed.
		case Ox00:
			this->_registers->IncPC();
			this->_cycles += 4;
			break;

		// 76: HALT
		// |1|4| Suspends CPU operation until an interrupt or reset occurs.
		case Ox76:
			this->_isHalted = true;
			this->_cycles += 4;
			break;

		// LD dd, nn
		case Ox01: this->LDddnn(Reg_BC); break;
		case Ox11: this->LDddnn(Reg_DE); break;
		case Ox21: this->LDddnn(Reg_HL); break;
		case Ox31: this->LDddnn(Reg_SP); break;

		// INC ss
		case Ox03: this->INCss(Reg_BC, 6, 1); break;
		case Ox13: this->INCss(Reg_DE, 6, 1); break;
		case Ox23: this->INCss(Reg_HL, 6, 1); break;
		case Ox33: this->INCss(Reg_SP, 6, 1); break;
		case Ox34: this->INCHL();       break;

		// INC r
		case Ox04: this->INCr(Reg_B, 4, 1); break;
		case Ox0C: this->INCr(Reg_C, 4, 1); break;
		case Ox14: this->INCr(Reg_D, 4, 1); break;
		case Ox1C: this->INCr(Reg_E, 4, 1); break;
		case Ox24: this->INCr(Reg_H, 4, 1); break;
		case Ox2C: this->INCr(Reg_L, 4, 1); break;
		case Ox3C: this->INCr(Reg_A, 4, 1); break;

		// ADD HL, s
		case Ox09: this->ADDXXpp(Reg_HL, Reg_BC, 11, 1); break;
		case Ox19: this->ADDXXpp(Reg_HL, Reg_DE, 11, 1); break;
		case Ox29: this->ADDXXpp(Reg_HL, Reg_HL, 11, 1); break;
		case Ox39: this->ADDXXpp(Reg_HL, Reg_SP, 11, 1); break;

		case Ox07: this->RLCA(); break;
		case Ox0F: this->RRCA(); break;
		case Ox17: this->RLA(); break;
		case Ox1F: this->RRA(); break;
		case Ox27: this->DAA(); break;
		case Ox2F: this->CPL(); break;
		case Ox37: this->SCF(); break;
		case Ox3F: this->CCF(); break;

		// 10 n: DJNZ *
		// |2|13/8| The b register is decremented, and if not zero, the signed value * is added to pc. The jump is measured from the start of the instruction opcode.
		case Ox10:
			{
				uint8_t bDec = this->_registers->GetB() - 1;
				this->_registers->SetB(bDec);
				this->JR(bDec != 0);
				this->_cycles++;
			}
			break;

		case Ox22: this->LDnndd(Reg_HL, 16, 3); break;

		// 36: LD (HL), *
		// |2|10| Loads * into (hl).
		case Ox36:
			this->WriteMemory(this->_registers->GetHL() , this->ReadMemory(pc + 1));
			this->_registers->IncPC(2);
			this->_cycles += 10;
			break;

		// DEC X
		case Ox0B: this->DECss(Reg_BC, 6, 1); break;
		case Ox1B: this->DECss(Reg_DE, 6, 1); break;
		case Ox2B: this->DECss(Reg_HL, 6, 1); break;
		case Ox3B: this->DECss(Reg_SP, 6, 1); break;

		case Ox05: this->DECm (Reg_B, 4, 1);  break;
		case Ox0D: this->DECm (Reg_C, 4, 1);  break;
		case Ox15: this->DECm (Reg_D, 4, 1);  break;
		case Ox1D: this->DECm (Reg_E, 4, 1);  break;
		case Ox25: this->DECm (Reg_H, 4, 1);  break;
		case Ox2D: this->DECm (Reg_L, 4, 1);  break;
		case Ox35: this->DECHL();       break;
		case Ox3D: this->DECm (Reg_A, 4, 1);  break;

		case Ox40: this->LDrr(Reg_B, this->_registers->GetB(), 4, 1); break;
		case Ox41: this->LDrr(Reg_B, this->_registers->GetC(), 4, 1); break;
		case Ox42: this->LDrr(Reg_B, this->_registers->GetD(), 4, 1); break;
		case Ox43: this->LDrr(Reg_B, this->_registers->GetE(), 4, 1); break;
		case Ox44: this->LDrr(Reg_B, this->_registers->GetH(), 4, 1); break;
		case Ox45: this->LDrr(Reg_B, this->_registers->GetL(), 4, 1); break;
		case Ox47: this->LDrr(Reg_B, this->_registers->GetA(), 4, 1); break;
		case Ox48: this->LDrr(Reg_C, this->_registers->GetB(), 4, 1); break;
		case Ox49: this->LDrr(Reg_C, this->_registers->GetC(), 4, 1); break;
		case Ox4A: this->LDrr(Reg_C, this->_registers->GetD(), 4, 1); break;
		case Ox4B: this->LDrr(Reg_C, this->_registers->GetE(), 4, 1); break;
		case Ox4C: this->LDrr(Reg_C, this->_registers->GetH(), 4, 1); break;
		case Ox4D: this->LDrr(Reg_C, this->_registers->GetL(), 4, 1); break;
		case Ox4F: this->LDrr(Reg_C, this->_registers->GetA(), 4, 1); break;
		case Ox50: this->LDrr(Reg_D, this->_registers->GetB(), 4, 1); break;
		case Ox51: this->LDrr(Reg_D, this->_registers->GetC(), 4, 1); break;
		case Ox52: this->LDrr(Reg_D, this->_registers->GetD(), 4, 1); break;
		case Ox53: this->LDrr(Reg_D, this->_registers->GetE(), 4, 1); break;
		case Ox54: this->LDrr(Reg_D, this->_registers->GetH(), 4, 1); break;
		case Ox55: this->LDrr(Reg_D, this->_registers->GetL(), 4, 1); break;
		case Ox57: this->LDrr(Reg_D, this->_registers->GetA(), 4, 1); break;
		case Ox58: this->LDrr(Reg_E, this->_registers->GetB(), 4, 1); break;
		case Ox59: this->LDrr(Reg_E, this->_registers->GetC(), 4, 1); break;
		case Ox5A: this->LDrr(Reg_E, this->_registers->GetD(), 4, 1); break;
		case Ox5B: this->LDrr(Reg_E, this->_registers->GetE(), 4, 1); break;
		case Ox5C: this->LDrr(Reg_E, this->_registers->GetH(), 4, 1); break;
		case Ox5D: this->LDrr(Reg_E, this->_registers->GetL(), 4, 1); break;
		case Ox5F: this->LDrr(Reg_E, this->_registers->GetA(), 4, 1); break;
		case Ox60: this->LDrr(Reg_H, this->_registers->GetB(), 4, 1); break;
		case Ox61: this->LDrr(Reg_H, this->_registers->GetC(), 4, 1); break;
		case Ox62: this->LDrr(Reg_H, this->_registers->GetD(), 4, 1); break;
		case Ox63: this->LDrr(Reg_H, this->_registers->GetE(), 4, 1); break;
		case Ox64: this->LDrr(Reg_H, this->_registers->GetH(), 4, 1); break;
		case Ox65: this->LDrr(Reg_H, this->_registers->GetL(), 4, 1); break;
		case Ox67: this->LDrr(Reg_H, this->_registers->GetA(), 4, 1); break;
		case Ox68: this->LDrr(Reg_L, this->_registers->GetB(), 4, 1); break;
		case Ox69: this->LDrr(Reg_L, this->_registers->GetC(), 4, 1); break;
		case Ox6A: this->LDrr(Reg_L, this->_registers->GetD(), 4, 1); break;
		case Ox6B: this->LDrr(Reg_L, this->_registers->GetE(), 4, 1); break;
		case Ox6C: this->LDrr(Reg_L, this->_registers->GetH(), 4, 1); break;
		case Ox6D: this->LDrr(Reg_L, this->_registers->GetL(), 4, 1); break;
		case Ox6F: this->LDrr(Reg_L, this->_registers->GetA(), 4, 1); break;
		case Ox78: this->LDrr(Reg_A, this->_registers->GetB(), 4, 1); break;
		case Ox79: this->LDrr(Reg_A, this->_registers->GetC(), 4, 1); break;
		case Ox7A: this->LDrr(Reg_A, this->_registers->GetD(), 4, 1); break;
		case Ox7B: this->LDrr(Reg_A, this->_registers->GetE(), 4, 1); break;
		case Ox7C: this->LDrr(Reg_A, this->_registers->GetH(), 4, 1); break;
		case Ox7D: this->LDrr(Reg_A, this->_registers->GetL(), 4, 1); break;
		case Ox7F: this->LDrr(Reg_A, this->_registers->GetA(), 4, 1); break;

		// LD r, (HL)
		case Ox46: this->LDrHL(Reg_B); break;
		case Ox4E: this->LDrHL(Reg_C); break;
		case Ox56: this->LDrHL(Reg_D); break;
		case Ox5E: this->LDrHL(Reg_E); break;
		case Ox66: this->LDrHL(Reg_H); break;
		case Ox6E: this->LDrHL(Reg_L); break;
		case Ox7E: this->LDrHL(Reg_A); break;

		// F9: LD SP, HL
		case OxF9: this->LDSPr(Reg_HL); break;

		// 28 nn: JR X, *
		// |2|12/7| If condition X is true, the signed value * is added to pc. The jump is measured from the start of the instruction opcode.
		case Ox18: this->JR(true); break;
		case Ox20: this->JR(!(this->_registers->GetF() & Flag_Z)); break;
		case Ox28: this->JR(  this->_registers->GetF() & Flag_Z);  break;
		case Ox30: this->JR(!(this->_registers->GetF() & Flag_C)); break;
		case Ox38: this->JR(  this->_registers->GetF() & Flag_C);  break;

		// 32 nn: LD **, A
		// |3|13| Stores a into the memory location pointed to by **.
		case Ox32:
			{
				uint16_t offset = (this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1);
				this->WriteMemory(offset, this->_registers->GetA());
				this->_registers->IncPC(3);
				this->_cycles += 13;
			}
			break;

		// 0A: LD A, (BC)
		// |1|7| Loads the value pointed to by bc into a.
		case Ox0A:
			{
				this->_registers->SetA(this->ReadMemory(this->_registers->GetBC()));
				this->_registers->IncPC();
				this->_cycles += 7;
			}
			break;

		// 1A: LD A, (DE)
		// |1|7| Loads the value pointed to by de into a.
		case Ox1A:
			{
				this->_registers->SetA(this->ReadMemory(this->_registers->GetDE()));
				this->_registers->IncPC();
				this->_cycles += 7;
			}
			break;

		// 3A: LD A, (**)
		// |3|13| Loads the value pointed to by ** into a.
		case Ox3A:
			{
				uint16_t offset = (this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1);
				this->_registers->SetA(this->ReadMemory(offset));
				this->_registers->IncPC(3);
				this->_cycles += 13;
			}
			break;

		// 2A: LD HL, (**)
		case Ox2A: this->LDdd_nn(Reg_HL, 16, 3); break;

		// LD r, *
		case Ox06: this->LDrn(Reg_B, 7, 2); break;
		case Ox0E: this->LDrn(Reg_C, 7, 2); break;
		case Ox16: this->LDrn(Reg_D, 7, 2); break;
		case Ox1E: this->LDrn(Reg_E, 7, 2); break;
		case Ox26: this->LDrn(Reg_H, 7, 2); break;
		case Ox2E: this->LDrn(Reg_L, 7, 2); break;
		case Ox3E: this->LDrn(Reg_A, 7, 2); break;

		// LD (ss), r
		case Ox02: this->LDssr(this->_registers->GetBC(), this->_registers->GetA()); break;
		case Ox12: this->LDssr(this->_registers->GetDE(), this->_registers->GetA()); break;
		case Ox70: this->LDssr(this->_registers->GetHL(), this->_registers->GetB()); break;
		case Ox71: this->LDssr(this->_registers->GetHL(), this->_registers->GetC()); break;
		case Ox72: this->LDssr(this->_registers->GetHL(), this->_registers->GetD()); break;
		case Ox73: this->LDssr(this->_registers->GetHL(), this->_registers->GetE()); break;
		case Ox74: this->LDssr(this->_registers->GetHL(), this->_registers->GetH()); break;
		case Ox75: this->LDssr(this->_registers->GetHL(), this->_registers->GetL()); break;
		case Ox77: this->LDssr(this->_registers->GetHL(), this->_registers->GetA()); break;

		case Ox80: this->ADD(this->_registers->GetB()); break;
		case Ox81: this->ADD(this->_registers->GetC()); break;
		case Ox82: this->ADD(this->_registers->GetD()); break;
		case Ox83: this->ADD(this->_registers->GetE()); break;
		case Ox84: this->ADD(this->_registers->GetH()); break;
		case Ox85: this->ADD(this->_registers->GetL()); break;
		case Ox86: this->ADD(this->ReadMemory(this->_registers->GetHL()), 7); break;
		case Ox87: this->ADD(this->_registers->GetA()); break;
		case OxC6: this->ADD(this->ReadMemory(pc + 1), 7, 2); break;

		case Ox88: this->ADC(this->_registers->GetB()); break;
		case Ox89: this->ADC(this->_registers->GetC()); break;
		case Ox8A: this->ADC(this->_registers->GetD()); break;
		case Ox8B: this->ADC(this->_registers->GetE()); break;
		case Ox8C: this->ADC(this->_registers->GetH()); break;
		case Ox8D: this->ADC(this->_registers->GetL()); break;
		case Ox8E: this->ADC(this->ReadMemory(this->_registers->GetHL()), 7); break;
		case Ox8F: this->ADC(this->_registers->GetA()); break;
		case OxCE: this->ADC(this->ReadMemory(pc + 1), 7, 2); break;

		case Ox90: this->SUB(this->_registers->GetB()); break;
		case Ox91: this->SUB(this->_registers->GetC()); break;
		case Ox92: this->SUB(this->_registers->GetD()); break;
		case Ox93: this->SUB(this->_registers->GetE()); break;
		case Ox94: this->SUB(this->_registers->GetH()); break;
		case Ox95: this->SUB(this->_registers->GetL()); break;
		case Ox96: this->SUB(this->ReadMemory(this->_registers->GetHL()), 7); break;
		case Ox97: this->SUB(this->_registers->GetA()); break;
		case OxD6: this->SUB(this->ReadMemory(pc + 1), 7, 2); break;

		case Ox98: this->SBC(this->_registers->GetB()); break;
		case Ox99: this->SBC(this->_registers->GetC()); break;
		case Ox9A: this->SBC(this->_registers->GetD()); break;
		case Ox9B: this->SBC(this->_registers->GetE()); break;
		case Ox9C: this->SBC(this->_registers->GetH()); break;
		case Ox9D: this->SBC(this->_registers->GetL()); break;
		case Ox9E: this->SBC(this->ReadMemory(this->_registers->GetHL()), 7); break;
		case Ox9F: this->SBC(this->_registers->GetA()); break;
		case OxDE: this->SBC(this->ReadMemory(pc + 1), 7, 2); break;

		case OxA0: this->AND(this->_registers->GetB()); break;
		case OxA1: this->AND(this->_registers->GetC()); break;
		case OxA2: this->AND(this->_registers->GetD()); break;
		case OxA3: this->AND(this->_registers->GetE()); break;
		case OxA4: this->AND(this->_registers->GetH()); break;
		case OxA5: this->AND(this->_registers->GetL()); break;
		case OxA6: this->AND(this->ReadMemory(this->_registers->GetHL()), 7); break;
		case OxA7: this->AND(this->_registers->GetA()); break;
		case OxE6: this->AND(this->ReadMemory(pc + 1), 7, 2); break;

		case OxA8: this->XOR(this->_registers->GetB()); break;
		case OxA9: this->XOR(this->_registers->GetC()); break;
		case OxAA: this->XOR(this->_registers->GetD()); break;
		case OxAB: this->XOR(this->_registers->GetE()); break;
		case OxAC: this->XOR(this->_registers->GetH()); break;
		case OxAD: this->XOR(this->_registers->GetL()); break;
		case OxAE: this->XOR(this->ReadMemory(this->_registers->GetHL()), 7); break;
		case OxAF: this->XOR(this->_registers->GetA()); break;
		case OxEE: this->XOR(this->ReadMemory(pc + 1), 7, 2); break;

		case OxB0: this->OR(this->_registers->GetB()); break;
		case OxB1: this->OR(this->_registers->GetC()); break;
		case OxB2: this->OR(this->_registers->GetD()); break;
		case OxB3: this->OR(this->_registers->GetE()); break;
		case OxB4: this->OR(this->_registers->GetH()); break;
		case OxB5: this->OR(this->_registers->GetL()); break;
		case OxB6: this->OR(this->ReadMemory(this->_registers->GetHL()), 7); break;
		case OxB7: this->OR(this->_registers->GetA()); break;
		case OxF6: this->OR(this->ReadMemory(pc + 1), 7, 2); break;

		case OxB8: this->CP(this->_registers->GetB()); break;
		case OxB9: this->CP(this->_registers->GetC()); break;
		case OxBA: this->CP(this->_registers->GetD()); break;
		case OxBB: this->CP(this->_registers->GetE()); break;
		case OxBC: this->CP(this->_registers->GetH()); break;
		case OxBD: this->CP(this->_registers->GetL()); break;
		case OxBE: this->CP(this->ReadMemory(this->_registers->GetHL()), 7); break;
		case OxBF: this->CP(this->_registers->GetA()); break;
		case OxFE: this->CP(this->ReadMemory(pc + 1), 7, 2); break;

		// JP cc, nn
		case OxC2: this->JPccnn(!(this->_registers->GetF() & Flag_Z));  break;
		case OxCA: this->JPccnn(  this->_registers->GetF() & Flag_Z);   break;
		case OxD2: this->JPccnn(!(this->_registers->GetF() & Flag_C));  break;
		case OxDA: this->JPccnn(  this->_registers->GetF() & Flag_C);   break;
		case OxE2: this->JPccnn(!(this->_registers->GetF() & Flag_V)); break;
		case OxEA: this->JPccnn(  this->_registers->GetF() & Flag_V);  break;
		case OxF2: this->JPccnn(!(this->_registers->GetF() & Flag_S));  break;
		case OxFA: this->JPccnn(  this->_registers->GetF() & Flag_S);   break;

		// C3 nn: JP **
		// |3|10| ** is copied to pc.
		case OxC3:
			{
				uint16_t offset = (this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1);
				if (offset == pc)
					this->_isHalted = true;
				else
					this->_registers->SetPC(offset);
				this->_cycles += 10;
			}
			break;

		case OxC1: this->POP16(Reg_BC, 10, 1); break;
		case OxD1: this->POP16(Reg_DE, 10, 1); break;
		case OxE1: this->POP16(Reg_HL, 10, 1); break;
		case OxF1: this->POP16(Reg_AF, 10, 1); break;

		case OxC5: this->PUSH16(Reg_BC, 11, 1); break;
		case OxD5: this->PUSH16(Reg_DE, 11, 1); break;
		case OxE5: this->PUSH16(Reg_HL, 11, 1); break;
		case OxF5: this->PUSH16(Reg_AF, 11, 1); break;

		// C9: RET
		// |1|10| The top stack entry is popped into pc.
		case OxC0: this->RET(!(this->_registers->GetF() & Flag_Z)); break;
		case OxC8: this->RET( (this->_registers->GetF() & Flag_Z)); break;
		case OxC9:
			this->RET(true, 10);
			this->_inInterrupt = false;
			break;
		case OxD0: this->RET(!(this->_registers->GetF() & Flag_C)); break;
		case OxD8: this->RET( (this->_registers->GetF() & Flag_C)); break;
		case OxE0: this->RET(!(this->_registers->GetF() & Flag_V)); break;
		case OxE8: this->RET( (this->_registers->GetF() & Flag_V)); break;
		case OxF0: this->RET(!(this->_registers->GetF() & Flag_S)); break;
		case OxF8: this->RET( (this->_registers->GetF() & Flag_S)); break;

		// CD nn: CALL **
		case OxC4: this->CALLccnn(!(this->_registers->GetF() & Flag_Z)); break;
		case OxCC: this->CALLccnn( (this->_registers->GetF() & Flag_Z)); break;
		case OxCD: this->CALLnn(); break;
		case OxD4: this->CALLccnn(!(this->_registers->GetF() & Flag_C)); break;
		case OxDC: this->CALLccnn( (this->_registers->GetF() & Flag_C)); break;
		case OxE4: this->CALLccnn(!(this->_registers->GetF() & Flag_V)); break;
		case OxEC: this->CALLccnn( (this->_registers->GetF() & Flag_V)); break;
		case OxF4: this->CALLccnn(!(this->_registers->GetF() & Flag_S)); break;
		case OxFC: this->CALLccnn( (this->_registers->GetF() & Flag_S)); break;

		// RST p
		case OxC7: this->RSTp(0x00); break;
		case OxCF: this->RSTp(0x08); break;
		case OxD7: this->RSTp(0x10); break;
		case OxDF: this->RSTp(0x18); break;
		case OxE7: this->RSTp(0x20); break;
		case OxEF: this->RSTp(0x28); break;
		case OxF7: this->RSTp(0x30); break;
		case OxFF: this->RSTp(0x38); break;

		// D3 *: OUT (*), A
		case OxD3:
			this->OUTnA();
			// printf("Address: %s\n", logLine);
			break;

		// D9: EXX
		// |1|4| Exchanges the 16-bit contents of bc, de, and hl with bc', de', and hl'.
		case OxD9:
			this->_registers->AlternateBC();
			this->_registers->AlternateDE();
			this->_registers->AlternateHL();
			this->_registers->IncPC();
			this->_cycles += 4;
			break;

		// DB n: IN A, *
		// |2|11| A byte from port * is written to a.
		case OxDB:
			{
				this->_registers->SetA(this->_ports->ReadByte(this->ReadMemory(pc + 1)));
//				printf("PORT: %d: %X\n", this->ReadMemory(pc + 1), this->ReadMemory(pc + 1));
				this->_registers->IncPC(2);
				this->_cycles += 11;
			}
			break;

		// E9: JP (HL)
		// |1|4| Loads the value of hl into pc.
		case OxE9:
			{
			    this->_registers->SetPC(this->_registers->GetHL());
				this->_cycles += 4;
			}
			break;

		// 08: EX AF, AF'
		// |1|4| Exchanges the 16-bit contents of af and af'.
		case Ox08:
			{
				this->_registers->AlternateAF();
				this->_registers->IncPC();
				this->_cycles += 4;
			}
			break;

		// EB: EX DE, HL
		// |1|4| Exchanges the 16-bit contents of de and hl.
		case OxEB:
			{
				uint16_t aux = this->_registers->GetDE();
				this->_registers->SetDE(this->_registers->GetHL());
				this->_registers->SetHL(aux);
				this->_registers->IncPC();
				this->_cycles += 4;
			}
			break;

		// E3: EX (SP), HL
		// |1|19| Exchanges (sp) with l, and (sp+1) with h.
		case OxE3:
			{
				uint16_t sp = this->_registers->GetSP();
				uint16_t aux = this->ReadMemory(sp + 1) << 8 | this->ReadMemory(sp);
				this->WriteMemory(sp + 1, this->_registers->GetH());
				this->WriteMemory(sp, this->_registers->GetL());
				this->_registers->SetHL(aux);
				this->_registers->IncPC();
				this->_cycles += 19;
			}
			break;

		// F3 DI
		// |1|4| Resets both interrupt flip-flops, thus prenting maskable interrupts from triggering.
		// I dont know if is completed
		case OxF3:
			this->_registers->IncPC();
			this->_registers->SetIFF1(false);
			this->_registers->SetIFF2(false);
			this->_cycles += 4;
			break;

		// FB EI
		// |1|4| Sets both interrupt flip-flops, thus allowing maskable interrupts to occur. An interrupt will not occur until after the immediatedly following instruction.
		// I dont know if is completed
		case OxFB:
			this->_registers->IncPC();
			this->_registers->SetIFF1(true);
			this->_registers->SetIFF2(true);
			this->_cycles += 4;
			break;

/******************************************************************************/
/************************ Extended instructions (ED) **************************/
/******************************************************************************/

		case OxED44:
		case OxED4C:
		case OxED54:
		case OxED5C:
		case OxED64:
		case OxED6C:
		case OxED74:
		case OxED7C: this->NEG(); break;

		case OxED67: this->RRD(); break;
		case OxED6F: this->RLD(); break;

		// OUT (C), r
		case OxED41: this->OUTCr(Reg_B); break;
		case OxED49: this->OUTCr(Reg_C); break;
		case OxED51: this->OUTCr(Reg_D); break;
		case OxED59: this->OUTCr(Reg_E); break;
		case OxED61: this->OUTCr(Reg_H); break;
		case OxED69: this->OUTCr(Reg_L); break;
		case OxED71: this->OUTC(); break;
		case OxED79: this->OUTCr(Reg_A); break;

		// SBC HL, ss
		case OxED42: this->SBCHLss(Reg_BC); break;
		case OxED52: this->SBCHLss(Reg_DE); break;
		case OxED62: this->SBCHLss(Reg_HL); break;
		case OxED72: this->SBCHLss(Reg_SP); break;

		// LD (nn), dd
		case OxED43: this->LDnndd(Reg_BC); break;
		case OxED53: this->LDnndd(Reg_DE); break;
		case OxED63: this->LDnndd(Reg_HL); break;
		case OxED73: this->LDnndd(Reg_SP); break;

		// LD dd, (nn)
		case OxED4B: this->LDdd_nn(Reg_BC, 20, 4); break;
		case OxED5B: this->LDdd_nn(Reg_DE, 20, 4); break;
		case OxED6B: this->LDdd_nn(Reg_HL, 20, 4); break;
		case OxED7B: this->LDdd_nn(Reg_SP, 20, 4); break;

		case OxED4A: this->ADCHLss(Reg_BC); break;
		case OxED5A: this->ADCHLss(Reg_DE); break;
		case OxED6A: this->ADCHLss(Reg_HL); break;
		case OxED7A: this->ADCHLss(Reg_SP); break;

		// ED46: IM 0
		// ED66: IM 0
		// |2|8| Sets interrupt mode 0.
		case OxED46:
		case OxED66:
			this->_registers->SetIM(0);
			this->_registers->IncPC(2);
			this->_cycles += 8;
			break;

		// ED56: IM 1
		// ED76: IM 1
		// |2|8| Sets interrupt mode 1.
		case OxED56:
		case OxED76:
			this->_registers->SetIM(1);
			this->_registers->IncPC(2);
			this->_cycles += 8;
			break;

		// ED5E: IM 2
		// ED7E: IM 2
		// |2|8| Sets interrupt mode 1.
		case OxED5E:
		case OxED7E:
			this->_registers->SetIM(2);
			this->_registers->IncPC(2);
			this->_cycles += 8;
			break;

		case OxED4D:
			this->RETI();
			this->_inInterrupt = false;
			// printf("Sale %d\n", this->_vdp->GetLine());
			// this->_showLog = false;
			break;

		case OxEDA0: this->LDI(); break;
		case OxEDB0: this->LDIR(); break;
		case OxEDA1: this->CPI(); break;
		case OxEDB1: this->CPIR(); break;
		case OxEDA3: this->OUTI(); break;
		case OxEDA8: this->LDD(); break;
		case OxEDB8: this->LDDR(); break;
		case OxEDA9: this->CPD(); break;
		case OxEDB9: this->CPDR(); break;

		// EDB3: OTIR
		// |2|21/16| A byte from the memory location pointed to by hl is written to port c.
		// Then hl is incremented and b is decremented. If b is not zero, this operation is repeated.
		// Interrupts can trigger while this instruction is processing.
		case OxEDB3:
			{
				uint16_t hl = this->_registers->GetHL();
				uint8_t b = this->_registers->GetB() - 1;
				uint8_t c = this->_registers->GetC();
				this->_addressBus._l = c;
				this->_addressBus._h = b;
				this->_ports->WriteByte(c, this->ReadMemory(hl));
				//printf("Address: %.4X\n", this->_addressBus._w);
				this->_registers->SetHL(hl + 1);
				this->_registers->SetB(b);
				this->_registers->SetFFlag(Flag_N, true);
				this->_registers->SetFFlag(Flag_Z, true);
				if (b == 0) {
					this->_registers->IncPC(2);
					this->_cycles += 16;
				} else
					this->_cycles += 21;
			}
			break;

/******************************************************************************/
/*************************** Bit instructions (CB) ****************************/
/******************************************************************************/

		case OxCB00: this->RLC(Reg_B); break;
		case OxCB01: this->RLC(Reg_C); break;
		case OxCB02: this->RLC(Reg_D); break;
		case OxCB03: this->RLC(Reg_E); break;
		case OxCB04: this->RLC(Reg_H); break;
		case OxCB05: this->RLC(Reg_L); break;
		case OxCB06: this->RLC_HL(); break;
		case OxCB07: this->RLC(Reg_A); break;

		case OxCB08: this->RRC(Reg_B); break;
		case OxCB09: this->RRC(Reg_C); break;
		case OxCB0A: this->RRC(Reg_D); break;
		case OxCB0B: this->RRC(Reg_E); break;
		case OxCB0C: this->RRC(Reg_H); break;
		case OxCB0D: this->RRC(Reg_L); break;
		case OxCB0E: this->RRC_HL(); break;
		case OxCB0F: this->RRC(Reg_A); break;

		case OxCB10: this->RL(Reg_B); break;
		case OxCB11: this->RL(Reg_C); break;
		case OxCB12: this->RL(Reg_D); break;
		case OxCB13: this->RL(Reg_E); break;
		case OxCB14: this->RL(Reg_H); break;
		case OxCB15: this->RL(Reg_L); break;
		case OxCB16: this->RL_HL(); break;
		case OxCB17: this->RL(Reg_A); break;

		case OxCB18: this->RR(Reg_B); break;
		case OxCB19: this->RR(Reg_C); break;
		case OxCB1A: this->RR(Reg_D); break;
		case OxCB1B: this->RR(Reg_E); break;
		case OxCB1C: this->RR(Reg_H); break;
		case OxCB1D: this->RR(Reg_L); break;
		case OxCB1E: this->RR_HL(); break;
		case OxCB1F: this->RR(Reg_A); break;

		case OxCB20: this->SLA(Reg_B); break;
		case OxCB21: this->SLA(Reg_C); break;
		case OxCB22: this->SLA(Reg_D); break;
		case OxCB23: this->SLA(Reg_E); break;
		case OxCB24: this->SLA(Reg_H); break;
		case OxCB25: this->SLA(Reg_L); break;
		case OxCB26: this->SLA_HL(); break;
		case OxCB27: this->SLA(Reg_A); break;

		case OxCB28: this->SRA(Reg_B); break;
		case OxCB29: this->SRA(Reg_C); break;
		case OxCB2A: this->SRA(Reg_D); break;
		case OxCB2B: this->SRA(Reg_E); break;
		case OxCB2C: this->SRA(Reg_H); break;
		case OxCB2D: this->SRA(Reg_L); break;
		case OxCB2E: this->SRA_HL(); break;
		case OxCB2F: this->SRA(Reg_A); break;

		case OxCB30: this->SLL(Reg_B); break;
		case OxCB31: this->SLL(Reg_C); break;
		case OxCB32: this->SLL(Reg_D); break;
		case OxCB33: this->SLL(Reg_E); break;
		case OxCB34: this->SLL(Reg_H); break;
		case OxCB35: this->SLL(Reg_L); break;
		case OxCB36: this->SLL_HL(); break;
		case OxCB37: this->SLL(Reg_A); break;

		case OxCB38: this->SRL(Reg_B); break;
		case OxCB39: this->SRL(Reg_C); break;
		case OxCB3A: this->SRL(Reg_D); break;
		case OxCB3B: this->SRL(Reg_E); break;
		case OxCB3C: this->SRL(Reg_H); break;
		case OxCB3D: this->SRL(Reg_L); break;
		case OxCB3E: this->SRL_HL(); break;
		case OxCB3F: this->SRL(Reg_A); break;

		case OxCB40: this->BIT(this->_registers->GetB(), 0x01); break;
		case OxCB41: this->BIT(this->_registers->GetC(), 0x01); break;
		case OxCB42: this->BIT(this->_registers->GetD(), 0x01); break;
		case OxCB43: this->BIT(this->_registers->GetE(), 0x01); break;
		case OxCB44: this->BIT(this->_registers->GetH(), 0x01); break;
		case OxCB45: this->BIT(this->_registers->GetL(), 0x01); break;
		case OxCB46: this->BITHL(0x01); break;
		case OxCB47: this->BIT(this->_registers->GetA(), 0x01); break;
		case OxCB48: this->BIT(this->_registers->GetB(), 0x02); break;
		case OxCB49: this->BIT(this->_registers->GetC(), 0x02); break;
		case OxCB4A: this->BIT(this->_registers->GetD(), 0x02); break;
		case OxCB4B: this->BIT(this->_registers->GetE(), 0x02); break;
		case OxCB4C: this->BIT(this->_registers->GetH(), 0x02); break;
		case OxCB4D: this->BIT(this->_registers->GetL(), 0x02); break;
		case OxCB4E: this->BITHL(0x02); break;
		case OxCB4F: this->BIT(this->_registers->GetA(), 0x02); break;
		case OxCB50: this->BIT(this->_registers->GetB(), 0x04); break;
		case OxCB51: this->BIT(this->_registers->GetC(), 0x04); break;
		case OxCB52: this->BIT(this->_registers->GetD(), 0x04); break;
		case OxCB53: this->BIT(this->_registers->GetE(), 0x04); break;
		case OxCB54: this->BIT(this->_registers->GetH(), 0x04); break;
		case OxCB55: this->BIT(this->_registers->GetL(), 0x04); break;
		case OxCB56: this->BITHL(0x04); break;
		case OxCB57: this->BIT(this->_registers->GetA(), 0x04); break;
		case OxCB58: this->BIT(this->_registers->GetB(), 0x08); break;
		case OxCB59: this->BIT(this->_registers->GetC(), 0x08); break;
		case OxCB5A: this->BIT(this->_registers->GetD(), 0x08); break;
		case OxCB5B: this->BIT(this->_registers->GetE(), 0x08); break;
		case OxCB5C: this->BIT(this->_registers->GetH(), 0x08); break;
		case OxCB5D: this->BIT(this->_registers->GetL(), 0x08); break;
		case OxCB5E: this->BITHL(0x08); break;
		case OxCB5F: this->BIT(this->_registers->GetA(), 0x08); break;
		case OxCB60: this->BIT(this->_registers->GetB(), 0x10); break;
		case OxCB61: this->BIT(this->_registers->GetC(), 0x10); break;
		case OxCB62: this->BIT(this->_registers->GetD(), 0x10); break;
		case OxCB63: this->BIT(this->_registers->GetE(), 0x10); break;
		case OxCB64: this->BIT(this->_registers->GetH(), 0x10); break;
		case OxCB65: this->BIT(this->_registers->GetL(), 0x10); break;
		case OxCB66: this->BITHL(0x10); break;
		case OxCB67: this->BIT(this->_registers->GetA(), 0x10); break;
		case OxCB68: this->BIT(this->_registers->GetB(), 0x20); break;
		case OxCB69: this->BIT(this->_registers->GetC(), 0x20); break;
		case OxCB6A: this->BIT(this->_registers->GetD(), 0x20); break;
		case OxCB6B: this->BIT(this->_registers->GetE(), 0x20); break;
		case OxCB6C: this->BIT(this->_registers->GetH(), 0x20); break;
		case OxCB6D: this->BIT(this->_registers->GetL(), 0x20); break;
		case OxCB6E: this->BITHL(0x20); break;
		case OxCB6F: this->BIT(this->_registers->GetA(), 0x20); break;
		case OxCB70: this->BIT(this->_registers->GetB(), 0x40); break;
		case OxCB71: this->BIT(this->_registers->GetC(), 0x40); break;
		case OxCB72: this->BIT(this->_registers->GetD(), 0x40); break;
		case OxCB73: this->BIT(this->_registers->GetE(), 0x40); break;
		case OxCB74: this->BIT(this->_registers->GetH(), 0x40); break;
		case OxCB75: this->BIT(this->_registers->GetL(), 0x40); break;
		case OxCB76: this->BITHL(0x40); break;
		case OxCB77: this->BIT(this->_registers->GetA(), 0x40); break;
		case OxCB78: this->BIT(this->_registers->GetB(), 0x80); break;
		case OxCB79: this->BIT(this->_registers->GetC(), 0x80); break;
		case OxCB7A: this->BIT(this->_registers->GetD(), 0x80); break;
		case OxCB7B: this->BIT(this->_registers->GetE(), 0x80); break;
		case OxCB7C: this->BIT(this->_registers->GetH(), 0x80); break;
		case OxCB7D: this->BIT(this->_registers->GetL(), 0x80); break;
		case OxCB7E: this->BITHL(0x80); break;
		case OxCB7F: this->BIT(this->_registers->GetA(), 0x80); break;

		case OxCB80: this->RES(Reg_B, 0x01); break;
		case OxCB81: this->RES(Reg_C, 0x01); break;
		case OxCB82: this->RES(Reg_D, 0x01); break;
		case OxCB83: this->RES(Reg_E, 0x01); break;
		case OxCB84: this->RES(Reg_H, 0x01); break;
		case OxCB85: this->RES(Reg_L, 0x01); break;
		case OxCB86: this->RESHL(0x01); break;
		case OxCB87: this->RES(Reg_A, 0x01); break;
		case OxCB88: this->RES(Reg_B, 0x02); break;
		case OxCB89: this->RES(Reg_C, 0x02); break;
		case OxCB8A: this->RES(Reg_D, 0x02); break;
		case OxCB8B: this->RES(Reg_E, 0x02); break;
		case OxCB8C: this->RES(Reg_H, 0x02); break;
		case OxCB8D: this->RES(Reg_L, 0x02); break;
		case OxCB8E: this->RESHL(0x02); break;
		case OxCB8F: this->RES(Reg_A, 0x02); break;
		case OxCB90: this->RES(Reg_B, 0x04); break;
		case OxCB91: this->RES(Reg_C, 0x04); break;
		case OxCB92: this->RES(Reg_D, 0x04); break;
		case OxCB93: this->RES(Reg_E, 0x04); break;
		case OxCB94: this->RES(Reg_H, 0x04); break;
		case OxCB95: this->RES(Reg_L, 0x04); break;
		case OxCB96: this->RESHL(0x04); break;
		case OxCB97: this->RES(Reg_A, 0x04); break;
		case OxCB98: this->RES(Reg_B, 0x08); break;
		case OxCB99: this->RES(Reg_C, 0x08); break;
		case OxCB9A: this->RES(Reg_D, 0x08); break;
		case OxCB9B: this->RES(Reg_E, 0x08); break;
		case OxCB9C: this->RES(Reg_H, 0x08); break;
		case OxCB9D: this->RES(Reg_L, 0x08); break;
		case OxCB9E: this->RESHL(0x08); break;
		case OxCB9F: this->RES(Reg_A, 0x08); break;
		case OxCBA0: this->RES(Reg_B, 0x10); break;
		case OxCBA1: this->RES(Reg_C, 0x10); break;
		case OxCBA2: this->RES(Reg_D, 0x10); break;
		case OxCBA3: this->RES(Reg_E, 0x10); break;
		case OxCBA4: this->RES(Reg_H, 0x10); break;
		case OxCBA5: this->RES(Reg_L, 0x10); break;
		case OxCBA6: this->RESHL(0x10); break;
		case OxCBA7: this->RES(Reg_A, 0x10); break;
		case OxCBA8: this->RES(Reg_B, 0x20); break;
		case OxCBA9: this->RES(Reg_C, 0x20); break;
		case OxCBAA: this->RES(Reg_D, 0x20); break;
		case OxCBAB: this->RES(Reg_E, 0x20); break;
		case OxCBAC: this->RES(Reg_H, 0x20); break;
		case OxCBAD: this->RES(Reg_L, 0x20); break;
		case OxCBAE: this->RESHL(0x20); break;
		case OxCBAF: this->RES(Reg_A, 0x20); break;
		case OxCBB0: this->RES(Reg_B, 0x40); break;
		case OxCBB1: this->RES(Reg_C, 0x40); break;
		case OxCBB2: this->RES(Reg_D, 0x40); break;
		case OxCBB3: this->RES(Reg_E, 0x40); break;
		case OxCBB4: this->RES(Reg_H, 0x40); break;
		case OxCBB5: this->RES(Reg_L, 0x40); break;
		case OxCBB6: this->RESHL(0x40); break;
		case OxCBB7: this->RES(Reg_A, 0x40); break;
		case OxCBB8: this->RES(Reg_B, 0x80); break;
		case OxCBB9: this->RES(Reg_C, 0x80); break;
		case OxCBBA: this->RES(Reg_D, 0x80); break;
		case OxCBBB: this->RES(Reg_E, 0x80); break;
		case OxCBBC: this->RES(Reg_H, 0x80); break;
		case OxCBBD: this->RES(Reg_L, 0x80); break;
		case OxCBBE: this->RESHL(0x80); break;
		case OxCBBF: this->RES(Reg_A, 0x80); break;

		case OxCBC0: this->SET(Reg_B, 0x01); break;
		case OxCBC1: this->SET(Reg_C, 0x01); break;
		case OxCBC2: this->SET(Reg_D, 0x01); break;
		case OxCBC3: this->SET(Reg_E, 0x01); break;
		case OxCBC4: this->SET(Reg_H, 0x01); break;
		case OxCBC5: this->SET(Reg_L, 0x01); break;
		case OxCBC6: this->SETHL(0x01); break;
		case OxCBC7: this->SET(Reg_A, 0x01); break;
		case OxCBC8: this->SET(Reg_B, 0x02); break;
		case OxCBC9: this->SET(Reg_C, 0x02); break;
		case OxCBCA: this->SET(Reg_D, 0x02); break;
		case OxCBCB: this->SET(Reg_E, 0x02); break;
		case OxCBCC: this->SET(Reg_H, 0x02); break;
		case OxCBCD: this->SET(Reg_L, 0x02); break;
		case OxCBCE: this->SETHL(0x02); break;
		case OxCBCF: this->SET(Reg_A, 0x02); break;
		case OxCBD0: this->SET(Reg_B, 0x04); break;
		case OxCBD1: this->SET(Reg_C, 0x04); break;
		case OxCBD2: this->SET(Reg_D, 0x04); break;
		case OxCBD3: this->SET(Reg_E, 0x04); break;
		case OxCBD4: this->SET(Reg_H, 0x04); break;
		case OxCBD5: this->SET(Reg_L, 0x04); break;
		case OxCBD6: this->SETHL(0x04); break;
		case OxCBD7: this->SET(Reg_A, 0x04); break;
		case OxCBD8: this->SET(Reg_B, 0x08); break;
		case OxCBD9: this->SET(Reg_C, 0x08); break;
		case OxCBDA: this->SET(Reg_D, 0x08); break;
		case OxCBDB: this->SET(Reg_E, 0x08); break;
		case OxCBDC: this->SET(Reg_H, 0x08); break;
		case OxCBDD: this->SET(Reg_L, 0x08); break;
		case OxCBDE: this->SETHL(0x08); break;
		case OxCBDF: this->SET(Reg_A, 0x08); break;
		case OxCBE0: this->SET(Reg_B, 0x10); break;
		case OxCBE1: this->SET(Reg_C, 0x10); break;
		case OxCBE2: this->SET(Reg_D, 0x10); break;
		case OxCBE3: this->SET(Reg_E, 0x10); break;
		case OxCBE4: this->SET(Reg_H, 0x10); break;
		case OxCBE5: this->SET(Reg_L, 0x10); break;
		case OxCBE6: this->SETHL(0x10); break;
		case OxCBE7: this->SET(Reg_A, 0x10); break;
		case OxCBE8: this->SET(Reg_B, 0x20); break;
		case OxCBE9: this->SET(Reg_C, 0x20); break;
		case OxCBEA: this->SET(Reg_D, 0x20); break;
		case OxCBEB: this->SET(Reg_E, 0x20); break;
		case OxCBEC: this->SET(Reg_H, 0x20); break;
		case OxCBED: this->SET(Reg_L, 0x20); break;
		case OxCBEE: this->SETHL(0x20); break;
		case OxCBEF: this->SET(Reg_A, 0x20); break;
		case OxCBF0: this->SET(Reg_B, 0x40); break;
		case OxCBF1: this->SET(Reg_C, 0x40); break;
		case OxCBF2: this->SET(Reg_D, 0x40); break;
		case OxCBF3: this->SET(Reg_E, 0x40); break;
		case OxCBF4: this->SET(Reg_H, 0x40); break;
		case OxCBF5: this->SET(Reg_L, 0x40); break;
		case OxCBF6: this->SETHL(0x40); break;
		case OxCBF7: this->SET(Reg_A, 0x40); break;
		case OxCBF8: this->SET(Reg_B, 0x80); break;
		case OxCBF9: this->SET(Reg_C, 0x80); break;
		case OxCBFA: this->SET(Reg_D, 0x80); break;
		case OxCBFB: this->SET(Reg_E, 0x80); break;
		case OxCBFC: this->SET(Reg_H, 0x80); break;
		case OxCBFD: this->SET(Reg_L, 0x80); break;
		case OxCBFE: this->SETHL(0x80); break;
		case OxCBFF: this->SET(Reg_A, 0x80); break;

/******************************************************************************/
/*************************** IX instructions (DD) *****************************/
/******************************************************************************/

		case OxDD09: this->ADDXXpp(Reg_IX, Reg_BC, 15, 2); break;
		case OxDD19: this->ADDXXpp(Reg_IX, Reg_DE, 15, 2); break;
		case OxDD29: this->ADDXXpp(Reg_IX, Reg_IX, 15, 2); break;
		case OxDD39: this->ADDXXpp(Reg_IX, Reg_SP, 15, 2); break;

		case OxDD86: this->ADD(this->ReadMemory(this->_registers->GetIX() + (int8_t)this->ReadMemory(pc + 2)), 19, 3); break;
		case OxDD8E: this->ADC(this->ReadMemory(this->_registers->GetIX() + (int8_t)this->ReadMemory(pc + 2)), 19, 3); break;
		case OxDD96: this->SUB(this->ReadMemory(this->_registers->GetIX() + (int8_t)this->ReadMemory(pc + 2)), 19, 3); break;
		case OxDD9E: this->SBC(this->ReadMemory(this->_registers->GetIX() + (int8_t)this->ReadMemory(pc + 2)), 19, 3); break;
		case OxDDA6: this->AND(this->ReadMemory(this->_registers->GetIX() + (int8_t)this->ReadMemory(pc + 2)), 19, 3); break;
		case OxDDAE: this->XOR(this->ReadMemory(this->_registers->GetIX() + (int8_t)this->ReadMemory(pc + 2)), 19, 3); break;
		case OxDDB6: this->OR(this->ReadMemory(this->_registers->GetIX() + (int8_t)this->ReadMemory(pc + 2)), 19, 3); break;
		case OxDDBE: this->CP(this->ReadMemory(this->_registers->GetIX() + (int8_t)this->ReadMemory(pc + 2)), 19, 3); break;

		case OxDDE1: this->POP16(Reg_IX, 14, 2); break;
		case OxDDE5: this->PUSH16(Reg_IX, 15, 2); break;
		case OxDD23: this->INCss(Reg_IX, 10, 2); break;
		case OxDD2B: this->DECss(Reg_IX, 10, 2); break;
		case OxDD34: this->INCXXd(Reg_IX); break;
		case OxDD35: this->DECXXd(Reg_IX); break;
		case OxDD36: this->LDXXdn(Reg_IX); break;

		case OxDD21: this->LDddnn(Reg_IX, 4); break;
		case OxDD22: this->LDnndd(Reg_IX); break;
		case OxDD24: this->INCr(Reg_IXH, 8, 2); break;
		case OxDD25: this->DECm(Reg_IXH, 8, 2); break;
		case OxDD2C: this->INCr(Reg_IXL, 8, 2); break;
		case OxDD2D: this->DECm(Reg_IXL, 8, 2); break;
		case OxDD2A: this->LDdd_nn(Reg_IX, 20, 4); break;

		case OxDD46: this->LDrXXd(Reg_B, Reg_IX); break;
		case OxDD4E: this->LDrXXd(Reg_C, Reg_IX); break;
		case OxDD56: this->LDrXXd(Reg_D, Reg_IX); break;
		case OxDD5E: this->LDrXXd(Reg_E, Reg_IX); break;
		case OxDD66: this->LDrXXd(Reg_H, Reg_IX); break;
		case OxDD6E: this->LDrXXd(Reg_L, Reg_IX); break;
		case OxDD7E: this->LDrXXd(Reg_A, Reg_IX); break;

		case OxDD44: this->LDrr(Reg_B,   this->_registers->GetIXH(), 8, 2); break;
		case OxDD45: this->LDrr(Reg_B,   this->_registers->GetIXL(), 8, 2); break;
		case OxDD4C: this->LDrr(Reg_C,   this->_registers->GetIXH(), 8, 2); break;
		case OxDD4D: this->LDrr(Reg_C,   this->_registers->GetIXL(), 8, 2); break;
		case OxDD54: this->LDrr(Reg_D,   this->_registers->GetIXH(), 8, 2); break;
		case OxDD55: this->LDrr(Reg_D,   this->_registers->GetIXL(), 8, 2); break;
		case OxDD5C: this->LDrr(Reg_E,   this->_registers->GetIXH(), 8, 2); break;
		case OxDD5D: this->LDrr(Reg_E,   this->_registers->GetIXL(), 8, 2); break;
		case OxDD60: this->LDrr(Reg_IXH, this->_registers->GetB(),   8, 2); break;
		case OxDD61: this->LDrr(Reg_IXH, this->_registers->GetC(),   8, 2); break;
		case OxDD62: this->LDrr(Reg_IXH, this->_registers->GetD(),   8, 2); break;
		case OxDD63: this->LDrr(Reg_IXH, this->_registers->GetE(),   8, 2); break;
		case OxDD64: this->LDrr(Reg_IXH, this->_registers->GetIXH(), 8, 2); break;
		case OxDD65: this->LDrr(Reg_IXH, this->_registers->GetIXL(), 8, 2); break;
		case OxDD67: this->LDrr(Reg_IXH, this->_registers->GetA(),   8, 2); break;
		case OxDD68: this->LDrr(Reg_IXL, this->_registers->GetB(),   8, 2); break;
		case OxDD69: this->LDrr(Reg_IXL, this->_registers->GetC(),   8, 2); break;
		case OxDD6A: this->LDrr(Reg_IXL, this->_registers->GetD(),   8, 2); break;
		case OxDD6B: this->LDrr(Reg_IXL, this->_registers->GetE(),   8, 2); break;
		case OxDD6C: this->LDrr(Reg_IXL, this->_registers->GetIXH(), 8, 2); break;
		case OxDD6D: this->LDrr(Reg_IXL, this->_registers->GetIXL(), 8, 2); break;
		case OxDD6F: this->LDrr(Reg_IXL, this->_registers->GetA(),   8, 2); break;
		case OxDD7C: this->LDrr(Reg_A,   this->_registers->GetIXH(), 8, 2); break;
		case OxDD7D: this->LDrr(Reg_A,   this->_registers->GetIXL(), 8, 2); break;

		case OxDD70: this->LDXXdr(Reg_IX, Reg_B); break;
		case OxDD71: this->LDXXdr(Reg_IX, Reg_C); break;
		case OxDD72: this->LDXXdr(Reg_IX, Reg_D); break;
		case OxDD73: this->LDXXdr(Reg_IX, Reg_E); break;
		case OxDD74: this->LDXXdr(Reg_IX, Reg_H); break;
		case OxDD75: this->LDXXdr(Reg_IX, Reg_L); break;
		case OxDD77: this->LDXXdr(Reg_IX, Reg_A); break;

		case OxDDF9: this->LDSPr(Reg_IX, 10, 2); break;
		case OxDD26: this->LDrn(Reg_IXH, 11, 3); break;
		case OxDD2E: this->LDrn(Reg_IXL, 11, 3); break;

		case OxDD84: this->ADD(this->_registers->GetIXH(), 8, 2); break;
		case OxDD85: this->ADD(this->_registers->GetIXL(), 8, 2); break;
		case OxDD8C: this->ADC(this->_registers->GetIXH(), 8, 2); break;
		case OxDD8D: this->ADC(this->_registers->GetIXL(), 8, 2); break;
		case OxDD94: this->SUB(this->_registers->GetIXH(), 8, 2); break;
		case OxDD95: this->SUB(this->_registers->GetIXL(), 8, 2); break;
		case OxDD9C: this->SBC(this->_registers->GetIXH(), 8, 2); break;
		case OxDD9D: this->SBC(this->_registers->GetIXL(), 8, 2); break;
		case OxDDA4: this->AND(this->_registers->GetIXH(), 8, 2); break;
		case OxDDA5: this->AND(this->_registers->GetIXL(), 8, 2); break;
		case OxDDAC: this->XOR(this->_registers->GetIXH(), 8, 2); break;
		case OxDDAD: this->XOR(this->_registers->GetIXL(), 8, 2); break;
		case OxDDB4: this->OR(this->_registers->GetIXH(), 8, 2); break;
		case OxDDB5: this->OR(this->_registers->GetIXL(), 8, 2); break;
		case OxDDBC: this->CP(this->_registers->GetIXH(), 8, 2); break;
		case OxDDBD: this->CP(this->_registers->GetIXL(), 8, 2); break;

/******************************************************************************/
/************************* IX bit instructions (DDCB) *************************/
/******************************************************************************/

		case OxDDCBnn06: this->RLCXXd(Reg_IX); break;
		case OxDDCBnn0E: this->RRCXXd(Reg_IX); break;
		case OxDDCBnn16: this->RLXXd(Reg_IX); break;
		case OxDDCBnn1E: this->RRXXd(Reg_IX); break;
		case OxDDCBnn26: this->SLAXXd(Reg_IX); break;
		case OxDDCBnn2E: this->SRAXXd(Reg_IX); break;
		case OxDDCBnn36: this->SLLXXd(Reg_IX); break;
		case OxDDCBnn3E: this->SRLXXd(Reg_IX); break;

		case OxDDCBnn40:
		case OxDDCBnn41:
		case OxDDCBnn42:
		case OxDDCBnn43:
		case OxDDCBnn44:
		case OxDDCBnn45:
		case OxDDCBnn47:
		case OxDDCBnn46: this->BITbssd(0x01, Reg_IX, this->ReadMemory(pc + 2)); break;

		case OxDDCBnn48:
		case OxDDCBnn49:
		case OxDDCBnn4A:
		case OxDDCBnn4B:
		case OxDDCBnn4C:
		case OxDDCBnn4D:
		case OxDDCBnn4F:
		case OxDDCBnn4E: this->BITbssd(0x02, Reg_IX, this->ReadMemory(pc + 2)); break;

		case OxDDCBnn50:
		case OxDDCBnn51:
		case OxDDCBnn52:
		case OxDDCBnn53:
		case OxDDCBnn54:
		case OxDDCBnn55:
		case OxDDCBnn57:
		case OxDDCBnn56: this->BITbssd(0x04, Reg_IX, this->ReadMemory(pc + 2)); break;

		case OxDDCBnn58:
		case OxDDCBnn59:
		case OxDDCBnn5A:
		case OxDDCBnn5B:
		case OxDDCBnn5C:
		case OxDDCBnn5D:
		case OxDDCBnn5F:
		case OxDDCBnn5E: this->BITbssd(0x08, Reg_IX, this->ReadMemory(pc + 2)); break;

		case OxDDCBnn60:
		case OxDDCBnn61:
		case OxDDCBnn62:
		case OxDDCBnn63:
		case OxDDCBnn64:
		case OxDDCBnn65:
		case OxDDCBnn67:
		case OxDDCBnn66: this->BITbssd(0x10, Reg_IX, this->ReadMemory(pc + 2)); break;

		case OxDDCBnn68:
		case OxDDCBnn69:
		case OxDDCBnn6A:
		case OxDDCBnn6B:
		case OxDDCBnn6C:
		case OxDDCBnn6D:
		case OxDDCBnn6F:
		case OxDDCBnn6E: this->BITbssd(0x20, Reg_IX, this->ReadMemory(pc + 2)); break;

		case OxDDCBnn70:
		case OxDDCBnn71:
		case OxDDCBnn72:
		case OxDDCBnn73:
		case OxDDCBnn74:
		case OxDDCBnn75:
		case OxDDCBnn77:
		case OxDDCBnn76: this->BITbssd(0x40, Reg_IX, this->ReadMemory(pc + 2)); break;

		case OxDDCBnn78:
		case OxDDCBnn79:
		case OxDDCBnn7A:
		case OxDDCBnn7B:
		case OxDDCBnn7C:
		case OxDDCBnn7D:
		case OxDDCBnn7F:
		case OxDDCBnn7E: this->BITbssd(0x80, Reg_IX, this->ReadMemory(pc + 2)); break;

		case OxDDCBnn86: this->RESETbssd(0x01, Reg_IX, this->ReadMemory(pc + 2)); break;
		case OxDDCBnn8E: this->RESETbssd(0x02, Reg_IX, this->ReadMemory(pc + 2)); break;
		case OxDDCBnn96: this->RESETbssd(0x04, Reg_IX, this->ReadMemory(pc + 2)); break;
		case OxDDCBnn9E: this->RESETbssd(0x08, Reg_IX, this->ReadMemory(pc + 2)); break;
		case OxDDCBnnA6: this->RESETbssd(0x10, Reg_IX, this->ReadMemory(pc + 2)); break;
		case OxDDCBnnAE: this->RESETbssd(0x20, Reg_IX, this->ReadMemory(pc + 2)); break;
		case OxDDCBnnB6: this->RESETbssd(0x40, Reg_IX, this->ReadMemory(pc + 2)); break;
		case OxDDCBnnBE: this->RESETbssd(0x80, Reg_IX, this->ReadMemory(pc + 2)); break;

		case OxDDCBnnC6: this->SETbssd(0x01, Reg_IX, this->ReadMemory(pc + 2)); break;
		case OxDDCBnnCE: this->SETbssd(0x02, Reg_IX, this->ReadMemory(pc + 2)); break;
		case OxDDCBnnD6: this->SETbssd(0x04, Reg_IX, this->ReadMemory(pc + 2)); break;
		case OxDDCBnnDE: this->SETbssd(0x08, Reg_IX, this->ReadMemory(pc + 2)); break;
		case OxDDCBnnE6: this->SETbssd(0x10, Reg_IX, this->ReadMemory(pc + 2)); break;
		case OxDDCBnnEE: this->SETbssd(0x20, Reg_IX, this->ReadMemory(pc + 2)); break;
		case OxDDCBnnF6: this->SETbssd(0x40, Reg_IX, this->ReadMemory(pc + 2)); break;
		case OxDDCBnnFE: this->SETbssd(0x80, Reg_IX, this->ReadMemory(pc + 2)); break;

/******************************************************************************/
/*************************** IY instructions (FD) *****************************/
/******************************************************************************/

		case OxFD09: this->ADDXXpp(Reg_IY, Reg_BC, 15, 2); break;
		case OxFD19: this->ADDXXpp(Reg_IY, Reg_DE, 15, 2); break;
		case OxFD29: this->ADDXXpp(Reg_IY, Reg_IY, 15, 2); break;
		case OxFD39: this->ADDXXpp(Reg_IY, Reg_SP, 15, 2); break;

		case OxFD86: this->ADD(this->ReadMemory(this->_registers->GetIY() + (int8_t)this->ReadMemory(pc + 2)), 19, 3); break;
		case OxFD8E: this->ADC(this->ReadMemory(this->_registers->GetIY() + (int8_t)this->ReadMemory(pc + 2)), 19, 3); break;
		case OxFD96: this->SUB(this->ReadMemory(this->_registers->GetIY() + (int8_t)this->ReadMemory(pc + 2)), 19, 3); break;
		case OxFD9E: this->SBC(this->ReadMemory(this->_registers->GetIY() + (int8_t)this->ReadMemory(pc + 2)), 19, 3); break;
		case OxFDA6: this->AND(this->ReadMemory(this->_registers->GetIY() + (int8_t)this->ReadMemory(pc + 2)), 19, 3); break;
		case OxFDAE: this->XOR(this->ReadMemory(this->_registers->GetIY() + (int8_t)this->ReadMemory(pc + 2)), 19, 3); break;
		case OxFDB6: this->OR(this->ReadMemory(this->_registers->GetIY() + (int8_t)this->ReadMemory(pc + 2)), 19, 3); break;
		case OxFDBE: this->CP(this->ReadMemory(this->_registers->GetIY() + (int8_t)this->ReadMemory(pc + 2)), 19, 3); break;

		case OxFDE1: this->POP16(Reg_IY, 14, 2); break;
		case OxFDE5: this->PUSH16(Reg_IY, 15, 2); break;
		case OxFD23: this->INCss(Reg_IY, 10, 2); break;
		case OxFD2B: this->DECss(Reg_IY, 10, 2); break;
		case OxFD34: this->INCXXd(Reg_IY); break;
		case OxFD35: this->DECXXd(Reg_IY); break;
		case OxFD36: this->LDXXdn(Reg_IY); break;

		case OxFD21: this->LDddnn(Reg_IY, 4); break;
		case OxFD22: this->LDnndd(Reg_IY); break;
		case OxFD24: this->INCr(Reg_IYH, 8, 2); break;
		case OxFD25: this->DECm(Reg_IYH, 8, 2); break;
		case OxFD2C: this->INCr(Reg_IYL, 8, 2); break;
		case OxFD2D: this->DECm(Reg_IYL, 8, 2); break;
		case OxFD2A: this->LDdd_nn(Reg_IY, 20, 4); break;

		case OxFD46: this->LDrXXd(Reg_B, Reg_IY); break;
		case OxFD4E: this->LDrXXd(Reg_C, Reg_IY); break;
		case OxFD56: this->LDrXXd(Reg_D, Reg_IY); break;
		case OxFD5E: this->LDrXXd(Reg_E, Reg_IY); break;
		case OxFD66: this->LDrXXd(Reg_H, Reg_IY); break;
		case OxFD6E: this->LDrXXd(Reg_L, Reg_IY); break;
		case OxFD7E: this->LDrXXd(Reg_A, Reg_IY); break;

		case OxFD44: this->LDrr(Reg_B,   this->_registers->GetIYH(), 8, 2); break;
		case OxFD45: this->LDrr(Reg_B,   this->_registers->GetIYL(), 8, 2); break;
		case OxFD4C: this->LDrr(Reg_C,   this->_registers->GetIYH(), 8, 2); break;
		case OxFD4D: this->LDrr(Reg_C,   this->_registers->GetIYL(), 8, 2); break;
		case OxFD54: this->LDrr(Reg_D,   this->_registers->GetIYH(), 8, 2); break;
		case OxFD55: this->LDrr(Reg_D,   this->_registers->GetIYL(), 8, 2); break;
		case OxFD5C: this->LDrr(Reg_E,   this->_registers->GetIYH(), 8, 2); break;
		case OxFD5D: this->LDrr(Reg_E,   this->_registers->GetIYL(), 8, 2); break;
		case OxFD60: this->LDrr(Reg_IYH, this->_registers->GetB(),   8, 2); break;
		case OxFD61: this->LDrr(Reg_IYH, this->_registers->GetC(),   8, 2); break;
		case OxFD62: this->LDrr(Reg_IYH, this->_registers->GetD(),   8, 2); break;
		case OxFD63: this->LDrr(Reg_IYH, this->_registers->GetE(),   8, 2); break;
		case OxFD64: this->LDrr(Reg_IYH, this->_registers->GetIYH(), 8, 2); break;
		case OxFD65: this->LDrr(Reg_IYH, this->_registers->GetIYL(), 8, 2); break;
		case OxFD67: this->LDrr(Reg_IYH, this->_registers->GetA(),   8, 2); break;
		case OxFD68: this->LDrr(Reg_IYL, this->_registers->GetB(),   8, 2); break;
		case OxFD69: this->LDrr(Reg_IYL, this->_registers->GetC(),   8, 2); break;
		case OxFD6A: this->LDrr(Reg_IYL, this->_registers->GetD(),   8, 2); break;
		case OxFD6B: this->LDrr(Reg_IYL, this->_registers->GetE(),   8, 2); break;
		case OxFD6C: this->LDrr(Reg_IYL, this->_registers->GetIYH(), 8, 2); break;
		case OxFD6D: this->LDrr(Reg_IYL, this->_registers->GetIYL(), 8, 2); break;
		case OxFD6F: this->LDrr(Reg_IYL, this->_registers->GetA(),   8, 2); break;
		case OxFD7C: this->LDrr(Reg_A,   this->_registers->GetIYH(), 8, 2); break;
		case OxFD7D: this->LDrr(Reg_A,   this->_registers->GetIYL(), 8, 2); break;

		case OxFD70: this->LDXXdr(Reg_IY, Reg_B); break;
		case OxFD71: this->LDXXdr(Reg_IY, Reg_C); break;
		case OxFD72: this->LDXXdr(Reg_IY, Reg_D); break;
		case OxFD73: this->LDXXdr(Reg_IY, Reg_E); break;
		case OxFD74: this->LDXXdr(Reg_IY, Reg_H); break;
		case OxFD75: this->LDXXdr(Reg_IY, Reg_L); break;
		case OxFD77: this->LDXXdr(Reg_IY, Reg_A); break;

		case OxFDF9: this->LDSPr(Reg_IY, 10, 2); break;
		case OxFD26: this->LDrn(Reg_IYH, 11, 3); break;
		case OxFD2E: this->LDrn(Reg_IYL, 11, 3); break;

		case OxFD84: this->ADD(this->_registers->GetIYH(), 8, 2); break;
		case OxFD85: this->ADD(this->_registers->GetIYL(), 8, 2); break;
		case OxFD8C: this->ADC(this->_registers->GetIYH(), 8, 2); break;
		case OxFD8D: this->ADC(this->_registers->GetIYL(), 8, 2); break;
		case OxFD94: this->SUB(this->_registers->GetIYH(), 8, 2); break;
		case OxFD95: this->SUB(this->_registers->GetIYL(), 8, 2); break;
		case OxFD9C: this->SBC(this->_registers->GetIYH(), 8, 2); break;
		case OxFD9D: this->SBC(this->_registers->GetIYL(), 8, 2); break;
		case OxFDA4: this->AND(this->_registers->GetIYH(), 8, 2); break;
		case OxFDA5: this->AND(this->_registers->GetIYL(), 8, 2); break;
		case OxFDAC: this->XOR(this->_registers->GetIYH(), 8, 2); break;
		case OxFDAD: this->XOR(this->_registers->GetIYL(), 8, 2); break;
		case OxFDB4: this->OR(this->_registers->GetIYH(), 8, 2); break;
		case OxFDB5: this->OR(this->_registers->GetIYL(), 8, 2); break;
		case OxFDBC: this->CP(this->_registers->GetIYH(), 8, 2); break;
		case OxFDBD: this->CP(this->_registers->GetIYL(), 8, 2); break;

/******************************************************************************/
/************************* IY bit instructions (FDCB) *************************/
/******************************************************************************/

		case OxFDCBnn06: this->RLCXXd(Reg_IY); break;
		case OxFDCBnn0E: this->RRCXXd(Reg_IY); break;
		case OxFDCBnn16: this->RLXXd(Reg_IY); break;
		case OxFDCBnn1E: this->RRXXd(Reg_IY); break;
		case OxFDCBnn26: this->SLAXXd(Reg_IY); break;
		case OxFDCBnn2E: this->SRAXXd(Reg_IY); break;
		case OxFDCBnn36: this->SLLXXd(Reg_IY); break;
		case OxFDCBnn3E: this->SRLXXd(Reg_IY); break;

		case OxFDCBnn40:
		case OxFDCBnn41:
		case OxFDCBnn42:
		case OxFDCBnn43:
		case OxFDCBnn44:
		case OxFDCBnn45:
		case OxFDCBnn47:
		case OxFDCBnn46: this->BITbssd(0x01, Reg_IY, this->ReadMemory(pc + 2)); break;

		case OxFDCBnn48:
		case OxFDCBnn49:
		case OxFDCBnn4A:
		case OxFDCBnn4B:
		case OxFDCBnn4C:
		case OxFDCBnn4D:
		case OxFDCBnn4F:
		case OxFDCBnn4E: this->BITbssd(0x02, Reg_IY, this->ReadMemory(pc + 2)); break;

		case OxFDCBnn50:
		case OxFDCBnn51:
		case OxFDCBnn52:
		case OxFDCBnn53:
		case OxFDCBnn54:
		case OxFDCBnn55:
		case OxFDCBnn57:
		case OxFDCBnn56: this->BITbssd(0x04, Reg_IY, this->ReadMemory(pc + 2)); break;

		case OxFDCBnn58:
		case OxFDCBnn59:
		case OxFDCBnn5A:
		case OxFDCBnn5B:
		case OxFDCBnn5C:
		case OxFDCBnn5D:
		case OxFDCBnn5F:
		case OxFDCBnn5E: this->BITbssd(0x08, Reg_IY, this->ReadMemory(pc + 2)); break;

		case OxFDCBnn60:
		case OxFDCBnn61:
		case OxFDCBnn62:
		case OxFDCBnn63:
		case OxFDCBnn64:
		case OxFDCBnn65:
		case OxFDCBnn67:
		case OxFDCBnn66: this->BITbssd(0x10, Reg_IY, this->ReadMemory(pc + 2)); break;

		case OxFDCBnn68:
		case OxFDCBnn69:
		case OxFDCBnn6A:
		case OxFDCBnn6B:
		case OxFDCBnn6C:
		case OxFDCBnn6D:
		case OxFDCBnn6F:
		case OxFDCBnn6E: this->BITbssd(0x20, Reg_IY, this->ReadMemory(pc + 2)); break;

		case OxFDCBnn70:
		case OxFDCBnn71:
		case OxFDCBnn72:
		case OxFDCBnn73:
		case OxFDCBnn74:
		case OxFDCBnn75:
		case OxFDCBnn77:
		case OxFDCBnn76: this->BITbssd(0x40, Reg_IY, this->ReadMemory(pc + 2)); break;

		case OxFDCBnn78:
		case OxFDCBnn79:
		case OxFDCBnn7A:
		case OxFDCBnn7B:
		case OxFDCBnn7C:
		case OxFDCBnn7D:
		case OxFDCBnn7F:
		case OxFDCBnn7E: this->BITbssd(0x80, Reg_IY, this->ReadMemory(pc + 2)); break;

		case OxFDCBnn86: this->RESETbssd(0x01, Reg_IY, this->ReadMemory(pc + 2)); break;
		case OxFDCBnn8E: this->RESETbssd(0x02, Reg_IY, this->ReadMemory(pc + 2)); break;
		case OxFDCBnn96: this->RESETbssd(0x04, Reg_IY, this->ReadMemory(pc + 2)); break;
		case OxFDCBnn9E: this->RESETbssd(0x08, Reg_IY, this->ReadMemory(pc + 2)); break;
		case OxFDCBnnA6: this->RESETbssd(0x10, Reg_IY, this->ReadMemory(pc + 2)); break;
		case OxFDCBnnAE: this->RESETbssd(0x20, Reg_IY, this->ReadMemory(pc + 2)); break;
		case OxFDCBnnB6: this->RESETbssd(0x40, Reg_IY, this->ReadMemory(pc + 2)); break;
		case OxFDCBnnBE: this->RESETbssd(0x80, Reg_IY, this->ReadMemory(pc + 2)); break;

		case OxFDCBnnC6: this->SETbssd(0x01, Reg_IY, this->ReadMemory(pc + 2)); break;
		case OxFDCBnnCE: this->SETbssd(0x02, Reg_IY, this->ReadMemory(pc + 2)); break;
		case OxFDCBnnD6: this->SETbssd(0x04, Reg_IY, this->ReadMemory(pc + 2)); break;
		case OxFDCBnnDE: this->SETbssd(0x08, Reg_IY, this->ReadMemory(pc + 2)); break;
		case OxFDCBnnE6: this->SETbssd(0x10, Reg_IY, this->ReadMemory(pc + 2)); break;
		case OxFDCBnnEE: this->SETbssd(0x20, Reg_IY, this->ReadMemory(pc + 2)); break;
		case OxFDCBnnF6: this->SETbssd(0x40, Reg_IY, this->ReadMemory(pc + 2)); break;
		case OxFDCBnnFE: this->SETbssd(0x80, Reg_IY, this->ReadMemory(pc + 2)); break;

		default:
		{
			bool normal = false;
			if ((opcode1 == 0xDD) || (opcode1 == 0xFD)) {
/*
				switch (opcode2) {
					case 0x88:
						normal = true;
						printf("%s: %s (Not implemented: Executing opcode %.2X)\n", logLine, logCode, opcode2);
						this->_registers->IncPC();
						break;
				}
*/
			}

			if (!normal) {
				this->_cycles += 12; // 71400;
				if (this->_showNotImplemented) {
#ifdef SLOW
//					printf("(SP = %.4X) ", this->_registers->GetSP());
					printf("%s: ", logLine);
					printf("%s (Not implemented)\n", logCode);
					fflush(stdout);
#endif
					this->_showNotImplemented = false;
				}
			}

//			assert(0);
			break;
		}
	}

//	printf("%s\n", logCode);
}

uint16_t CPU::GetAddressBus() const {
	return this->_addressBus._w;
}

void CPU::SetAddressBus(uint16_t data) {
	this->_addressBus._w = data;
}

VDP * CPU::GetVDP() {
	return this->_vdp;
}

bool CPU::IsHalted() const {
	return this->_isHalted;
}

void CPU::PrintLog() {
#ifdef NUMOPCODES
	printf("\n");
	for (int i = 0; i < OxNOTIMPLEMENTED; i++) {
//		if (opcodesT[i] == 0)
//			continue;

		this->_registers->SetPC(0);
		this->_opcode.SetByte1(0);
		this->_opcode.SetByte2(0);
		this->_opcode.SetByte4(0);
		printf("%ld : ", opcodesT[i]);
		printf("%d : ", i);
		this->_opcode.ShowLogOpcode(i);
		printf("\n");
	}

	printf("\n");
#endif
}
