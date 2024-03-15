/**
 * awui/Emulation/Spectrum/Motherboard.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Motherboard.h"

#include <assert.h>
#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/DateTime.h>
#include <awui/Emulation/Common/Rom.h>
#include <awui/Emulation/Common/Word.h>
#include <awui/Emulation/Processors/Z80/CPU.h>
#include <awui/Emulation/Spectrum/Sound.h>
#include <awui/Emulation/Spectrum/ULA.h>
#include <string.h>

using namespace awui;
using namespace awui::Emulation;
using namespace awui::Emulation::Common;
using namespace awui::Emulation::Spectrum;
using namespace awui::Emulation::Processors::Z80;

/*
Content Memory:
	14335	6 (until 14.341) <- 14335 % 8 = 7;
	14336	5 (until 14.341) <- 14336 % 8 = 0;
	14337	4 (until 14.341) <- 14337 % 8 = 1;
	14338	3 (until 14.341) <- 14338 % 8 = 2;
	14339	2 (until 14.341) <- 14339 % 8 = 3;
	14340	1 (until 14.341) <- 14340 % 8 = 4;
	14341	No delay         <- 14341 % 8 = 5;
	14342	No delay         <- 14342 % 8 = 6;
*/

static int content_states[] = {5, 4, 3, 2, 1, 0, 0, 6};

void WriteMemoryCB(uint16_t pos, uint8_t value, void *data) {
	((Motherboard *) data)->WriteMemory(pos, value);
}
uint8_t ReadMemoryCB(uint16_t pos, void *data) {
	return ((Motherboard *) data)->ReadMemory(pos);
}
void WritePortCB(uint8_t port, uint8_t value, void *data) {
	((Motherboard *) data)->WritePort(port, value);
}
uint8_t ReadPortCB(uint8_t port, void *data) {
	return ((Motherboard *) data)->ReadPort(port);
}

Motherboard::Motherboard() {
	this->_percFrame = 0;
	this->_initFrame = 0;
	this->_countReadCycles = 0;
	this->_lastCycles = 0;
	this->_writeCassetteDataCB = NULL;
	this->_readCassetteDataCB = NULL;

	this->_z80 = new awui::Emulation::Processors::Z80::CPU();
	this->_z80->SetWriteMemoryCB(WriteMemoryCB, this);
	this->_z80->SetReadMemoryCB(ReadMemoryCB, this);
	this->_z80->SetWritePortCB(WritePortCB, this);
	this->_z80->SetReadPortCB(ReadPortCB, this);

	this->_writeCassetteCB = 0;
	this->_readCassetteCB = 0;
	this->_lastWriteState = 0;
	this->_lastWriteCycle = 0;
	this->_lastReadCycle = 0;
	this->_lastReadState = 0;

	this->_ula = new ULA();
	this->_sound = new Sound();
	this->_cycles = 0;
	this->_cyclesULA = 0;
	this->_fast = false;

	this->_rom = new Common::Rom(16384);

	for (int i = 0; i < 8; i++)
		this->d._keys[i] = 0xFF;

	this->Reset();
}

Motherboard::~Motherboard() {
	delete this->_rom;
	delete this->_z80;
	delete this->_ula;
}

void Motherboard::Reset() {
	this->_z80->Reset();
	this->_ula->Reset();

	memset(this->d._ram, 0, 32768 * sizeof(uint8_t));
}

void Motherboard::LoadRom(const String file) {
	this->_rom->LoadRom(file);
}

