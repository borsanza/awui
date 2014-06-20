/*
 * awui/Emulation/MasterSystem/CPU.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "CPU.h"

#include <awui/Console.h>
#include <awui/Emulation/MasterSystem/Ram.h>
#include <awui/Emulation/MasterSystem/Registers.h>
#include <awui/Emulation/MasterSystem/Rom.h>

using namespace awui::Emulation::MasterSystem;

CPU::CPU() {
	this->_registers = new Registers();
	this->_ram = new Ram();
	this->_rom = new Rom(4096);
	this->_pc = 0;
	this->_cycles = 0;

	this->Reset();
}

CPU::~CPU() {
	delete this->_registers;
	delete this->_rom;
}

void CPU::Reset() {
	this->_registers->Clear();
}

void CPU::LoadRom(const String file) {
	this->_rom->LoadRom(file);
}

void CPU::OnTick() {
	this->RunOpcode();
}

void CPU::RunOpcode() {
	uint8_t opcode1 = this->_rom->ReadByte(this->_pc);

	printf("0x%x: 0x%x\n", this->_pc, opcode1);

	switch (opcode1) {
		// 31 n n: LD SP,nn		10	3	1
		// 31: LD   SP,&0000  -              SLS  C      sls (iy+0)->c    -
		case 0x31:
			this->_registers->SetSP((this->_rom->ReadByte(this->_pc + 2) << 8) | this->_rom->ReadByte(this->_pc + 1));
			printf("%x\n", this->_registers->GetSP());
			this->_pc += 3;
			this->_cycles += 10;
			break;

		// C3 n n:		JP (nn)			10	3	1
		case 0xC3:
			this->_pc = (this->_rom->ReadByte(this->_pc + 2) << 8) | this->_rom->ReadByte(this->_pc + 1);
			this->_cycles += 10;
			break;

		// CD n n:	CALL (nn)		17	5	1
		case 0xCD:
			{
				this->_pc += 3;
				uint16_t sp = this->_registers->GetSP() - 2;
				this->_registers->SetSP(sp);
				this->WriteMemory(sp, this->_pc & 0xFF);
				this->WriteMemory(sp + 1, (this->_pc >> 8) & 0xFF);
				this->_cycles += 17;
				this->_pc = (this->_rom->ReadByte(this->_pc - 1) << 8) | this->_rom->ReadByte(this->_pc - 2);
			}
			break;

		// ED **** ED ****   -              SET  5,L    set 5,(iy+0)->l  -
		case 0xED:
			this->_pc++;
			opcode1 = this->_rom->ReadByte(this->_pc);

			switch (opcode1) {
				// ED56: IM 1
				case 0x56:
					this->_pc++;
					this->_cycles += 8;
					break;
			}
			break;

		// DB n: IN A,(n)		11	3	1
		case 0xDB:
			this->_pc += 2;
			this->_cycles += 11;
			break;

		// F3 DI             -              SET  6,E    set 6,(iy+0)->e  -
		case 0xF3:
			this->_pc++;
			this->_cycles += 4;
			break;
	}
}

void CPU::WriteMemory(uint16_t pos, uint8_t value) {
	// RAM
	if (pos >= 0xC000) {
		// RAM or RAM (mirror)
		if (pos <= 0xDFFF)
			this->_rom->WriteByte(pos - 0xC000, value);
		else
			this->_rom->WriteByte(pos - 0xE000, value);
	}
}

uint8_t CPU::ReadMemory(uint16_t pos) {
	// RAM
	if (pos >= 0xC000) {
		// RAM or RAM (mirror)
		if (pos <= 0xDFFF)
			return this->_rom->ReadByte(pos - 0xC000);
		else
			return this->_rom->ReadByte(pos - 0xE000);
	}

	return 0;
}