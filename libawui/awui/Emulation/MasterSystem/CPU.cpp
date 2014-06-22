/*
 * awui/Emulation/MasterSystem/CPU.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "CPU.h"

#include <awui/Console.h>
#include <awui/Emulation/MasterSystem/Ports.h>
#include <awui/Emulation/MasterSystem/Ram.h>
#include <awui/Emulation/MasterSystem/Registers.h>
#include <awui/Emulation/MasterSystem/Rom.h>
#include <awui/Emulation/MasterSystem/VDP.h>

using namespace awui::Emulation::MasterSystem;

CPU::CPU() {
	this->_ram = new Ram();
	this->_registers = new Registers();
	this->_rom = new Rom(4096);
	this->_vdp = new VDP();
	this->_ports = new Ports(this->_vdp);
	this->_cycles = 0;

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
	static float frame = 0;
	static float oldFrame = 0;
	frame += FPS / GUIFPS;

	if ((int) frame == (int) oldFrame) {
		return;
	}

	oldFrame = frame;

	float iters = (SPEED * 1000000.0f) / FPS; // 71400
	float itersVDP = 256 * 192;               // 49152

	bool vsync = false;
	int vdpCount = 0;
	float vdpIters = 0;
	for (int i = 0; i < iters; i++) {
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

	while (!vsync)
		vsync = this->_vdp->OnTick();
}

void CPU::RunOpcode() {
	static bool showLog = false;

	uint8_t opcode1 = this->ReadMemory(this->_registers->GetPC());
	this->_opcode.SetByte1(opcode1);

	if (showLog) printf("%.4X: %.2X", this->_registers->GetPC(), opcode1);
	if ((opcode1 == 0xCB) || (opcode1 == 0xDD) || (opcode1 == 0xED) || (opcode1 == 0xFD)) {
		uint8_t opcode2 = this->ReadMemory(this->_registers->GetPC() + 1);
		this->_opcode.SetByte2(opcode2);
		if (showLog) printf(" %.2X", opcode2);
		if (((opcode1 == 0xDD) || (opcode1 == 0xFD)) && (opcode2 == 0xCB)) {
			uint8_t opcode4 = this->ReadMemory(this->_registers->GetPC() + 3);
			this->_opcode.SetByte4(opcode4);
			if (showLog) printf(" %.2X", opcode4);
		}
	}

	if (showLog) printf("\n");

	// http://clrhome.org/table/
	switch (this->_opcode.GetEnum()) {
		// 00: NOP
		// |1|4| No operation is performed.
		case Ox00:
			this->_registers->IncPC();
			this->_cycles += 4;
			break;

		// 01: LD BC, **
		// |3|10| Loads ** into bc.
		case Ox01:
			{
				uint16_t pc = this->_registers->GetPC();
				this->_registers->SetBC((this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1));
				this->_registers->IncPC(3);
				this->_cycles += 10;
			}
			break;

		// 02: LD (BC), A
		// |1|7| Stores a into the memory location pointed to by bc.
		case Ox02:
			this->WriteMemory(this->_registers->GetBC(), this->_registers->GetA());
			this->_registers->IncPC();
			this->_cycles += 7;
			break;

		// 03: INC BC
		// |1|6| Adds one to bc.
		case Ox03:
			this->_registers->SetBC(this->_registers->GetBC() + 1);
			this->_registers->IncPC();
			this->_cycles += 6;
			break;

		// 04: INC B
		// |1|4| Adds one to b.
		case Ox04:
			this->_registers->SetB(this->_registers->GetB() + 1);
			this->_registers->IncPC();
			this->_cycles += 4;
			break;

		// 05: DEC B
		// |1|4| Subtracts one from b.
		case Ox05:
			this->_registers->SetB(this->_registers->GetB() - 1);
			this->_registers->IncPC();
			this->_cycles += 4;
			break;

		// 06: LD B, *
		// |2|7| Loads * into b.
		case Ox06:
			this->_registers->SetB(this->ReadMemory(this->_registers->GetPC() + 1));
			this->_registers->IncPC(2);
			this->_cycles += 7;
			break;

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

		// 28 nn: JR X, *
		// |2|12/7| If condition X is true, the signed value * is added to pc. The jump is measured from the start of the instruction opcode.
		case Ox18: this->JR(true); break;
		case Ox20: this->JR(!(this->_registers->GetF() & FFlag_Z)); break;
		case Ox28: this->JR(  this->_registers->GetF() & FFlag_Z);  break;
		case Ox30: this->JR(!(this->_registers->GetF() & FFlag_C)); break;
		case Ox38: this->JR(  this->_registers->GetF() & FFlag_C);  break;

		// 11 nn: LD DE, **
		// |3|10| Loads ** into de.
		case Ox11:
			{
				uint16_t pc = this->_registers->GetPC();
				this->_registers->SetDE((this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1));
				this->_registers->IncPC(3);
				this->_cycles += 10;
			}
			break;


		// 21 nn: LD HL, **
		// |3|10| Loads ** into hl.
		case Ox21:
			{
				uint16_t pc = this->_registers->GetPC();
				this->_registers->SetHL((this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1));
				this->_registers->IncPC(3);
				this->_cycles += 10;
			}
			break;

		// 31 nn: LD SP, **
		// |3|10| Loads ** into sp.
		case Ox31:
			{
				uint16_t pc = this->_registers->GetPC();
				this->_registers->SetSP((this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1));
				this->_registers->IncPC(3);
				this->_cycles += 10;
			}
			break;

		// C3 nn: JP **
		// |3|10| ** is copied to pc.
		case OxC3:
			{
				uint16_t pc = this->_registers->GetPC();
				this->_registers->SetPC((this->ReadMemory(pc + 2) << 8) | this->ReadMemory(pc + 1));
				this->_cycles += 10;
			}
			break;

		// CB40-CB7F: BIT X, Y
		// |2|8| Tests bit X of Y.
		case OxCB40: BIT(this->_registers->GetB(), 0x01); break;
		case OxCB41: BIT(this->_registers->GetC(), 0x01); break;
		case OxCB42: BIT(this->_registers->GetD(), 0x01); break;
		case OxCB43: BIT(this->_registers->GetE(), 0x01); break;
		case OxCB44: BIT(this->_registers->GetH(), 0x01); break;
		case OxCB45: BIT(this->_registers->GetL(), 0x01); break;
		case OxCB46: BIT(this->ReadMemory(this->_registers->GetHL()), 0x01); break;
		case OxCB47: BIT(this->_registers->GetA(), 0x01); break;
		case OxCB48: BIT(this->_registers->GetB(), 0x02); break;
		case OxCB49: BIT(this->_registers->GetC(), 0x02); break;
		case OxCB4A: BIT(this->_registers->GetD(), 0x02); break;
		case OxCB4B: BIT(this->_registers->GetE(), 0x02); break;
		case OxCB4C: BIT(this->_registers->GetH(), 0x02); break;
		case OxCB4D: BIT(this->_registers->GetL(), 0x02); break;
		case OxCB4E: BIT(this->ReadMemory(this->_registers->GetHL()), 0x02); break;
		case OxCB4F: BIT(this->_registers->GetA(), 0x02); break;
		case OxCB50: BIT(this->_registers->GetB(), 0x04); break;
		case OxCB51: BIT(this->_registers->GetC(), 0x04); break;
		case OxCB52: BIT(this->_registers->GetD(), 0x04); break;
		case OxCB53: BIT(this->_registers->GetE(), 0x04); break;
		case OxCB54: BIT(this->_registers->GetH(), 0x04); break;
		case OxCB55: BIT(this->_registers->GetL(), 0x04); break;
		case OxCB56: BIT(this->ReadMemory(this->_registers->GetHL()), 0x04); break;
		case OxCB57: BIT(this->_registers->GetA(), 0x04); break;
		case OxCB58: BIT(this->_registers->GetB(), 0x08); break;
		case OxCB59: BIT(this->_registers->GetC(), 0x08); break;
		case OxCB5A: BIT(this->_registers->GetD(), 0x08); break;
		case OxCB5B: BIT(this->_registers->GetE(), 0x08); break;
		case OxCB5C: BIT(this->_registers->GetH(), 0x08); break;
		case OxCB5D: BIT(this->_registers->GetL(), 0x08); break;
		case OxCB5E: BIT(this->ReadMemory(this->_registers->GetHL()), 0x08); break;
		case OxCB5F: BIT(this->_registers->GetA(), 0x08); break;
		case OxCB60: BIT(this->_registers->GetB(), 0x10); break;
		case OxCB61: BIT(this->_registers->GetC(), 0x10); break;
		case OxCB62: BIT(this->_registers->GetD(), 0x10); break;
		case OxCB63: BIT(this->_registers->GetE(), 0x10); break;
		case OxCB64: BIT(this->_registers->GetH(), 0x10); break;
		case OxCB65: BIT(this->_registers->GetL(), 0x10); break;
		case OxCB66: BIT(this->ReadMemory(this->_registers->GetHL()), 0x10); break;
		case OxCB67: BIT(this->_registers->GetA(), 0x10); break;
		case OxCB68: BIT(this->_registers->GetB(), 0x20); break;
		case OxCB69: BIT(this->_registers->GetC(), 0x20); break;
		case OxCB6A: BIT(this->_registers->GetD(), 0x20); break;
		case OxCB6B: BIT(this->_registers->GetE(), 0x20); break;
		case OxCB6C: BIT(this->_registers->GetH(), 0x20); break;
		case OxCB6D: BIT(this->_registers->GetL(), 0x20); break;
		case OxCB6E: BIT(this->ReadMemory(this->_registers->GetHL()), 0x20); break;
		case OxCB6F: BIT(this->_registers->GetA(), 0x20); break;
		case OxCB70: BIT(this->_registers->GetB(), 0x40); break;
		case OxCB71: BIT(this->_registers->GetC(), 0x40); break;
		case OxCB72: BIT(this->_registers->GetD(), 0x40); break;
		case OxCB73: BIT(this->_registers->GetE(), 0x40); break;
		case OxCB74: BIT(this->_registers->GetH(), 0x40); break;
		case OxCB75: BIT(this->_registers->GetL(), 0x40); break;
		case OxCB76: BIT(this->ReadMemory(this->_registers->GetHL()), 0x40); break;
		case OxCB77: BIT(this->_registers->GetA(), 0x40); break;
		case OxCB78: BIT(this->_registers->GetB(), 0x80); break;
		case OxCB79: BIT(this->_registers->GetC(), 0x80); break;
		case OxCB7A: BIT(this->_registers->GetD(), 0x80); break;
		case OxCB7B: BIT(this->_registers->GetE(), 0x80); break;
		case OxCB7C: BIT(this->_registers->GetH(), 0x80); break;
		case OxCB7D: BIT(this->_registers->GetL(), 0x80); break;
		case OxCB7E: BIT(this->ReadMemory(this->_registers->GetHL()), 0x80); break;
		case OxCB7F: BIT(this->_registers->GetA(), 0x80); break;

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

		default:
			// showLog = true;
			this->_cycles += 4;
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

void CPU::BIT(uint8_t value, uint8_t compare) {
	this->_registers->SetFFlag(FFlag_N, false);
	this->_registers->SetFFlag(FFlag_H, true);
	this->_registers->SetFFlag(FFlag_Z, !(value & compare));
	this->_registers->IncPC(2);
	this->_cycles += 8;
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