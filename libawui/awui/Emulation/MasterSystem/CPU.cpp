/*
 * awui/Emulation/MasterSystem/CPU.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "CPU.h"

#include <assert.h>
#include <awui/Console.h>
#include <awui/Emulation/MasterSystem/Ports.h>
#include <awui/Emulation/MasterSystem/Registers.h>
#include <awui/Emulation/MasterSystem/Rom.h>
#include <awui/Emulation/MasterSystem/VDP.h>

#define Print(...)  if (this->_showLog) printf(__VA_ARGS__);

using namespace awui::Emulation;
using namespace awui::Emulation::MasterSystem;

CPU::CPU() : _opcode(this) {
	this->_vdp = new VDP(this);
	this->_addressBus._w = 0;
	this->_frame = 0;
	this->_oldFrame = 0;
	this->_showLog = false;

	this->_ports->SetVDP(this->_vdp);

	this->Reset();
}

CPU::~CPU() {
	delete this->_vdp;
}

void CPU::LoadRom(const String file) {
	this->_rom->LoadRom(file);
}

void CPU::CheckInterrupts() {
	if (!this->_registers->GetIFF1() || !this->_registers->GetIFF2() || !this->_registers->GetInterruptsEnabled())
		return;

	if (this->_vdp->GetInterrupt()) {
//		printf("Entra %d\n", this->_vdp->GetLine());
		this->CallInterrupt();
	}
}

void CPU::OnTick() {
	float fps = this->_vdp->GetNTSC() ? 60.0f : 50.0f;
	float speed = this->_vdp->GetNTSC() ? 3.57f : 3.54f;
	this->_frame += fps / 60.0f; // Refresco de awui

	if ((int) this->_frame == (int) this->_oldFrame) {
		return;
	}

	this->_oldFrame = this->_frame;

	float iters = (speed * 1000000.0f) / fps;
	float itersVDP = this->_vdp->GetTotalWidth() * this->_vdp->GetTotalHeight();

	bool vsync = false;
	int vdpCount = 0;
	float vdpIters = 0;

	int realIters = 0;
	for (int i = 0; i < iters; i++) {
		int64_t oldCycles = this->_cycles;
		this->RunOpcode();
		float times = (this->_cycles - oldCycles);
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
	uint8_t opcode1 = this->ReadMemory(this->_registers->GetPC());
	this->_opcode.SetByte1(opcode1);

	Print("%.4X: %.2X ", this->_registers->GetPC(), opcode1);

	if ((opcode1 == 0xCB) || (opcode1 == 0xDD) || (opcode1 == 0xED) || (opcode1 == 0xFD)) {
		uint8_t opcode2 = this->ReadMemory(this->_registers->GetPC() + 1);
		this->_opcode.SetByte2(opcode2);
		Print("%.2X ", opcode2);
		if (((opcode1 == 0xDD) || (opcode1 == 0xFD)) && (opcode2 == 0xCB)) {
			uint8_t opcode4 = this->ReadMemory(this->_registers->GetPC() + 3);
			this->_opcode.SetByte4(opcode4);
			Print("%.2X ", opcode4);
		} else
			Print("   ");
	} else
		Print("      ");

	uint16_t opcodeEnum = this->_opcode.GetEnum();

	if (this->_showLog)
		this->_opcode.ShowLogOpcode(opcodeEnum);

	Print("\n");

	// http://clrhome.org/table/
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
			this->_cycles += 4;
			break;

		// LD dd, nn
		case Ox01: this->LDddnn(Reg_BC); break;
		case Ox11: this->LDddnn(Reg_DE); break;
		case Ox21: this->LDddnn(Reg_HL); break;
		case Ox31: this->LDddnn(Reg_SP); break;

		// INC ss
		case Ox03: this->INCss(Reg_BC); break;
		case Ox13: this->INCss(Reg_DE); break;
		case Ox23: this->INCss(Reg_HL); break;
		case Ox33: this->INCss(Reg_SP); break;
		case Ox34: this->INCHL();       break;

		// INC r
		case Ox04: this->INCr(Reg_B); break;
		case Ox0C: this->INCr(Reg_C); break;
		case Ox14: this->INCr(Reg_D); break;
		case Ox1C: this->INCr(Reg_E); break;
		case Ox24: this->INCr(Reg_H); break;
		case Ox2C: this->INCr(Reg_L); break;
		case Ox3C: this->INCr(Reg_A); break;

		// ADD HL, s
		case Ox09: this->ADDHLss(Reg_BC); break;
		case Ox19: this->ADDHLss(Reg_DE); break;
		case Ox29: this->ADDHLss(Reg_HL); break;
		case Ox39: this->ADDHLss(Reg_SP); break;

		// RLCA
		case Ox07: this->RLCA(); break;

		// RRCA
		case Ox0F: this->RRCA(); break;

		// RLA
		case Ox17: this->RLA(); break;

		// RRA
		case Ox1F: this->RRA(); break;

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

		// 22: LD (**), HL
		// |3|16| Stores hl into the memory location pointed to by **
		case Ox22:
			{
				uint16_t pc = this->_registers->GetPC();
				uint16_t offset = (this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1);
				uint16_t hl = this->_registers->GetHL();
				uint8_t h = hl >> 8;
				uint8_t l = hl;
				this->WriteMemory(offset, l);
				this->WriteMemory(offset + 1, h);
				this->_registers->IncPC(3);
				this->_cycles += 16;
			}
			break;

		// 36: LD (HL), *
		// |2|10| Loads * into (hl).
		case Ox36:
			this->WriteMemory(this->_registers->GetHL() , this->ReadMemory(this->_registers->GetPC() + 1));
			this->_registers->IncPC(2);
			this->_cycles += 10;
			break;

		// DEC X
		case Ox05: this->DECm (Reg_B);  break;
		case Ox0B: this->DECss(Reg_BC); break;
		case Ox0D: this->DECm (Reg_C);  break;
		case Ox15: this->DECm (Reg_D);  break;
		case Ox1B: this->DECss(Reg_DE); break;
		case Ox1D: this->DECm (Reg_E);  break;
		case Ox25: this->DECm (Reg_H);  break;
		case Ox2B: this->DECss(Reg_HL); break;
		case Ox2D: this->DECm (Reg_L);  break;
		case Ox35: this->DECHL();       break;
		case Ox3B: this->DECss(Reg_SP); break;
		case Ox3D: this->DECm (Reg_A);  break;

		// LD r, r'
		case Ox40: this->LDrr(Reg_B, Reg_B); break;
		case Ox41: this->LDrr(Reg_B, Reg_C); break;
		case Ox42: this->LDrr(Reg_B, Reg_D); break;
		case Ox43: this->LDrr(Reg_B, Reg_E); break;
		case Ox44: this->LDrr(Reg_B, Reg_H); break;
		case Ox45: this->LDrr(Reg_B, Reg_L); break;
		case Ox47: this->LDrr(Reg_B, Reg_A); break;
		case Ox48: this->LDrr(Reg_C, Reg_B); break;
		case Ox49: this->LDrr(Reg_C, Reg_C); break;
		case Ox4A: this->LDrr(Reg_C, Reg_D); break;
		case Ox4B: this->LDrr(Reg_C, Reg_E); break;
		case Ox4C: this->LDrr(Reg_C, Reg_H); break;
		case Ox4D: this->LDrr(Reg_C, Reg_L); break;
		case Ox4F: this->LDrr(Reg_C, Reg_A); break;
		case Ox50: this->LDrr(Reg_D, Reg_B); break;
		case Ox51: this->LDrr(Reg_D, Reg_C); break;
		case Ox52: this->LDrr(Reg_D, Reg_D); break;
		case Ox53: this->LDrr(Reg_D, Reg_E); break;
		case Ox54: this->LDrr(Reg_D, Reg_H); break;
		case Ox55: this->LDrr(Reg_D, Reg_L); break;
		case Ox57: this->LDrr(Reg_D, Reg_A); break;
		case Ox58: this->LDrr(Reg_E, Reg_B); break;
		case Ox59: this->LDrr(Reg_E, Reg_C); break;
		case Ox5A: this->LDrr(Reg_E, Reg_D); break;
		case Ox5B: this->LDrr(Reg_E, Reg_E); break;
		case Ox5C: this->LDrr(Reg_E, Reg_H); break;
		case Ox5D: this->LDrr(Reg_E, Reg_L); break;
		case Ox5F: this->LDrr(Reg_E, Reg_A); break;
		case Ox60: this->LDrr(Reg_H, Reg_B); break;
		case Ox61: this->LDrr(Reg_H, Reg_C); break;
		case Ox62: this->LDrr(Reg_H, Reg_D); break;
		case Ox63: this->LDrr(Reg_H, Reg_E); break;
		case Ox64: this->LDrr(Reg_H, Reg_H); break;
		case Ox65: this->LDrr(Reg_H, Reg_L); break;
		case Ox67: this->LDrr(Reg_H, Reg_A); break;
		case Ox68: this->LDrr(Reg_L, Reg_B); break;
		case Ox69: this->LDrr(Reg_L, Reg_C); break;
		case Ox6A: this->LDrr(Reg_L, Reg_D); break;
		case Ox6B: this->LDrr(Reg_L, Reg_E); break;
		case Ox6C: this->LDrr(Reg_L, Reg_H); break;
		case Ox6D: this->LDrr(Reg_L, Reg_L); break;
		case Ox6F: this->LDrr(Reg_L, Reg_A); break;
		case Ox78: this->LDrr(Reg_A, Reg_B); break;
		case Ox79: this->LDrr(Reg_A, Reg_C); break;
		case Ox7A: this->LDrr(Reg_A, Reg_D); break;
		case Ox7B: this->LDrr(Reg_A, Reg_E); break;
		case Ox7C: this->LDrr(Reg_A, Reg_H); break;
		case Ox7D: this->LDrr(Reg_A, Reg_L); break;
		case Ox7F: this->LDrr(Reg_A, Reg_A); break;

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
		case Ox20: this->JR(!(this->_registers->GetF() & FFlag_Z)); break;
		case Ox28: this->JR(  this->_registers->GetF() & FFlag_Z);  break;
		case Ox30: this->JR(!(this->_registers->GetF() & FFlag_C)); break;
		case Ox38: this->JR(  this->_registers->GetF() & FFlag_C);  break;

		// 32 nn: LD **, A
		// |3|13| Stores a into the memory location pointed to by **.
		case Ox32:
			{
				uint16_t pc = this->_registers->GetPC();
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
				uint16_t pc = this->_registers->GetPC();
				uint16_t offset = (this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1);
				this->_registers->SetA(this->ReadMemory(offset));
				this->_registers->IncPC(3);
				this->_cycles += 13;
			}
			break;

		// 2A: LD HL, (**)
		// |3|16| Loads the value pointed to by ** into hl.
		case Ox2A:
			{
				uint16_t pc = this->_registers->GetPC();
				uint16_t offset = (this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1);
				this->_registers->SetHL((this->ReadMemory(offset + 1) << 8) | this->ReadMemory(offset));
				this->_registers->IncPC(3);
				this->_cycles += 16;
			}
			break;

		// CPL
		case Ox2F: this->CPL(); break;
		case Ox3F: this->CCF(); break;

		// LD r, *
		case Ox06: this->LDrn(Reg_B); break;
		case Ox0E: this->LDrn(Reg_C); break;
		case Ox16: this->LDrn(Reg_D); break;
		case Ox1E: this->LDrn(Reg_E); break;
		case Ox26: this->LDrn(Reg_H); break;
		case Ox2E: this->LDrn(Reg_L); break;
		case Ox3E: this->LDrn(Reg_A); break;

		// LD (ss), r
		case Ox02: this->LDssr(Reg_A, Reg_BC); break;
		case Ox12: this->LDssr(Reg_A, Reg_DE); break;
		case Ox70: this->LDssr(Reg_B, Reg_HL); break;
		case Ox71: this->LDssr(Reg_C, Reg_HL); break;
		case Ox72: this->LDssr(Reg_D, Reg_HL); break;
		case Ox73: this->LDssr(Reg_E, Reg_HL); break;
		case Ox74: this->LDssr(Reg_H, Reg_HL); break;
		case Ox75: this->LDssr(Reg_L, Reg_HL); break;
		case Ox77: this->LDssr(Reg_A, Reg_HL); break;

		// ADD s
		case Ox80: this->ADD(this->_registers->GetB()); break;
		case Ox81: this->ADD(this->_registers->GetC()); break;
		case Ox82: this->ADD(this->_registers->GetD()); break;
		case Ox83: this->ADD(this->_registers->GetE()); break;
		case Ox84: this->ADD(this->_registers->GetH()); break;
		case Ox85: this->ADD(this->_registers->GetL()); break;
		case Ox86: this->ADD(this->ReadMemory(this->_registers->GetHL()), 7); break;
		case Ox87: this->ADD(this->_registers->GetA()); break;
		case OxC6: this->ADD(this->ReadMemory(this->_registers->GetPC() + 1), 7, 2); break;

		// CP s
		case OxB8: this->CP(this->_registers->GetB()); break;
		case OxB9: this->CP(this->_registers->GetC()); break;
		case OxBA: this->CP(this->_registers->GetD()); break;
		case OxBB: this->CP(this->_registers->GetE()); break;
		case OxBC: this->CP(this->_registers->GetH()); break;
		case OxBD: this->CP(this->_registers->GetL()); break;
		case OxBE: this->CP(this->ReadMemory(this->_registers->GetHL()), 7); break;
		case OxBF: this->CP(this->_registers->GetA()); break;
		case OxFE: this->CP(this->ReadMemory(this->_registers->GetPC() + 1), 7, 2); break;

		// XOR s
		case OxA8: this->XOR(this->_registers->GetB()); break;
		case OxA9: this->XOR(this->_registers->GetC()); break;
		case OxAA: this->XOR(this->_registers->GetD()); break;
		case OxAB: this->XOR(this->_registers->GetE()); break;
		case OxAC: this->XOR(this->_registers->GetH()); break;
		case OxAD: this->XOR(this->_registers->GetL()); break;
		case OxAE: this->XOR(this->ReadMemory(this->_registers->GetHL()), 7); break;
		case OxAF: this->XOR(this->_registers->GetA()); break;
		case OxEE: this->XOR(this->ReadMemory(this->_registers->GetPC() + 1), 7, 2); break;

		// ADC s
		case Ox88: this->ADC(this->_registers->GetB()); break;
		case Ox89: this->ADC(this->_registers->GetC()); break;
		case Ox8A: this->ADC(this->_registers->GetD()); break;
		case Ox8B: this->ADC(this->_registers->GetE()); break;
		case Ox8C: this->ADC(this->_registers->GetH()); break;
		case Ox8D: this->ADC(this->_registers->GetL()); break;
		case Ox8E: this->ADC(this->ReadMemory(this->_registers->GetHL()), 7); break;
		case Ox8F: this->ADC(this->_registers->GetA()); break;
		case OxCE: this->ADC(this->ReadMemory(this->_registers->GetPC() + 1), 7, 2); break;

		// SUB s
		case Ox90: this->SUB(this->_registers->GetB()); break;
		case Ox91: this->SUB(this->_registers->GetC()); break;
		case Ox92: this->SUB(this->_registers->GetD()); break;
		case Ox93: this->SUB(this->_registers->GetE()); break;
		case Ox94: this->SUB(this->_registers->GetH()); break;
		case Ox95: this->SUB(this->_registers->GetL()); break;
		case Ox96: this->SUB(this->ReadMemory(this->_registers->GetHL()), 7); break;
		case Ox97: this->SUB(this->_registers->GetA()); break;
		case OxD6: this->SUB(this->ReadMemory(this->_registers->GetPC() + 1), 7, 2); break;

		// SBC s
		case Ox98: this->SBC(this->_registers->GetB()); break;
		case Ox99: this->SBC(this->_registers->GetC()); break;
		case Ox9A: this->SBC(this->_registers->GetD()); break;
		case Ox9B: this->SBC(this->_registers->GetE()); break;
		case Ox9C: this->SBC(this->_registers->GetH()); break;
		case Ox9D: this->SBC(this->_registers->GetL()); break;
		case Ox9E: this->SBC(this->ReadMemory(this->_registers->GetHL()), 7); break;
		case Ox9F: this->SBC(this->_registers->GetA()); break;
		case OxDE: this->SBC(this->ReadMemory(this->_registers->GetPC() + 1), 7, 2); break;

		// OR s
		case OxB0: this->OR(this->_registers->GetB()); break;
		case OxB1: this->OR(this->_registers->GetC()); break;
		case OxB2: this->OR(this->_registers->GetD()); break;
		case OxB3: this->OR(this->_registers->GetE()); break;
		case OxB4: this->OR(this->_registers->GetH()); break;
		case OxB5: this->OR(this->_registers->GetL()); break;
		case OxB6: this->OR(this->ReadMemory(this->_registers->GetHL()), 7); break;
		case OxB7: this->OR(this->_registers->GetA()); break;
		case OxF6: this->OR(this->ReadMemory(this->_registers->GetPC() + 1), 7, 2); break;

		// AND s
		case OxA0: this->AND(this->_registers->GetB()); break;
		case OxA1: this->AND(this->_registers->GetC()); break;
		case OxA2: this->AND(this->_registers->GetD()); break;
		case OxA3: this->AND(this->_registers->GetE()); break;
		case OxA4: this->AND(this->_registers->GetH()); break;
		case OxA5: this->AND(this->_registers->GetL()); break;
		case OxA6: this->AND(this->ReadMemory(this->_registers->GetHL()), 7); break;
		case OxA7: this->AND(this->_registers->GetA()); break;
		case OxE6: this->AND(this->ReadMemory(this->_registers->GetPC() + 1), 7, 2); break;

		// JP cc, nn
		case OxC2: this->JPccnn(!(this->_registers->GetF() & FFlag_Z));  break;
		case OxCA: this->JPccnn(  this->_registers->GetF() & FFlag_Z);   break;
		case OxD2: this->JPccnn(!(this->_registers->GetF() & FFlag_C));  break;
		case OxDA: this->JPccnn(  this->_registers->GetF() & FFlag_C);   break;
		case OxE2: this->JPccnn(!(this->_registers->GetF() & FFlag_PV)); break;
		case OxEA: this->JPccnn(  this->_registers->GetF() & FFlag_PV);  break;
		case OxF2: this->JPccnn(!(this->_registers->GetF() & FFlag_S));  break;
		case OxFA: this->JPccnn(  this->_registers->GetF() & FFlag_S);   break;

		// C3 nn: JP **
		// |3|10| ** is copied to pc.
		case OxC3:
			{
				uint16_t pc = this->_registers->GetPC();
				uint16_t offset = (this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1);
				if (offset == pc) {
						if (this->_showLog) {
							Console::WriteLine(" --- ROM FINISHED --- ");
							this->_showLog = false;
						}
				}

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
		case OxC0: this->RET(!(this->_registers->GetF() & FFlag_Z)); break;
		case OxC8: this->RET( (this->_registers->GetF() & FFlag_Z)); break;
		case OxC9: this->RET(true, 10); break;
		case OxD0: this->RET(!(this->_registers->GetF() & FFlag_C)); break;
		case OxD8: this->RET( (this->_registers->GetF() & FFlag_C)); break;
		case OxE0: this->RET(!(this->_registers->GetF() & FFlag_PV)); break;
		case OxE8: this->RET( (this->_registers->GetF() & FFlag_PV)); break;
		case OxF0: this->RET(!(this->_registers->GetF() & FFlag_S)); break;
		case OxF8: this->RET( (this->_registers->GetF() & FFlag_S)); break;

		// CD nn: CALL **
		case OxC4: this->CALLccnn(!(this->_registers->GetF() & FFlag_Z)); break;
		case OxCC: this->CALLccnn( (this->_registers->GetF() & FFlag_Z)); break;
		case OxCD: this->CALLnn(); break;
		case OxD4: this->CALLccnn(!(this->_registers->GetF() & FFlag_C)); break;
		case OxDC: this->CALLccnn( (this->_registers->GetF() & FFlag_C)); break;
		case OxE4: this->CALLccnn(!(this->_registers->GetF() & FFlag_PV)); break;
		case OxEC: this->CALLccnn( (this->_registers->GetF() & FFlag_PV)); break;
		case OxF4: this->CALLccnn(!(this->_registers->GetF() & FFlag_S)); break;
		case OxFC: this->CALLccnn( (this->_registers->GetF() & FFlag_S)); break;

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
		case OxD3: this->OUTnA(); break;

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
				uint16_t pc = this->_registers->GetPC();
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
				uint16_t hl = this->_registers->GetHL();
				uint16_t aux = this->ReadMemory(hl + 1) << 8 | this->ReadMemory(hl);
			    this->_registers->SetPC(aux);
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
		// |1|4| Exchanges the 16-bit contents of de and hl.
		case OxE3:
			{
				uint16_t sp = this->_registers->GetSP();
				uint16_t aux = this->ReadMemory(sp + 1) << 8 | this->ReadMemory(sp);
				this->WriteMemory(sp + 1, this->_registers->GetH());
				this->WriteMemory(sp, this->_registers->GetL());
				this->_registers->SetHL(aux);
				this->_registers->IncPC();
				this->_cycles += 4;
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
		case OxED4B: this->LDdd_nn(Reg_BC); break;
		case OxED5B: this->LDdd_nn(Reg_DE); break;
		case OxED6B: this->LDdd_nn(Reg_HL); break;
		case OxED7B: this->LDdd_nn(Reg_SP); break;

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

		case OxED4D: this->RETI(); /* printf("Sale %d\n", this->_vdp->GetLine()); */ break;

		// EDB0: LDIR
		// |2|21/16| Transfers a byte of data from the memory location pointed to by hl to the memory location pointed to by de.
		// Then hl and de are incremented and bc is decremented.
		// If bc is not zero, this operation is repeated. Interrupts can trigger while this instruction is processing.
		case OxEDB0:
			{
				uint16_t hl = this->_registers->GetHL();
				uint16_t de = this->_registers->GetDE();
				uint16_t bc = this->_registers->GetBC() - 1;
				this->WriteMemory(de, this->ReadMemory(hl));
				this->_registers->SetHL(hl + 1);
				this->_registers->SetDE(de + 1);
				this->_registers->SetBC(bc);

				this->_registers->SetFFlag(FFlag_N, false);
				this->_registers->SetFFlag(FFlag_PV, false);
				this->_registers->SetFFlag(FFlag_H, false);

				if (bc == 0) {
					this->_registers->IncPC(2);
					this->_cycles += 16;
				} else
					this->_cycles += 21;
			}
			break;

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
				// printf("Address: %.4X\n", this->_addressBus._w);
				this->_registers->SetHL(hl + 1);
				this->_registers->SetB(b);
				this->_registers->SetFFlag(FFlag_N, true);
				this->_registers->SetFFlag(FFlag_Z, true);
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

		// RL reg
		case OxCB10: this->RL(Reg_B); break;
		case OxCB11: this->RL(Reg_C); break;
		case OxCB12: this->RL(Reg_D); break;
		case OxCB13: this->RL(Reg_E); break;
		case OxCB14: this->RL(Reg_H); break;
		case OxCB15: this->RL(Reg_L); break;
		case OxCB17: this->RL(Reg_A); break;

		// RR reg
		case OxCB18: this->RR(Reg_B); break;
		case OxCB19: this->RR(Reg_C); break;
		case OxCB1A: this->RR(Reg_D); break;
		case OxCB1B: this->RR(Reg_E); break;
		case OxCB1C: this->RR(Reg_H); break;
		case OxCB1D: this->RR(Reg_L); break;
		case OxCB1F: this->RR(Reg_A); break;

		// SLA reg
		case OxCB20: this->SLA(Reg_B); break;
		case OxCB21: this->SLA(Reg_C); break;
		case OxCB22: this->SLA(Reg_D); break;
		case OxCB23: this->SLA(Reg_E); break;
		case OxCB24: this->SLA(Reg_H); break;
		case OxCB25: this->SLA(Reg_L); break;
		case OxCB27: this->SLA(Reg_A); break;

		// SRA reg
		case OxCB28: this->SRA(Reg_B); break;
		case OxCB29: this->SRA(Reg_C); break;
		case OxCB2A: this->SRA(Reg_D); break;
		case OxCB2B: this->SRA(Reg_E); break;
		case OxCB2C: this->SRA(Reg_H); break;
		case OxCB2D: this->SRA(Reg_L); break;
		case OxCB2F: this->SRA(Reg_A); break;

		// SRL reg
		case OxCB38: this->SRA(Reg_B); break;
		case OxCB39: this->SRA(Reg_C); break;
		case OxCB3A: this->SRA(Reg_D); break;
		case OxCB3B: this->SRA(Reg_E); break;
		case OxCB3C: this->SRA(Reg_H); break;
		case OxCB3D: this->SRA(Reg_L); break;
		case OxCB3F: this->SRA(Reg_A); break;

		// CB40-CB7F: BIT X, Y
		// |2|8| Tests bit X of Y.
		case OxCB40: this->BIT(this->_registers->GetB(), 0x01); break;
		case OxCB41: this->BIT(this->_registers->GetC(), 0x01); break;
		case OxCB42: this->BIT(this->_registers->GetD(), 0x01); break;
		case OxCB43: this->BIT(this->_registers->GetE(), 0x01); break;
		case OxCB44: this->BIT(this->_registers->GetH(), 0x01); break;
		case OxCB45: this->BIT(this->_registers->GetL(), 0x01); break;
		case OxCB46: this->BIT(this->ReadMemory(this->_registers->GetHL()), 0x01, 12); break;
		case OxCB47: this->BIT(this->_registers->GetA(), 0x01); break;
		case OxCB48: this->BIT(this->_registers->GetB(), 0x02); break;
		case OxCB49: this->BIT(this->_registers->GetC(), 0x02); break;
		case OxCB4A: this->BIT(this->_registers->GetD(), 0x02); break;
		case OxCB4B: this->BIT(this->_registers->GetE(), 0x02); break;
		case OxCB4C: this->BIT(this->_registers->GetH(), 0x02); break;
		case OxCB4D: this->BIT(this->_registers->GetL(), 0x02); break;
		case OxCB4E: this->BIT(this->ReadMemory(this->_registers->GetHL()), 0x02, 12); break;
		case OxCB4F: this->BIT(this->_registers->GetA(), 0x02); break;
		case OxCB50: this->BIT(this->_registers->GetB(), 0x04); break;
		case OxCB51: this->BIT(this->_registers->GetC(), 0x04); break;
		case OxCB52: this->BIT(this->_registers->GetD(), 0x04); break;
		case OxCB53: this->BIT(this->_registers->GetE(), 0x04); break;
		case OxCB54: this->BIT(this->_registers->GetH(), 0x04); break;
		case OxCB55: this->BIT(this->_registers->GetL(), 0x04); break;
		case OxCB56: this->BIT(this->ReadMemory(this->_registers->GetHL()), 0x04, 12); break;
		case OxCB57: this->BIT(this->_registers->GetA(), 0x04); break;
		case OxCB58: this->BIT(this->_registers->GetB(), 0x08); break;
		case OxCB59: this->BIT(this->_registers->GetC(), 0x08); break;
		case OxCB5A: this->BIT(this->_registers->GetD(), 0x08); break;
		case OxCB5B: this->BIT(this->_registers->GetE(), 0x08); break;
		case OxCB5C: this->BIT(this->_registers->GetH(), 0x08); break;
		case OxCB5D: this->BIT(this->_registers->GetL(), 0x08); break;
		case OxCB5E: this->BIT(this->ReadMemory(this->_registers->GetHL()), 0x08, 12); break;
		case OxCB5F: this->BIT(this->_registers->GetA(), 0x08); break;
		case OxCB60: this->BIT(this->_registers->GetB(), 0x10); break;
		case OxCB61: this->BIT(this->_registers->GetC(), 0x10); break;
		case OxCB62: this->BIT(this->_registers->GetD(), 0x10); break;
		case OxCB63: this->BIT(this->_registers->GetE(), 0x10); break;
		case OxCB64: this->BIT(this->_registers->GetH(), 0x10); break;
		case OxCB65: this->BIT(this->_registers->GetL(), 0x10); break;
		case OxCB66: this->BIT(this->ReadMemory(this->_registers->GetHL()), 0x10, 12); break;
		case OxCB67: this->BIT(this->_registers->GetA(), 0x10); break;
		case OxCB68: this->BIT(this->_registers->GetB(), 0x20); break;
		case OxCB69: this->BIT(this->_registers->GetC(), 0x20); break;
		case OxCB6A: this->BIT(this->_registers->GetD(), 0x20); break;
		case OxCB6B: this->BIT(this->_registers->GetE(), 0x20); break;
		case OxCB6C: this->BIT(this->_registers->GetH(), 0x20); break;
		case OxCB6D: this->BIT(this->_registers->GetL(), 0x20); break;
		case OxCB6E: this->BIT(this->ReadMemory(this->_registers->GetHL()), 0x20, 12); break;
		case OxCB6F: this->BIT(this->_registers->GetA(), 0x20); break;
		case OxCB70: this->BIT(this->_registers->GetB(), 0x40); break;
		case OxCB71: this->BIT(this->_registers->GetC(), 0x40); break;
		case OxCB72: this->BIT(this->_registers->GetD(), 0x40); break;
		case OxCB73: this->BIT(this->_registers->GetE(), 0x40); break;
		case OxCB74: this->BIT(this->_registers->GetH(), 0x40); break;
		case OxCB75: this->BIT(this->_registers->GetL(), 0x40); break;
		case OxCB76: this->BIT(this->ReadMemory(this->_registers->GetHL()), 0x40, 12); break;
		case OxCB77: this->BIT(this->_registers->GetA(), 0x40); break;
		case OxCB78: this->BIT(this->_registers->GetB(), 0x80); break;
		case OxCB79: this->BIT(this->_registers->GetC(), 0x80); break;
		case OxCB7A: this->BIT(this->_registers->GetD(), 0x80); break;
		case OxCB7B: this->BIT(this->_registers->GetE(), 0x80); break;
		case OxCB7C: this->BIT(this->_registers->GetH(), 0x80); break;
		case OxCB7D: this->BIT(this->_registers->GetL(), 0x80); break;
		case OxCB7E: this->BIT(this->ReadMemory(this->_registers->GetHL()), 0x80, 12); break;
		case OxCB7F: this->BIT(this->_registers->GetA(), 0x80); break;

		// CB80-CBBF: RES X, Y
		// |2|8| Resets bit X of Y.
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

		// CBC0-CBFF: SET X, Y
		// |2|8| Sets bit X of Y.
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

		case OxDD09: this->ADDXXpp(Reg_IX, Reg_BC); break;
		case OxDD19: this->ADDXXpp(Reg_IX, Reg_DE); break;
		case OxDD29: this->ADDXXpp(Reg_IX, Reg_IX); break;
		case OxDD39: this->ADDXXpp(Reg_IX, Reg_SP); break;

		case OxDDE1: this->POP16(Reg_IX, 14, 2); break;
		case OxDDE5: this->PUSH16(Reg_IX, 15, 2); break;
		case OxDD23: this->INCXX(Reg_IX); break;

		case OxDD21: this->LDddnn(Reg_IX, 4); break;

		case OxDD46: this->LDrXXd(Reg_B, Reg_IX); break;
		case OxDD4E: this->LDrXXd(Reg_C, Reg_IX); break;
		case OxDD56: this->LDrXXd(Reg_D, Reg_IX); break;
		case OxDD5E: this->LDrXXd(Reg_E, Reg_IX); break;
		case OxDD66: this->LDrXXd(Reg_H, Reg_IX); break;
		case OxDD6E: this->LDrXXd(Reg_L, Reg_IX); break;
		case OxDD7E: this->LDrXXd(Reg_A, Reg_IX); break;

		case OxDDF9: this->LDSPr(Reg_IX, 10, 2); break;

