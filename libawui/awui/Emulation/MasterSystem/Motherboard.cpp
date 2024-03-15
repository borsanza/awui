/*
 * awui/Emulation/MasterSystem/Motherboard.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Motherboard.h"

#include <assert.h>
#include <awui/Console.h>
#include <awui/DateTime.h>
#include <awui/Emulation/Common/Rom.h>
#include <awui/Emulation/MasterSystem/Ports.h>
#include <awui/Emulation/MasterSystem/Sound.h>
#include <awui/Emulation/MasterSystem/VDP.h>
#include <string.h>

using namespace awui;
using namespace awui::Emulation::Common;
using namespace awui::Emulation::MasterSystem;

void MasterGearWriteMemoryCB(uint16_t pos, uint8_t value, void *data) {
	((Motherboard *) data)->WriteMemory(pos, value);
}
uint8_t MasterGearReadMemoryCB(uint16_t pos, void *data) {
	return ((Motherboard *) data)->ReadMemory(pos);
}
void MasterGearWritePortCB(uint8_t port, uint8_t value, void *data) {
	((Motherboard *) data)->WritePort(port, value);
}
uint8_t MasterGearReadPortCB(uint8_t port, void *data) {
	return ((Motherboard *) data)->ReadPort(port);
}

Motherboard::Motherboard() {
	m_seconds = 0.0f;
	m_nextTick = 0.0f;

	m_z80.SetWriteMemoryCB(MasterGearWriteMemoryCB, this);
	m_z80.SetReadMemoryCB(MasterGearReadMemoryCB, this);
	m_z80.SetWritePortCB(MasterGearWritePortCB, this);
	m_z80.SetReadPortCB(MasterGearReadPortCB, this);

	m_saveData._mapper = MAPPER_SEGA;
	m_rom = new Rom(4096);
	m_sound = new Sound();
	m_sound->SetCPU(this);

	m_percFrame = 0;
	m_initFrame = 0;
	m_vdp = new VDP(this);
	m_saveData._addressBus.W = 0;
	m_saveData._frame = 0;
	m_saveData._oldFrame = 0;

	m_showLog = false;
	m_showLogInt = false;
	m_showNotImplemented = true;

	m_saveData._wantPause = false;
	m_saveData._pad1 = 0xFF;
	m_saveData._pad2 = 0xFF;

	Reset();
}

Motherboard::~Motherboard() {
	delete m_sound;
	delete m_rom;
	delete m_vdp;
}

void Motherboard::Reset() {
	m_saveData._controlbyte = 0;
	m_saveData._frame0 = 0;
	m_saveData._frame1 = 1;
	m_saveData._frame2 = 2;
	m_z80.Reset();

	memset(m_saveData._ram, 0, 8192 * sizeof(uint8_t));
	memset(m_saveData._boardram, 0, 32768 * sizeof(uint8_t));
	m_vdp->Reset();
}

void Motherboard::LoadRom(const String file) {
	m_rom->LoadRom(file);
}

void Motherboard::CheckInterrupts() {
	bool interrupt = m_vdp->GetInterrupt();
	if (!m_z80.GetRegisters()->GetIFF1())
		return;

	if (interrupt) {
		//		printf("Entra %d\n", m_vdp->GetLine());
		m_z80.SetInInterrupt(true);
		m_z80.GetRegisters()->SetIFF1(false);
		m_z80.GetRegisters()->SetIFF2(false);
		m_z80.CallInterrupt(0x0038);
	}
}

void Motherboard::RunOpcode() {
	m_z80.RunOpcode();
}

// http://www.smspower.org/forums/viewtopic.php?p=69680
// 53693175 / (15 * 228 * 262) ~ 59.922743404 frames per second for NTSC
// 53203424 / (15 * 228 * 313) ~ 49.7014591858 frames per second for PAL

void Motherboard::OnTick(float deltaSeconds) {
	m_seconds += deltaSeconds;
	if (m_seconds < m_nextTick) {
		return;
	}

	while (m_seconds >= m_nextTick) {
		m_nextTick += 1.0f / 60.0f;
		DoTick();
	}
}

void Motherboard::DoTick() {
	m_initFrame = DateTime::GetTotalSeconds();

	double fps = m_vdp->GetNTSC() ? 59.922743404f : 49.7014591858f;
	double speed = m_vdp->GetNTSC() ? 3.579545f : 3.5468949f;
	m_saveData._frame += fps / 59.922743404f; // Refresco de awui

	if ((int) m_saveData._frame == (int) m_saveData._oldFrame)
		return;

	m_saveData._oldFrame = m_saveData._frame;

	double iters = (speed * 1000000.0f) / fps;
	double itersVDP = m_vdp->GetTotalWidth() * m_vdp->GetTotalHeight();

	bool vsync = false;
	int vdpCount = 0;
	double vdpIters = 0;

	int realIters = 0;
	m_percFrame = 0;

	for (int i = 0; i < iters; i++) {
		int64_t oldCycles = m_z80.GetCycles();
		RunOpcode();

		if (m_saveData._wantPause & !m_z80.IsInInterrupt()) {
			m_z80.GetRegisters()->SetIFF1(false);
			m_z80.CallInterrupt(0x0066);
			m_saveData._wantPause = false;
		}

		double times = (m_z80.GetCycles() - oldCycles);
		i = i + times - 1;
		m_percFrame = i / iters;

		vdpIters += times * (itersVDP / iters);
		if (!vsync) {
			for (; vdpCount < vdpIters; vdpCount++) {
				if (vsync)
					continue;
				vsync = m_vdp->OnTick(realIters);
				if (vsync)
					CheckInterrupts();
			}
		}
		realIters++;
	}

	while (!vsync) {
		vsync = m_vdp->OnTick(realIters);
		if (vsync)
			CheckInterrupts();
	}
}

uint16_t Motherboard::GetAddressBus() const {
	return m_saveData._addressBus.W;
}

void Motherboard::SetAddressBus(uint16_t data) {
	m_saveData._addressBus.W = data;
}

bool Motherboard::IsEndlessLoop() const {
	return m_z80.IsEndlessLoop();
}

void Motherboard::CallPaused() {
	m_saveData._wantPause = true;
}

void Motherboard::WriteMemory(uint16_t pos, uint8_t value) {
	//	if (pos == 0xc092)
	//		printf("Writing: %.2X\n", value);

	switch (m_saveData._mapper) {
		default:
		case MAPPER_SEGA:
			if (pos < 0xC000) {
				if ((pos >= 0x8000) && (m_saveData._controlbyte & 0x08)) {
					uint16_t offset = ((m_saveData._controlbyte & 0x04) ? 0x4000 : 0x0000) + pos;
					if (offset >= 32768) {
						printf("Motherboard::WriteMemory Out of Range");
						return;
					}

					m_saveData._boardram[offset] = value;
				}
				return;
			}

			// RAM or RAM (mirror)
			if (pos < 0xE000) {
				m_saveData._ram[pos - 0xC000] = value;
				return;
			}

			if (pos >= 0xFFFC) {
				switch (pos) {
					case 0xFFFC:
						m_saveData._controlbyte = value;
						break;
					case 0xFFFD:
						value = value % m_rom->GetNumPages();
						m_saveData._frame0 = value;
						// printf("Frames: %.2X %.2X %.2X\n", d._frame0, d._frame1, d._frame2);
						break;
					case 0xFFFE:
						value = value % m_rom->GetNumPages();
						m_saveData._frame1 = value;
						// printf("Frames: %.2X %.2X %.2X\n", m_saveData._frame0, m_saveData._frame1, m_saveData._frame2);
						break;
					case 0xFFFF:
						value = value % m_rom->GetNumPages();
						m_saveData._frame2 = value;
						// printf("Frames: %.2X %.2X %.2X\n", m_saveData._frame0, m_saveData._frame1, m_saveData._frame2);
						break;
				}
			}

			m_saveData._ram[pos - 0xE000] = value;
			break;

		case MAPPER_NONE:
			// En la rom no se escribe
			if (pos < 0xC000)
				return;

			if (pos < 0xE000) {
				m_saveData._ram[pos - 0xC000] = value;
				return;
			}

			m_saveData._ram[pos - 0xE000] = value;
			return;
	}
}

uint8_t Motherboard::ReadMemory(uint16_t pos) const {
	switch (m_saveData._mapper) {
		default:
		case MAPPER_SEGA:
			if (pos < 0xC000) {
				if (pos < 0x400)
					return m_rom->ReadByte(pos);

				if (pos < 0x4000)
					return m_rom->ReadByte((uint16_t(m_saveData._frame0) << 14) + pos);

				if (pos < 0x8000)
					return m_rom->ReadByte((uint16_t(m_saveData._frame1) << 14) + (pos - 0x4000));

				if (m_saveData._controlbyte & 0x08) {
					uint16_t offset = ((m_saveData._controlbyte & 0x04) ? 0x4000 : 0x0000) + pos;
					if (offset >= 32768) {
						printf("Motherboard::ReadMemory Out of Range");
						return 0;
					}

					return m_saveData._boardram[offset];
				} else {
					return m_rom->ReadByte((uint16_t(m_saveData._frame2) << 14) + (pos - 0x8000));
				}
			}

			// RAM or RAM (mirror)
			if (pos < 0xE000)
				return m_saveData._ram[pos - 0xC000];

			return m_saveData._ram[pos - 0xE000];

		case MAPPER_NONE:
			if (pos < 0xC000)
				return m_rom->ReadByte(pos);

			if (pos < 0xE000)
				return m_saveData._ram[pos - 0xC000];

			return m_saveData._ram[pos - 0xE000];
	}
}

void Motherboard::WritePort(uint8_t port, uint8_t value) {
	m_saveData._ports.WriteByte(this, port, value);
}

uint8_t Motherboard::ReadPort(uint8_t port) {
	return m_saveData._ports.ReadByte(this, port);
}

uint32_t Motherboard::GetCRC32() {
	return m_rom->GetCRC32();
}

void Motherboard::SetMapper(uint8_t mapper) {
	m_saveData._mapper = mapper;
}

int Motherboard::GetSaveSize() {
	int size = sizeof(Motherboard::saveData);
	size += VDP::GetSaveSize();
	size += awui::Emulation::Processors::Z80::CPU::GetSaveSize();

	return size;
}

void Motherboard::LoadState(uint8_t *data) {
	memcpy(&m_saveData, data, sizeof(Motherboard::saveData));

	m_vdp->LoadState(&data[sizeof(Motherboard::saveData)]);
	m_z80.LoadState(&data[sizeof(Motherboard::saveData) + VDP::GetSaveSize()]);
}

void Motherboard::SaveState(uint8_t *data) {
	memcpy(data, &m_saveData, sizeof(Motherboard::saveData));

	m_vdp->SaveState(&data[sizeof(Motherboard::saveData)]);
	m_z80.SaveState(&data[sizeof(Motherboard::saveData) + VDP::GetSaveSize()]);
}

double Motherboard::GetVirtualTime() {
	double begin = m_initFrame;
	double frameDuration = 1.0 / 59.922743404;
	return begin + (frameDuration * m_percFrame);
}
