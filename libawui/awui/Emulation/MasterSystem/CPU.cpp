/*
 * awui/Emulation/MasterSystem/CPU.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "CPU.h"

#include <assert.h>
#include <awui/Console.h>
#include <awui/Emulation/MasterSystem/Ports.h>
#include <awui/Emulation/MasterSystem/Ram.h>
#include <awui/Emulation/MasterSystem/Registers.h>
#include <awui/Emulation/MasterSystem/Rom.h>
#include <awui/Emulation/MasterSystem/VDP.h>

using namespace awui::Emulation::MasterSystem;

CPU::CPU() {
	this->_ram = new Ram(8192);
	this->_registers = new Registers();
	this->_rom = new Rom(4096);
	this->_vdp = new VDP();
	this->_ports = new Ports(this->_vdp);
	this->_cycles = 0;

	this->_frame = 0;
	this->_oldFrame = 0;
	this->_showLog = true;

	this->Reset();
}

CPU::~CPU() {
	delete this->_ports;
	delete this->_vdp;
	delete this->_rom;
	delete this->_registers;
	delete this->_ram;
}

void CPU::Reset() {
	this->_registers->Clear();
}

void CPU::LoadRom(const String file) {
	this->_rom->LoadRom(file);
}

//#define SPEED 3.54f
#define SPEED 3.57f
#define GUIFPS 60.0f
#define FPS 50.0f

void CPU::OnTick() {
	this->_frame += FPS / GUIFPS;

	if ((int) this->_frame == (int) this->_oldFrame) {
		return;
	}

	this->_oldFrame = this->_frame;

	float iters = (SPEED * 1000000.0f) / FPS; // 71400
	float itersVDP = 256 * 192;               // 49152

	bool vsync = false;
	int vdpCount = 0;
	float vdpIters = 0;

	// int realIters = 0;
	for (int i = 0; i < iters; i++) {
		// realIters++;
		int64_t oldCycles = this->_cycles;
		this->RunOpcode();
		int times = this->_cycles - oldCycles;
		i += times - 1;
		vdpIters += times * (itersVDP / iters);
		if (!vsync) {
			for (; vdpCount < vdpIters; vdpCount++) {
				if (vsync) continue;
				vsync = this->_vdp->OnTick();
			}
		}
	}

	// printf("%d\n", realIters);

	while (!vsync)
		vsync = this->_vdp->OnTick();
}

void CPU::RunOpcode() {
	uint8_t opcode1 = this->ReadMemory(this->_registers->GetPC());
	this->_opcode.SetByte1(opcode1);

	if (this->_showLog) printf("%.4X: %.2X", this->_registers->GetPC(), opcode1);
	if ((opcode1 == 0xCB) || (opcode1 == 0xDD) || (opcode1 == 0xED) || (opcode1 == 0xFD)) {
		uint8_t opcode2 = this->ReadMemory(this->_registers->GetPC() + 1);
		this->_opcode.SetByte2(opcode2);
		if (this->_showLog) printf(" %.2X", opcode2);
		if (((opcode1 == 0xDD) || (opcode1 == 0xFD)) && (opcode2 == 0xCB)) {
			uint8_t opcode4 = this->ReadMemory(this->_registers->GetPC() + 3);
			this->_opcode.SetByte4(opcode4);
			if (this->_showLog) printf(" %.2X", opcode4);
		}
	}

	if (this->_showLog) printf("\n");

	// http://clrhome.org/table/
	switch (this->_opcode.GetEnum()) {

/******************************************************************************/
/***************************** Main instructions ******************************/
/******************************************************************************/

		// 00: NOP
		// |1|4| No operation is performed.
		case Ox00:
			this->_registers->IncPC();
			this->_cycles += 4;
			break;

		// LD dd, nn
		case Ox01: this->LDddnn(Reg_BC); break;
		case Ox11: this->LDddnn(Reg_DE); break;
		case Ox21: this->LDddnn(Reg_HL); break;
		case Ox31: this->LDddnn(Reg_SP); break;

		// 02: LD (BC), A
		// |1|7| Stores a into the memory location pointed to by bc.
		case Ox02:
			this->WriteMemory(this->_registers->GetBC(), this->_registers->GetA());
			this->_registers->IncPC();
			this->_cycles += 7;
			break;

		// INC ss
		case Ox03: this->INCss(Reg_BC); break;
		case Ox13: this->INCss(Reg_DE); break;
		case Ox23: this->INCss(Reg_HL); break;
		case Ox33: this->INCss(Reg_SP); break;

		// INC r
		case Ox04: this->INCr(Reg_B); break;
		case Ox0C: this->INCr(Reg_C); break;
		case Ox14: this->INCr(Reg_D); break;
		case Ox1C: this->INCr(Reg_E); break;
		case Ox24: this->INCr(Reg_H); break;
		case Ox2C: this->INCr(Reg_L); break;
		case Ox3C: this->INCr(Reg_A); break;

		// ADD HL, s
		case Ox09: this->ADDHLs(Reg_BC); break;
		case Ox19: this->ADDHLs(Reg_DE); break;
		case Ox29: this->ADDHLs(Reg_HL); break;
		case Ox39: this->ADDHLs(Reg_SP); break;

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
		// |1|6| Loads the value of hl into sp.
		case OxF9:
			this->_registers->SetSP(this->_registers->GetHL());
			this->_registers->IncPC();
			this->_cycles += 6;
			break;

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

		// LD r, *
		case Ox06: this->LDrn(Reg_B); break;
		case Ox0E: this->LDrn(Reg_C); break;
		case Ox16: this->LDrn(Reg_D); break;
		case Ox1E: this->LDrn(Reg_E); break;
		case Ox26: this->LDrn(Reg_H); break;
		case Ox2E: this->LDrn(Reg_L); break;
		case Ox3E: this->LDrn(Reg_A); break;

		// LD (HL), r
		case Ox70: this->LDHLr(Reg_B); break;
		case Ox71: this->LDHLr(Reg_C); break;
		case Ox72: this->LDHLr(Reg_D); break;
		case Ox73: this->LDHLr(Reg_E); break;
		case Ox74: this->LDHLr(Reg_H); break;
		case Ox75: this->LDHLr(Reg_L); break;
		case Ox77: this->LDHLr(Reg_A); break;

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

		// C3 nn: JP **
		// |3|10| ** is copied to pc.
		case OxC3:
			{
				uint16_t pc = this->_registers->GetPC();
				this->_registers->SetPC((this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1));
				this->_cycles += 10;
			}
			break;

		// PUSH qq
		case OxC5: this->PUSHqq(Reg_B, Reg_C); break;
		case OxD5: this->PUSHqq(Reg_D, Reg_E); break;
		case OxE5: this->PUSHqq(Reg_H, Reg_L); break;
		case OxF5: this->PUSHqq(Reg_A, Reg_F); break;

		// POP qq
		case OxC1: this->POPqq(Reg_B, Reg_C); break;
		case OxD1: this->POPqq(Reg_D, Reg_E); break;
		case OxE1: this->POPqq(Reg_H, Reg_L); break;
		case OxF1: this->POPqq(Reg_A, Reg_F); break;

		// C9: RET
		// |1|10| The top stack entry is popped into pc.
		case OxC9:
			{
				uint16_t sp = this->_registers->GetSP();
				uint16_t pc = this->ReadMemory(sp);
				pc |= (this->ReadMemory(sp + 1) << 8);
				this->_registers->SetSP(sp + 2);
				this->_cycles += 10;
				this->_registers->SetPC(pc);
			}
			break;

		// CD nn: CALL **
		// |3|17| The current pc value plus three is pushed onto the stack, then is loaded with **.
		case OxCD:
			{
				uint16_t pc = this->_registers->GetPC() + 3;
				uint16_t sp = this->_registers->GetSP() - 2;
				this->_registers->SetSP(sp);
				this->WriteMemory(sp, pc & 0xFF);
				this->WriteMemory(sp + 1, (pc >> 8) & 0xFF);
				this->_cycles += 17;
				this->_registers->SetPC((this->ReadMemory(pc - 1) << 8) | this->ReadMemory(pc - 2));
			}
			break;

		// D3 *: OUT (*), A
		// |2|11| The value of a is written to port *.
		case OxD3:
			this->_ports->WriteByte(this->ReadMemory(this->_registers->GetPC() + 1), this->_registers->GetA());
			this->_registers->IncPC(2);
			this->_cycles += 11;
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
				uint16_t pc = this->_registers->GetPC();
				this->_registers->SetA(this->_ports->ReadByte(this->ReadMemory(pc + 1)));
