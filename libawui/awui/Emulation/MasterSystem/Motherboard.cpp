/*
 * awui/Emulation/MasterSystem/Motherboard.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Motherboard.h"

#include <awui/Console.h>
#include <awui/DateTime.h>
#include <awui/Emulation/Common/Rom.h>
#include <awui/Emulation/MasterSystem/Ports.h>
#include <awui/Emulation/MasterSystem/Sound.h>
#include <awui/Emulation/MasterSystem/VDP.h>
#include <assert.h>
#include <string.h>

using namespace awui;
using namespace awui::Emulation::Common;
using namespace awui::Emulation::MasterSystem;

void MasterGearWriteMemoryCB(uint16_t pos, uint8_t value, void * data) { ((Motherboard *) data)->WriteMemory(pos, value); }
uint8_t MasterGearReadMemoryCB(uint16_t pos, void * data) { return ((Motherboard *) data)->ReadMemory(pos); }
void MasterGearWritePortCB(uint8_t port, uint8_t value, void * data) { ((Motherboard *) data)->WritePort(port, value); }
uint8_t MasterGearReadPortCB(uint8_t port, void * data) { return ((Motherboard *) data)->ReadPort(port); }

Motherboard::Motherboard() {
	this->_z80.SetWriteMemoryCB(MasterGearWriteMemoryCB, this);
	this->_z80.SetReadMemoryCB(MasterGearReadMemoryCB, this);
	this->_z80.SetWritePortCB(MasterGearWritePortCB, this);
	this->_z80.SetReadPortCB(MasterGearReadPortCB, this);

	this->d._mapper = MAPPER_SEGA;
	this->_rom = new Rom(4096);
	this->_sound = new Sound();
	this->_sound->SetCPU(this);

	this->_percFrame = 0;
	this->_initFrame = 0;
	this->_vdp = new VDP(this);
	this->d._addressBus.W = 0;
	this->d._frame = 0;
	this->d._oldFrame = 0;

	this->_showLog = false;
	this->_showLogInt = false;
	this->_showNotImplemented = true;

	this->d._wantPause = false;
	this->d._pad1 = 0xFF;
	this->d._pad2 = 0xFF;

	this->Reset();
}

Motherboard::~Motherboard() {
	delete this->_sound;
	delete this->_rom;
	delete this->_vdp;
}

void Motherboard::Reset() {
	this->d._controlbyte = 0;
	this->d._frame0 = 0;
	this->d._frame1 = 1;
	this->d._frame2 = 2;
	this->_z80.Reset();

	memset(this->d._ram, 0, 8192 * sizeof(uint8_t));
	memset(this->d._boardram, 0, 32768 * sizeof(uint8_t));
	this->_vdp->Reset();
}

void Motherboard::LoadRom(const String file) {
	this->_rom->LoadRom(file);
}

void Motherboard::CheckInterrupts() {
	bool interrupt = this->_vdp->GetInterrupt();
	if (!this->_z80.GetRegisters()->GetIFF1())
		return;

	if (interrupt) {
//		printf("Entra %d\n", this->_vdp->GetLine());
		this->_z80.SetInInterrupt(true);
		this->_z80.GetRegisters()->SetIFF1(false);
		this->_z80.GetRegisters()->SetIFF2(false);
		this->_z80.CallInterrupt(0x0038);
	}
}

void Motherboard::RunOpcode() {
	this->_z80.RunOpcode();
}

// http://www.smspower.org/forums/viewtopic.php?p=69680
// 53693175 / (15 * 228 * 262) ~ 59.922743404 frames per second for NTSC
// 53203424 / (15 * 228 * 313) ~ 49.7014591858 frames per second for PAL
void Motherboard::OnTick() {
	this->_initFrame = DateTime::GetTotalSeconds();

	double fps = this->_vdp->GetNTSC() ? 59.922743404f : 49.7014591858f;
	double speed = this->_vdp->GetNTSC() ? 3.579545f : 3.5468949f;
	this->d._frame += fps / 59.922743404f; // Refresco de awui

	if ((int) this->d._frame == (int) this->d._oldFrame)
		return;

	this->d._oldFrame = this->d._frame;

	double iters = (speed * 1000000.0f) / fps;
	double itersVDP = this->_vdp->GetTotalWidth() * this->_vdp->GetTotalHeight();

	bool vsync = false;
	int vdpCount = 0;
	double vdpIters = 0;

	int realIters = 0;
	this->_percFrame = 0;

	for (int i = 0; i < iters; i++) {
		int64_t oldCycles = this->_z80.GetCycles();
		this->RunOpcode();

		if (this->d._wantPause & !this->_z80.IsInInterrupt()) {
			this->_z80.GetRegisters()->SetIFF1(false);
			this->_z80.CallInterrupt(0x0066);
			this->d._wantPause = false;
		}

		double times = (this->_z80.GetCycles() - oldCycles);
		i = i + times - 1;
		this->_percFrame = i / iters;

		vdpIters += times * (itersVDP / iters);
		if (!vsync) {
			for (; vdpCount < vdpIters; vdpCount++) {
				if (vsync) continue;
				vsync = this->_vdp->OnTick(realIters);
				if (vsync)
					this->CheckInterrupts();
			}
		}
		realIters++;
	}

	this->_sound->EndFrame(DateTime::GetTotalSeconds());

	while (!vsync) {
		vsync = this->_vdp->OnTick(realIters);
		if (vsync)
			this->CheckInterrupts();
	}
}

uint16_t Motherboard::GetAddressBus() const {
	return this->d._addressBus.W;
}

void Motherboard::SetAddressBus(uint16_t data) {
	this->d._addressBus.W = data;
}

bool Motherboard::IsEndlessLoop() const {
	return this->_z80.IsEndlessLoop();
}

void Motherboard::CallPaused() {
	this->d._wantPause = true;
}

void Motherboard::WriteMemory(uint16_t pos, uint8_t value) {
//	if (pos == 0xc092)
//		printf("Writing: %.2X\n", value);

	switch (this->d._mapper) {
		default:
		case MAPPER_SEGA:
			if (pos < 0xC000) {
				if ((pos >= 0x8000) && (this->d._controlbyte & 0x08)) {
					uint16_t offset = ((this->d._controlbyte & 0x04)? 0x4000 : 0x0000) + pos;
					if (offset >= 32768) {
						printf("Motherboard::WriteMemory Out of Range");
						return;
					}

					this->d._boardram[offset] = value;
				}
				return;
			}

			// RAM or RAM (mirror)
			if (pos < 0xE000) {
				this->d._ram[pos - 0xC000] = value;
				return;
			}

			if (pos >= 0xFFFC) {
				switch (pos) {
					case 0xFFFC:
						this->d._controlbyte = value;
						break;
					case 0xFFFD:
						value = value % this->_rom->GetNumPages();
						this->d._frame0 = value;
//						printf("Frames: %.2X %.2X %.2X\n", this->d._frame0, this->d._frame1, this->d._frame2);
						break;
					case 0xFFFE:
						value = value % this->_rom->GetNumPages();
						this->d._frame1 = value;
//						printf("Frames: %.2X %.2X %.2X\n", this->d._frame0, this->d._frame1, this->d._frame2);
						break;
					case 0xFFFF:
						value = value % this->_rom->GetNumPages();
						this->d._frame2 = value;
//						printf("Frames: %.2X %.2X %.2X\n", this->d._frame0, this->d._frame1, this->d._frame2);
						break;
				}
			}

			this->d._ram[pos - 0xE000] = value;
			break;

		case MAPPER_NONE:
			// En la rom no se escribe
			if (pos < 0xC000)
				return;

			if (pos < 0xE000) {
				this->d._ram[pos - 0xC000] = value;
				return;
			}

			this->d._ram[pos - 0xE000] = value;
			return;
	}
}

uint8_t Motherboard::ReadMemory(uint16_t pos) const {
	switch (this->d._mapper) {
		default:
		case MAPPER_SEGA:
			if (pos < 0xC000) {
				if (pos < 0x400)
					return this->_rom->ReadByte(pos);

				if (pos < 0x4000)
					return this->_rom->ReadByte((uint16_t(this->d._frame0) << 14) + pos);

				if (pos < 0x8000)
					return this->_rom->ReadByte((uint16_t(this->d._frame1) << 14) + (pos - 0x4000));


				if (this->d._controlbyte & 0x08) {
					uint16_t offset = ((this->d._controlbyte & 0x04)? 0x4000 : 0x0000) + pos;
					if (offset >= 32768) {
						printf("Motherboard::ReadMemory Out of Range");
						return 0;
					}

					return this->d._boardram[offset];
				} else {
					return this->_rom->ReadByte((uint16_t(this->d._frame2) << 14) + (pos - 0x8000));
				}
			}

			// RAM or RAM (mirror)
			if (pos < 0xE000)
				return this->d._ram[pos - 0xC000];

			return this->d._ram[pos - 0xE000];

		case MAPPER_NONE:
			if (pos < 0xC000)
				return this->_rom->ReadByte(pos);

			if (pos < 0xE000)
				return this->d._ram[pos - 0xC000];

			return this->d._ram[pos - 0xE000];
	}
}

void Motherboard::WritePort(uint8_t port, uint8_t value) {
	this->d._ports.WriteByte(this, port, value);
}

uint8_t Motherboard::ReadPort(uint8_t port) {
	return this->d._ports.ReadByte(this, port);
}

uint32_t Motherboard::GetCRC32() {
	return this->_rom->GetCRC32();
}

void Motherboard::SetMapper(uint8_t mapper) {
	this->d._mapper = mapper;
}

int Motherboard::GetSaveSize() {
	int size = sizeof(Motherboard::saveData);
	size += VDP::GetSaveSize();
	size += awui::Emulation::Processors::Z80::CPU::GetSaveSize();

	return size;
}

void Motherboard::LoadState(uint8_t * data) {
	memcpy (&this->d, data, sizeof(Motherboard::saveData));

	this->_vdp->LoadState(&data[sizeof(Motherboard::saveData)]);
	this->_z80.LoadState(&data[sizeof(Motherboard::saveData) + VDP::GetSaveSize()]);
}

void Motherboard::SaveState(uint8_t * data) {
	memcpy (data, &this->d, sizeof(Motherboard::saveData));

	this->_vdp->SaveState(&data[sizeof(Motherboard::saveData)]);
	this->_z80.SaveState(&data[sizeof(Motherboard::saveData) + VDP::GetSaveSize()]);
}

double Motherboard::GetVirtualTime() {
	double begin = this->_initFrame;
	double frameDuration = 1.0 / 59.922743404;
	return begin + (frameDuration * this->_percFrame);
}