void Motherboard::CheckInterrupts() {
	if (!this->_z80->GetRegisters()->GetIFF1())
		return;

	Word newPC;
	switch (this->_z80->GetRegisters()->GetIM()) {
		case 0:
			printf("Motherboard::CheckInterrupts: Mode 0 IM: No testeado\n");
			newPC.W = 0x0038;
			break;
		case 1:
			newPC.W = 0x0038;
			break;
		case 2: {
			Word address;
			address.H = this->_z80->GetRegisters()->GetI();
			address.L = 0xFF;
			newPC.L = this->_z80->ReadMemory(address.W);
			address.W++;
			newPC.H = this->_z80->ReadMemory(address.W);
			break;
		}
		default:
			newPC.W = 0x0038;
			printf("Motherboard::CheckInterrupts: Mode %d IM: Desconocido\n", this->_z80->GetRegisters()->GetIM());
			break;
	}

	this->_z80->SetInInterrupt(true);
	this->_z80->GetRegisters()->SetIFF1(false);
	this->_z80->GetRegisters()->SetIFF2(false);
	this->_z80->CallInterrupt(newPC.W);
}

void Motherboard::ProcessCassette() {
	int data = -2;
	if ((this->_countReadCycles <= 0) && this->_readCassetteCB)
		data = this->_readCassetteCB(this->_readCassetteDataCB);

	// Para testear si funciona
	// if (this->_countReadCycles <= 0)
	//	data = 2168;

	switch (data) {
		case -2: // Hay que dejarlo para que no ejecute el default
			break;
		case -1: // Lee del cassette pero no hay datos
			this->_countReadCycles = 0;
			break;
		default:
			this->_countReadCycles += data;
			this->_lastReadState = !this->_lastReadState;
			if (!this->_fast)
				this->_sound->WriteSound(this, this->_lastReadState ? 0x08 : 0x10);
			break;
	}

	if (this->_countReadCycles > 0) {
		int16_t diff = (int16_t) (this->_lastCycles - this->_lastReadCycle);
		this->_countReadCycles -= diff;
	}

	this->_lastReadCycle = this->_lastCycles;
}

void Motherboard::OnTick() {
	this->_initFrame = DateTime::GetTotalSeconds();
	double speed = 3500000.0f;
	if (this->_fast)
		speed *= 9;

	double cyclesFrame = speed / 59.922743404f;

	this->_percFrame = 0;
	do {
		this->_lastCycles = this->_z80->GetCycles();
		this->_z80->RunOpcode();
		this->_cycles += this->_z80->GetCycles() - this->_lastCycles;
		this->_cyclesULA += this->_z80->GetCycles() - this->_lastCycles;

		this->_percFrame = this->_cycles / cyclesFrame;

		this->ProcessCassette();

		while (this->_cyclesULA > 0) {
			if (this->_ula->OnTick(0))
				this->CheckInterrupts();
			if (this->_ula->OnTick(0))
				this->CheckInterrupts();
			this->_cyclesULA--;
		}

		if (this->_cycles > cyclesFrame)
			break;
	} while (true);

	this->_cycles -= cyclesFrame;
}

/**
 * 0000 - 3FFF: ROM Memory
 * 4000 - 7FFF: ULA Memory
 * 8000 - FFFF: RAM Memory
 */
void Motherboard::WriteMemory(uint16_t offset, uint8_t data) {
	switch (offset >> 14) {
		// 0000 - 3FFF
		case 0:
			// En la rom no se escribe
			break;

		// 4000 - 7FFF
		case 1:
			this->_z80->IncCycles(content_states[this->_z80->GetCycles() % 8]);
			this->_ula->WriteByte(offset & 0x3FFF, data);
			break;

		// 8000 - FFFF
		case 2:
		case 3:
			this->d._ram[offset & 0x7FFF] = data;
			break;
	}
}

uint8_t Motherboard::ReadMemory(uint16_t offset) const {
	uint8_t data;

	switch (offset >> 14) {
		// 0000 - 3FFF
		case 0:
			data = this->_rom->ReadByte(offset);
			break;

		// 4000 - 7FFF
		case 1:
			this->_z80->IncCycles(content_states[this->_z80->GetCycles() % 8]);
			data = this->_ula->ReadByte(offset & 0x3FFF);
			break;

		// 8000 - FFFF
		case 2:
		case 3:
			data = this->d._ram[offset & 0x7FFF];
			break;
	}

	return data;
}

