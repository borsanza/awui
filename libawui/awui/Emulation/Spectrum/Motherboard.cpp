/*
 * awui/Emulation/Spectrum/Motherboard.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Motherboard.h"

#include <assert.h>
#include <string.h>
#include <awui/Console.h>
#include <awui/DateTime.h>
#include <awui/Emulation/Common/Rom.h>
#include <awui/Emulation/Processors/Z80/CPU.h>
#include <awui/Emulation/Spectrum/Sound.h>
#include <awui/Emulation/Spectrum/ULA.h>
#include <awui/Convert.h>

using namespace awui;
using namespace awui::Emulation;
using namespace awui::Emulation::Common;
using namespace awui::Emulation::Spectrum;
using namespace awui::Emulation::Processors::Z80;

void WriteMemoryCB(uint16_t pos, uint8_t value, void * data) { ((Motherboard *) data)->WriteMemory(pos, value); }
uint8_t ReadMemoryCB(uint16_t pos, void * data) { return ((Motherboard *) data)->ReadMemory(pos); }
void WritePortCB(uint8_t port, uint8_t value, void * data) { ((Motherboard *) data)->WritePort(port, value); }
uint8_t ReadPortCB(uint8_t port, void * data) { return ((Motherboard *) data)->ReadPort(port); }

Motherboard::Motherboard() {
	this->_z80 = new awui::Emulation::Processors::Z80::CPU();
	this->_z80->SetWriteMemoryCB(WriteMemoryCB, this);
	this->_z80->SetReadMemoryCB(ReadMemoryCB, this);
	this->_z80->SetWritePortCB(WritePortCB, this);
	this->_z80->SetReadPortCB(ReadPortCB, this);

	this->_ula = new ULA();
	this->_sound = new Sound();
	this->_cycles = 0;
	this->_cyclesULA = 0;

	this->_rom = new Common::Rom(4096);

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

	memset(this->d._ram, 0, 8192 * sizeof(uint8_t));
	memset(this->d._boardram, 0, 32768 * sizeof(uint8_t));
}

void Motherboard::LoadRom(const String file) {
	this->_rom->LoadRom(file);
}

void Motherboard::CheckInterrupts() {
	if (!this->_z80->GetRegisters()->GetIFF1())
		return;

//		printf("Entra %d\n", this->_vdp->GetLine());
	this->_z80->SetInInterrupt(true);
	this->_z80->GetRegisters()->SetIFF1(false);
	this->_z80->GetRegisters()->SetIFF2(false);
	this->_z80->CallInterrupt(0x0038);
}

void Motherboard::OnTick() {
	this->_initFrame = DateTime::GetTotalSeconds();
	double speed = 3500000.0f;
	double cyclesFrame = speed / 59.922743404f;

	this->_percFrame = 0;
	do {
		int64_t tmpCycles = this->_z80->GetCycles();
		this->_z80->RunOpcode();
		this->_cycles += this->_z80->GetCycles() - tmpCycles;
		this->_cyclesULA += this->_z80->GetCycles() - tmpCycles;

		this->_percFrame = this->_cycles / cyclesFrame;

		while (this->_cyclesULA > 0) {
			if (this->_ula->OnTick(0)) this->CheckInterrupts();
			if (this->_ula->OnTick(0)) this->CheckInterrupts();
			this->_cyclesULA--;
		}

		if (this->_cycles > cyclesFrame)
			break;
	} while (true);

	this->_cycles -= cyclesFrame;
}

void Motherboard::WriteMemory(uint16_t pos, uint8_t value) {
	// En la rom no se escribe
	if (pos < 0x4000)
		return;

	if (pos < 0x8000) {
		this->_ula->WriteByte(pos - 0x4000, value);
		return;
	}

	if (pos < 0xE000) {
		this->d._ram[pos - 0xC000] = value;
		return;
	}

	this->d._ram[pos - 0xE000] = value;
}

uint8_t Motherboard::ReadMemory(uint16_t pos) const {
	if (pos < 0x4000)
		return this->_rom->ReadByte(pos);

	if (pos < 0x8000)
		return this->_ula->ReadByte(pos - 0x4000);

	if (pos < 0xE000)
		return this->d._ram[pos - 0xC000];

	return this->d._ram[pos - 0xE000];
}

void Motherboard::WritePort(uint8_t port, uint8_t value) {
	if (port == 0xFE) {
		this->_ula->SetBackColor(value & 0x07);
		this->_sound->WriteSound(this, value);

		return;
	}

	printf("WritePort: %.2X: %.2X\n", port, value);
}

uint8_t Motherboard::ReadPort(uint8_t port) const {
	if (port == 0xFE) {
		assert(this->_z80->GetAddressBus().L == 0xFE);
		uint8_t row = this->_z80->GetAddressBus().H;

		uint8_t value = 0xFF;

		for (int i = 0; i <= 7; i++) {
			if ((row & 1) == 0)
				value &= this->d._keys[i];
			row >>= 1;
		}

/*
		if (value != 0xFF) {
			for (int i = 0; i <= 7; i++)
				printf("%d: %X  ", i, this->d._keys[i]);
			printf("port: %X  ", port);
			printf("address: %X  ", this->_z80->GetAddressBus().W);
			printf("Value: %X  ", value);
			printf("\n");
		}
*/

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

void Motherboard::LoadState(uint8_t * data) {
	memcpy (&this->d, data, sizeof(Motherboard::saveData));
	this->_z80->LoadState(&data[sizeof(Motherboard::saveData)]);
	this->_ula->LoadState(&data[sizeof(Motherboard::saveData) + awui::Emulation::Processors::Z80::CPU::GetSaveSize()]);
}

void Motherboard::SaveState(uint8_t * data) {
	memcpy (data, &this->d, sizeof(Motherboard::saveData));
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