//				printf("PORT: %d: %X\n", this->ReadMemory(pc + 1), this->ReadMemory(pc + 1));
				this->_registers->IncPC(2);
				this->_cycles += 11;
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

		// LD (nn), dd
		case OxED43: this->LDnndd(Reg_BC); break;
		case OxED53: this->LDnndd(Reg_DE); break;
		case OxED63: this->LDnndd(Reg_HL); break;
		case OxED73: this->LDnndd(Reg_SP); break;

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
				this->_ports->WriteByte(this->_registers->GetC(), this->ReadMemory(hl));
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

		default:
			this->_showLog = true;
			this->_cycles += 71400;
			break;
	}
}

void CPU::WriteMemory(uint16_t pos, uint8_t value) {
	if (pos <= 0xBFFF)
		this->_rom->WriteByte(pos, value);

	// RAM
	if (pos >= 0xC000) {
		// RAM or RAM (mirror)
		if (pos <= 0xDFFF)
			this->_ram->WriteByte(pos - 0xC000, value);
		else
			this->_ram->WriteByte(pos - 0xE000, value);
	}
}

uint8_t CPU::ReadMemory(uint16_t pos) {
	if (pos <= 0xBFFF)
		return this->_rom->ReadByte(pos);

	// RAM
	if (pos >= 0xC000) {
		// RAM or RAM (mirror)
		if (pos <= 0xDFFF)
			return this->_ram->ReadByte(pos - 0xC000);
		else
			return this->_ram->ReadByte(pos - 0xE000);
	}

	return 0;
}