/******************************************************************************/
/************************* IX bit instructions (DDCB) *************************/
/******************************************************************************/

		case OxDDCBnnC6: this->SETbssd(0, Reg_IX, this->ReadMemory(this->_registers->GetPC() + 2)); break;
		case OxDDCBnnCE: this->SETbssd(1, Reg_IX, this->ReadMemory(this->_registers->GetPC() + 2)); break;
		case OxDDCBnnD6: this->SETbssd(2, Reg_IX, this->ReadMemory(this->_registers->GetPC() + 2)); break;
		case OxDDCBnnDE: this->SETbssd(3, Reg_IX, this->ReadMemory(this->_registers->GetPC() + 2)); break;
		case OxDDCBnnE6: this->SETbssd(4, Reg_IX, this->ReadMemory(this->_registers->GetPC() + 2)); break;
		case OxDDCBnnEE: this->SETbssd(5, Reg_IX, this->ReadMemory(this->_registers->GetPC() + 2)); break;
		case OxDDCBnnF6: this->SETbssd(6, Reg_IX, this->ReadMemory(this->_registers->GetPC() + 2)); break;
		case OxDDCBnnFE: this->SETbssd(7, Reg_IX, this->ReadMemory(this->_registers->GetPC() + 2)); break;

/******************************************************************************/
/*************************** IY instructions (FD) *****************************/
/******************************************************************************/

		case OxFD09: this->ADDXXpp(Reg_IY, Reg_BC); break;
		case OxFD19: this->ADDXXpp(Reg_IY, Reg_DE); break;
		case OxFD29: this->ADDXXpp(Reg_IY, Reg_IY); break;
		case OxFD39: this->ADDXXpp(Reg_IY, Reg_SP); break;

		case OxFDE1: this->POP16(Reg_IY, 14, 2); break;
		case OxFDE5: this->PUSH16(Reg_IY, 15, 2); break;
		case OxFD23: this->INCXX(Reg_IY); break;

		case OxFD21: this->LDddnn(Reg_IY, 4); break;

		case OxFD46: this->LDrXXd(Reg_B, Reg_IY); break;
		case OxFD4E: this->LDrXXd(Reg_C, Reg_IY); break;
		case OxFD56: this->LDrXXd(Reg_D, Reg_IY); break;
		case OxFD5E: this->LDrXXd(Reg_E, Reg_IY); break;
		case OxFD66: this->LDrXXd(Reg_H, Reg_IY); break;
		case OxFD6E: this->LDrXXd(Reg_L, Reg_IY); break;
		case OxFD7E: this->LDrXXd(Reg_A, Reg_IY); break;

		case OxFDF9: this->LDSPr(Reg_IY, 10, 2); break;

