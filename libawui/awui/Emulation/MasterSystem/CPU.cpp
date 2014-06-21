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

using namespace awui::Emulation::MasterSystem;

CPU::CPU() {
	this->_ports = new Ports();
	this->_ram = new Ram();
	this->_registers = new Registers();
	this->_rom = new Rom(4096);
	this->_cycles = 0;

	this->Reset();
}

CPU::~CPU() {
	delete this->_ports;
	delete this->_ram;
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
	uint8_t opcode1 = this->_rom->ReadByte(this->_registers->GetPC());

	printf("0x%x: 0x%x\n", this->_registers->GetPC(), opcode1);

	// http://clrhome.org/table/
	switch (opcode1) {
		// 00: NOP
		// |1|4| No operation is performed.
		case 0x00:
			this->_registers->IncPC();
			this->_cycles += 4;
			break;

		// 31 nn: LD SP, **
		// |3|10| Loads ** into sp.
		case 0x31:
			{
				uint16_t pc = this->_registers->GetPC();
				this->_registers->SetSP((this->_rom->ReadByte(pc + 2) << 8) | this->_rom->ReadByte(pc + 1));
				this->_registers->IncPC(3);
				this->_cycles += 10;
			}
			break;

		// C3 nn: JP **
		// |3|10| ** is copied to pc.
		case 0xC3:
			{
				uint16_t pc = this->_registers->GetPC();
				this->_registers->SetPC((this->_rom->ReadByte(pc + 2) << 8) | this->_rom->ReadByte(pc + 1));
				this->_cycles += 10;
			}
			break;

		// CD nn: CALL **
		// |3|17| The current pc value plus three is pushed onto the stack, then is loaded with **.
		case 0xCD:
			{
				uint16_t pc = this->_registers->GetPC() + 3;
				uint16_t sp = this->_registers->GetSP() - 2;
				this->_registers->SetSP(sp);
				this->WriteMemory(sp, pc & 0xFF);
				this->WriteMemory(sp + 1, (pc >> 8) & 0xFF);
				this->_cycles += 17;
				this->_registers->SetPC((this->_rom->ReadByte(pc - 1) << 8) | this->_rom->ReadByte(pc - 2));
			}
			break;

		// Extended instructions
		case 0xED:
			{
				uint16_t pc = this->_registers->GetPC();
				opcode1 = this->_rom->ReadByte(pc + 1);

				switch (opcode1) {
					// ED56: IM 1
					// |2|8| Sets interrupt mode 1.
					case 0x56:
						this->_registers->IncPC(2);
						this->_cycles += 8;
						break;
				}
			}
			break;

		// DB n: IN A, *
		// |2|11| A byte from port * is written to a.
		case 0xDB:
			{
				uint16_t pc = this->_registers->GetPC();
				this->_registers->SetA(this->_ports->ReadByte(this->_rom->ReadByte(pc + 1)));
				this->_registers->IncPC(2);
				this->_cycles += 11;
			}
			break;

		// F3 DI
		// |1|4| Resets both interrupt flip-flops, thus prenting maskable interrupts from triggering.
		case 0xF3:
			this->_registers->IncPC();
			this->_registers->SetIFF1(false);
			this->_registers->SetIFF2(false);
			this->_cycles += 4;
			break;

		// FB EI
		// |1|4| Sets both interrupt flip-flops, thus allowing maskable interrupts to occur. An interrupt will not occur until after the immediatedly following instruction.
		case 0xFB:
			this->_registers->IncPC();
			this->_registers->SetIFF1(true);
			this->_registers->SetIFF2(true);
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