void Motherboard::WritePort(uint8_t port, uint8_t value) {
	if (port == 0xFE) {
		this->_ula->SetBackColor(value & 0x07);
		if (!this->_fast && this->_countReadCycles == 0)
			this->_sound->WriteSound(this, value);

		if (((value >> 3) & 0x01) != this->_lastWriteState) {
			this->_lastWriteState = ((value >> 3) & 0x01);
			int32_t diff = (int32_t) this->_lastCycles - this->_lastWriteCycle;
			if (this->_writeCassetteCB)
				this->_writeCassetteCB(diff, this->_writeCassetteDataCB);
			this->_lastWriteCycle = this->_lastCycles;
		}

		return;
	}

	printf("WritePort: %.2X: %.2X\n", port, value);
}

uint8_t Motherboard::ReadPort(uint8_t port) const {
	if ((port & 0x20) == 0)
		return this->d._kempston;

	// Entra en todas las direcciones pares
	if ((port & 0x01) == 0) {
		assert(this->_z80->GetAddressBus().L == port);
		uint8_t row = this->_z80->GetAddressBus().H;

		uint8_t value = 0xFF;
		/*
				static uint8_t debug = 0;
				if (debug != row) {
					debug = row;
					printf("ReadPort: %.2X -> %.2X\n", port, row);
				}
		*/
		for (int i = 0; i <= 7; i++) {
			if ((row & 0x01) == 0)
				value &= this->d._keys[i];
			row >>= 1;
		}

		if (this->_lastReadState)
			value |= 0x40;
		else
			value &= 0xBF;

		return value;
	}

	printf("ReadPort: %.2X\n", port);
	return 0xFF;
}

uint32_t Motherboard::GetCRC32() {
	return this->_rom->GetCRC32();
}

int Motherboard::GetSaveSize() {
	int size = sizeof(Motherboard::saveData);
	size += awui::Emulation::Processors::Z80::CPU::GetSaveSize();
	size += awui::Emulation::Spectrum::ULA::GetSaveSize();

	return size;
}

void Motherboard::LoadState(uint8_t *data) {
	memcpy(&this->d, data, sizeof(Motherboard::saveData));
	this->_z80->LoadState(&data[sizeof(Motherboard::saveData)]);
	this->_ula->LoadState(&data[sizeof(Motherboard::saveData) + awui::Emulation::Processors::Z80::CPU::GetSaveSize()]);
}

void Motherboard::SaveState(uint8_t *data) {
	memcpy(data, &this->d, sizeof(Motherboard::saveData));
	this->_z80->SaveState(&data[sizeof(Motherboard::saveData)]);
	this->_ula->SaveState(&data[sizeof(Motherboard::saveData) + awui::Emulation::Processors::Z80::CPU::GetSaveSize()]);
}

double Motherboard::GetVirtualTime() {
	double begin = this->_initFrame;
	double frameDuration = 1.0 / 59.922743404;
	return begin + (frameDuration * this->_percFrame);
}

void Motherboard::OnKeyPress(uint8_t row, uint8_t key) {
	this->d._keys[row] &= ~key;
	// printf("Press %d: %x\n", row, this->d._keys[row]);
}

void Motherboard::OnKeyUp(uint8_t row, uint8_t key) {
	this->d._keys[row] |= key;
	// printf("Up %d: %x\n", row, this->d._keys[row]);
}

void Motherboard::OnPadEvent(uint8_t status) {
	this->d._kempston = status;
}

void Motherboard::SetWriteCassetteCB(void (*fun)(int32_t, void *), void *data) {
	this->_writeCassetteCB = fun;
	this->_writeCassetteDataCB = data;
}

void Motherboard::SetReadCassetteCB(int32_t (*fun)(void *), void *data) {
	this->_readCassetteCB = fun;
	this->_readCassetteDataCB = data;
}