/******************************************************************************/
/************************* IY bit instructions (DDCB) *************************/
/******************************************************************************/

		case OxFDCBnnC6: this->SETbssd(0, Reg_IY, this->ReadMemory(this->_registers->GetPC() + 2)); break;
		case OxFDCBnnCE: this->SETbssd(1, Reg_IY, this->ReadMemory(this->_registers->GetPC() + 2)); break;
		case OxFDCBnnD6: this->SETbssd(2, Reg_IY, this->ReadMemory(this->_registers->GetPC() + 2)); break;
		case OxFDCBnnDE: this->SETbssd(3, Reg_IY, this->ReadMemory(this->_registers->GetPC() + 2)); break;
		case OxFDCBnnE6: this->SETbssd(4, Reg_IY, this->ReadMemory(this->_registers->GetPC() + 2)); break;
		case OxFDCBnnEE: this->SETbssd(5, Reg_IY, this->ReadMemory(this->_registers->GetPC() + 2)); break;
		case OxFDCBnnF6: this->SETbssd(6, Reg_IY, this->ReadMemory(this->_registers->GetPC() + 2)); break;
		case OxFDCBnnFE: this->SETbssd(7, Reg_IY, this->ReadMemory(this->_registers->GetPC() + 2)); break;

		default:
			this->_showLog = true;
			this->_cycles += 12; // 71400;
//			assert(0);
			break;
	}
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

Registers * CPU::GetRegisters() {
	return this->_registers;
}