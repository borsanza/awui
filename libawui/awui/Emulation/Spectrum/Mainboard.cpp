/*
 * awui/Emulation/Spectrum/Mainboard.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Mainboard.h"

#include <assert.h>
#include <string.h>
#include <awui/Console.h>
#include <awui/DateTime.h>
#include <awui/Emulation/Spectrum/Rom.h>
#include <awui/Emulation/Processors/Z80/CPU.h>

using namespace awui;
using namespace awui::Emulation;
using namespace awui::Emulation::Spectrum;

void WriteMemoryCB(uint16_t pos, uint8_t value, void * data) { ((Mainboard *) data)->WriteMemory(pos, value); }
uint8_t ReadMemoryCB(uint16_t pos, void * data) { return ((Mainboard *) data)->ReadMemory(pos); }
void WritePortCB(uint8_t port, uint8_t value, void * data) { ((Mainboard *) data)->WritePort(port, value); }
uint8_t ReadPortCB(uint8_t port, void * data) { return ((Mainboard *) data)->ReadPort(port); }

Mainboard::Mainboard() {
	this->_z80 = new awui::Emulation::Processors::Z80::CPU();
	this->_z80->SetWriteMemoryCB(WriteMemoryCB, this);
	this->_z80->SetReadMemoryCB(ReadMemoryCB, this);
	this->_z80->SetWritePortCB(WritePortCB, this);
	this->_z80->SetReadPortCB(ReadPortCB, this);

	this->_rom = new Rom(4096);
	this->d._frame = 0;
	this->d._oldFrame = 0;

	this->Reset();
}

Mainboard::~Mainboard() {
	delete this->_rom;
	delete this->_z80;
}

void Mainboard::Reset() {
}

void Mainboard::LoadRom(const String file) {
	this->_rom->LoadRom(file);
}

#define NTSC 1
void Mainboard::OnTick() {
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
}

void Mainboard::WriteMemory(uint16_t pos, uint8_t value) {
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

uint8_t Mainboard::ReadMemory(uint16_t pos) const {
	if (pos < 0x4000)
		return this->_rom->ReadByte(pos);

	if (pos < 0x8000)
		return this->d._ula[pos - 0x4000];

	if (pos < 0xE000)
		return this->d._ram[pos - 0xC000];

	return this->d._ram[pos - 0xE000];
}

void Mainboard::WritePort(uint8_t pos, uint8_t value) {
}

uint8_t Mainboard::ReadPort(uint8_t pos) const {
	return 0xFF;
}

uint32_t Mainboard::GetCRC32() {
	return this->_rom->GetCRC32();
}

int Mainboard::GetSaveSize() {
	int size = sizeof(Mainboard::saveData);

	return size;
}

void Mainboard::LoadState(uint8_t * data) {
	memcpy (&this->d, data, sizeof(Mainboard::saveData));
}

void Mainboard::SaveState(uint8_t * data) {
	memcpy (data, &this->d, sizeof(Mainboard::saveData));
}

double Mainboard::GetVirtualTime() {
	double begin = this->_initFrame;
	double frameDuration = 1.0 / 59.922743404;
	return begin + (frameDuration * this->_percFrame);
}