void CPU::BIT(uint8_t value, uint8_t compare, uint8_t cycles) {
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_H, true);
	this->_registers->SetFFlag(FFlag_Z, !(value & compare));
	this->_registers->IncPC(2);
	this->_cycles += cycles;
//	printf("%.2x: %.2x\n", value, this->_registers->GetF());
}

void CPU::JR(bool condition) {
	int8_t value = this->ReadMemory(this->_registers->GetPC() + 1);
	if (condition) {
		this->_registers->IncPC(value + 2);
		this->_cycles += 12;
	} else {
		this->_registers->IncPC(2);
		this->_cycles += 7;
	}
}

// |1/2|4/7| Bitwise XOR on a with b.
void CPU::XOR(uint8_t b, uint8_t cycles, uint8_t size) {
	uint8_t value = this->_registers->GetA() ^ b;
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, false);
	// TODO: P/V is set if overflow; reset otherwise
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, false);
	this->_registers->SetA(value);
	this->_registers->IncPC(size);
	this->_cycles += cycles;
}

// |1|4| Subtracts one from m
void CPU::DECm(uint8_t reg) {
	uint8_t old = this->_registers->GetRegm(reg);
	this->_registers->SetFFlag(FFlag_PV, old == 0x80);
	uint8_t value = old - 1;
	this->_registers->SetRegm(reg, value);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_N, true);
	// TODO: H is set if borrow from bit 4, reset otherwise
	this->_registers->IncPC();
	this->_cycles += 4;
}

// |1|6| Subtracts one from ss
void CPU::DECss(uint8_t reg) {
	this->_registers->SetRegss(reg, this->_registers->GetRegss(reg)  - 1);
	this->_registers->IncPC();
	this->_cycles += 6;
}

// |1|11| Subtracts one from (hl)
void CPU::DECHL() {
	uint8_t old = this->ReadMemory(this->_registers->GetHL());
	this->_registers->SetFFlag(FFlag_PV, old == 0x80);
	uint8_t value = old - 1;
	this->WriteMemory(this->_registers->GetHL(), value);
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_N, true);
	// TODO: H is set if borrow from bit 4, reset otherwise
	this->_registers->IncPC();
	this->_cycles += 11;
}

// |1|4| The contents of reg2 are loaded into reg1
void CPU::LDrr(uint8_t reg1, uint8_t reg2) {
	this->_registers->SetRegm(reg1, this->_registers->GetRegm(reg2));
	this->_registers->IncPC();
	this->_cycles += 4;
}

