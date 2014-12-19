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

using namespace awui;
using namespace awui::Emulation;
using namespace awui::Emulation::Common;
using namespace awui::Emulation::Spectrum;

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

	this->_rom = new Common::Rom(4096);
	this->d._frame = 0;
	this->d._oldFrame = 0;
	this->d._bgColor = 0;

	for (int i = 0; i < 8; i++)
		this->d._keys[i] = 0xFF;

	this->Reset();
}

Motherboard::~Motherboard() {
	delete this->_rom;
	delete this->_z80;
}

void Motherboard::Reset() {
	this->_z80->Reset();

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

#define NTSC 1
void Motherboard::OnTick() {
	this->_initFrame = DateTime::GetTotalSeconds();


	double fps = NTSC ? 59.922743404f : 49.7014591858f;
	double speed = NTSC ? 3.579545f : 3.5468949f;
	this->d._frame += fps / 59.922743404f; // Refresco de awui

	if ((int) this->d._frame == (int) this->d._oldFrame)
		return;

	this->d._oldFrame = this->d._frame;

	double iters = (speed * 1000000.0f) / fps;
	double itersVDP = 256*192;

	double vdpIters = 0;

	int realIters = 0;
	this->_percFrame = 0;
	for (int i = 0; i < iters; i++) {
		int64_t oldCycles = this->_z80->GetCycles();
		this->_z80->RunOpcode();

		double times = (this->_z80->GetCycles() - oldCycles);
		i = i + times - 1;
		this->_percFrame = i / iters;

		vdpIters += times * (itersVDP / iters);
		realIters++;
	}

	this->CheckInterrupts();
}

void Motherboard::WriteMemory(uint16_t pos, uint8_t value) {
	// En la rom no se escribe
	if (pos < 0x4000)
		return;

	if (pos < 0x8000) {
		this->d._ula[pos - 0x4000] = value;
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
		return this->d._ula[pos - 0x4000];

	if (pos < 0xE000)
		return this->d._ram[pos - 0xC000];

	return this->d._ram[pos - 0xE000];
}

void Motherboard::WritePort(uint8_t port, uint8_t value) {
	if (port == 0xFE) {
		this->d._bgColor = value & 0x07;
		return;
	}

	printf("%.2X: %.2X\n", port, value);
}

uint8_t Motherboard::ReadPort(uint8_t port) const {
	if (port == 0xFE) {
		uint8_t row = (this->_z80->GetAddressBus() >> 8);
		for (int i = 0; i < 8; i++)
			if ((row & (1 << i)) == 0)
				return this->d._keys[i];
	}

	return 0xFF;
}

uint32_t Motherboard::GetCRC32() {
	return this->_rom->GetCRC32();
}

int Motherboard::GetSaveSize() {
	int size = sizeof(Motherboard::saveData);
	size += awui::Emulation::Processors::Z80::CPU::GetSaveSize();

	return size;
}

void Motherboard::LoadState(uint8_t * data) {
	memcpy (&this->d, data, sizeof(Motherboard::saveData));
	this->_z80->LoadState(&data[sizeof(Motherboard::saveData)]);
}

void Motherboard::SaveState(uint8_t * data) {
	memcpy (data, &this->d, sizeof(Motherboard::saveData));
	this->_z80->SaveState(&data[sizeof(Motherboard::saveData)]);
}

double Motherboard::GetVirtualTime() {
	double begin = this->_initFrame;
	double frameDuration = 1.0 / 59.922743404;
	return begin + (frameDuration * this->_percFrame);
}

void Motherboard::OnKeyPress(uint8_t row, uint8_t key) {
	this->d._keys[row] &= ~key;
}

void Motherboard::OnKeyUp(uint8_t row, uint8_t key) {
	this->d._keys[row] |= key;
}