// |1|11| sp is decremented and reg1 is stored into the memory location pointed to by sp.
// sp is decremented again and reg2 is stored into the memory location pointed to by sp.
void CPU::PUSHqq(uint8_t reg1, uint8_t reg2) {
	uint16_t sp = this->_registers->GetSP();
	this->WriteMemory(sp - 1, this->_registers->GetRegm(reg1));
	this->WriteMemory(sp - 2, this->_registers->GetRegm(reg2));
	this->_registers->SetSP(sp - 2);
	this->_registers->IncPC();
	this->_cycles += 11;
}

// |1|10| The memory location pointed to by sp is stored into reg2 and sp is incremented.
// The memory location pointed to by sp is stored into reg1 and sp is incremented again.
void CPU::POPqq(uint8_t reg1, uint8_t reg2) {
	uint16_t sp = this->_registers->GetSP();
	this->_registers->SetRegm(reg2, this->ReadMemory(sp));
	this->_registers->SetRegm(reg1, this->ReadMemory(sp + 1));
	this->_registers->SetSP(sp + 2);
	this->_registers->IncPC();
	this->_cycles += 10;
}

// |1|4| Bitwise OR on a with valueb
void CPU::OR(uint8_t valueb, uint8_t cycles, uint8_t size) {
	uint8_t value = this->_registers->GetA() | valueb;
	this->_registers->SetFFlag(FFlag_S, value & 0x80);
	this->_registers->SetFFlag(FFlag_Z, value == 0);
	this->_registers->SetFFlag(FFlag_H, false);
	// TODO: P/V is set if overflow; reset otherwise
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_C, false);
	this->_registers->SetA(value);
	this->_registers->IncPC(size);
	this->_cycles += cycles;
}

// |3|10| Loads ** into reg
void CPU::LDddnn(uint8_t reg) {
	uint16_t pc = this->_registers->GetPC();
	this->_registers->SetRegss(reg, (this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1));
	this->_registers->IncPC(3);
	this->_cycles += 10;
}

// |1|6| Adds one to reg
void CPU::INCss(uint8_t reg) {
	this->_registers->SetRegss(reg, this->_registers->GetRegss(reg) + 1);
	this->_registers->IncPC();
	this->_cycles += 6;
}

// |1|4| Adds one to reg
void CPU::INCr(uint8_t reg) {
	this->_registers->SetRegm(reg, this->_registers->GetRegm(reg) + 1);
	this->_registers->IncPC();
	this->_cycles += 4;
}

// |4|20| Stores reg into the memory location pointed to by **
void CPU::LDnndd(uint8_t reg) {
	uint16_t pc = this->_registers->GetPC();
	uint16_t word = this->_registers->GetRegss(reg);
	uint8_t high = word >> 8;
	uint8_t low = word;
	uint16_t offset = (this->ReadMemory(pc + 3) << 8) | this->ReadMemory(pc + 2);
	this->WriteMemory(offset, low);
	this->WriteMemory(offset + 1, high);
	this->_registers->IncPC(4);
	this->_cycles += 20;
}

// |2|7| Loads * into reg
void CPU::LDrn(uint8_t reg) {
	this->_registers->SetRegm(reg, this->ReadMemory(this->_registers->GetPC() + 1));
	this->_registers->IncPC(2);
	this->_cycles += 7;
}

// |1|7| The contents of reg are loaded into (hl).
void CPU::LDHLr(uint8_t reg) {
	this->WriteMemory(this->_registers->GetHL(), this->_registers->GetRegm(reg));
	this->_registers->IncPC();
	this->_cycles += 7;
}

// |1|7| The contents of (hl) are loaded into reg
void CPU::LDrHL(uint8_t reg) {
	this->_registers->SetRegm(reg, this->ReadMemory(this->_registers->GetHL()));
	this->_registers->IncPC();
	this->_cycles += 7;
}

void CPU::ADDHLs(uint8_t reg) {
	uint16_t value = this->_registers->GetHL() + this->_registers->GetRegss(reg);
	this->_registers->SetHL(value);
	this->_registers->SetFFlag(FFlag_N, false);
	// TODO: H is set if carry out of bit 11; reset otherwise
	// TODO: C is set if carry from bit 15; reset otherwise
	this->_registers->IncPC();
	this->_cycles += 11;
}