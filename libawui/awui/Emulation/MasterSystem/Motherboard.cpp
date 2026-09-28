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
	m_startButton = false;

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
	m_saveData._codemastersRam = false;
	m_z80.Reset();

	memset(m_saveData._ram, 0, 8192 * sizeof(uint8_t));
	memset(m_saveData._boardram, 0, 32768 * sizeof(uint8_t));
	m_vdp->Reset();
	m_sound->Reset();
}

// Juegos de Game Gear que funcionan en el modo de compatibilidad con Master System
// (paleta de 6 bits y pantalla completa). La cabecera no lo indica, así que van por CRC.
static bool IsGameGearInSmsMode(uint32_t crc) {
	switch (crc) {
		case 0x59840fd6: // Castle of Illusion Starring Mickey Mouse (USA, Europe)
		case 0x44fbe8f6: // Chase H.Q. (USA)
		case 0x8813514b: // Excellent Dizzy Collection, The (Europe)
		case 0xc888222b: // Fantastic Dizzy (USA)
		case 0xa2f9c7af: // Olympic Gold (USA)
		case 0x10dbbef4: // Super Kick Off (Europe, Japan)
			return true;
	}

	return false;
}

void Motherboard::LoadRom(const String file) {
	m_rom->LoadRom(file);
	m_vdp->SetGameGear(file.ToLower().EndsWith(".gg") && !IsGameGearInSmsMode(m_rom->GetCRC32()));
	if (file.ToLower().EndsWith(".sg"))
		m_saveData._mapper = MAPPER_SG1000;
	else
		m_saveData._mapper = IsCodemastersRom() ? MAPPER_CODEMASTERS : MAPPER_SEGA;
}

// Las ROMs de Codemasters llevan en 0x7FE6 una suma de comprobación y en 0x7FE8 su complemento (suman 0x10000)
bool Motherboard::IsCodemastersRom() const {
	if (m_rom->GetNumPages() < 2)
		return false;

	uint16_t checksum = m_rom->ReadByte(0x7FE6) | (m_rom->ReadByte(0x7FE7) << 8);
	uint16_t complement = m_rom->ReadByte(0x7FE8) | (m_rom->ReadByte(0x7FE9) << 8);

	return (checksum != 0) && (uint16_t(checksum + complement) == 0);
}

// La IRQ del VDP es por nivel: sigue activa hasta que el juego lee el registro de estado.
// Se comprueba después de cada instrucción y no se pierde si en ese momento las interrupciones están deshabilitadas.
void Motherboard::CheckInterrupts() {
	if (!m_vdp->IsIRQ())
		return;

	if (!m_z80.GetRegisters()->GetIFF1() || m_z80.IsAfterEI())
		return;

	m_z80.SetInInterrupt(true);
	m_z80.GetRegisters()->SetIFF1(false);
	m_z80.GetRegisters()->SetIFF2(false);
	m_z80.CallInterrupt(0x0038);
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

		vdpIters += times * (itersVDP / iters);
		if (!vsync) {
			for (; vdpCount < vdpIters; vdpCount++) {
				if (vsync)
					continue;
				vsync = m_vdp->OnTick(realIters);
			}
		}

		CheckInterrupts();
		realIters++;
	}

	while (!vsync)
		vsync = m_vdp->OnTick(realIters);

	m_sound->EndFrame(this);
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

void Motherboard::SetPauseButton(bool pressed) {
	if (IsGameGear()) {
		m_startButton = pressed;
		return;
	}

	if (pressed)
		CallPaused();
}

bool Motherboard::IsGameGear() const {
	return m_vdp->IsGameGear();
}

// RAM del cartucho (0xFFFC bit 3): se mapea en 0x8000-0xBFFF y el bit 2 elige cuál de los dos bancos de 16KB
uint16_t Motherboard::GetBoardRamOffset(uint16_t pos) const {
	return ((m_saveData._controlbyte & 0x04) ? 0x4000 : 0x0000) + (pos - 0x8000);
}

void Motherboard::WriteMemory(uint16_t pos, uint8_t value) {
	//	if (pos == 0xc092)
	//		printf("Writing: %.2X\n", value);

	switch (m_saveData._mapper) {
		default:
		case MAPPER_SEGA:
			if (pos < 0xC000) {
				if ((pos >= 0x8000) && (m_saveData._controlbyte & 0x08))
					m_saveData._boardram[GetBoardRamOffset(pos)] = value;
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

		// Mapper de Codemasters: escribir en 0x0000, 0x4000 o 0x8000 elige el banco de 16KB de esa zona.
		// En 0x4000 el bit 7 activa los 8KB de RAM del cartucho en 0xA000-0xBFFF (Ernie Els Golf)
		case MAPPER_CODEMASTERS:
			if (pos < 0xC000) {
				if ((pos >= 0xA000) && m_saveData._codemastersRam) {
					m_saveData._boardram[pos - 0xA000] = value;
					return;
				}

				switch (pos) {
					case 0x0000:
						m_saveData._frame0 = value % m_rom->GetNumPages();
						break;
					case 0x4000:
						m_saveData._frame1 = (value & 0x7F) % m_rom->GetNumPages();
						m_saveData._codemastersRam = (value & 0x80) != 0;
						break;
					case 0x8000:
						m_saveData._frame2 = value % m_rom->GetNumPages();
						break;
				}
				return;
			}

			if (pos < 0xE000) {
				m_saveData._ram[pos - 0xC000] = value;
				return;
			}

			m_saveData._ram[pos - 0xE000] = value;
			return;

		// SG-1000: ROM en 0x0000-0xBFFF y 1KB de RAM repetido en 0xC000-0xFFFF
		case MAPPER_SG1000:
			if (pos >= 0xC000)
				m_saveData._ram[pos & 0x03FF] = value;
			return;

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
					return m_saveData._boardram[GetBoardRamOffset(pos)];
				} else {
					return m_rom->ReadByte((uint16_t(m_saveData._frame2) << 14) + (pos - 0x8000));
				}
			}

			// RAM or RAM (mirror)
			if (pos < 0xE000)
				return m_saveData._ram[pos - 0xC000];

			return m_saveData._ram[pos - 0xE000];

		case MAPPER_CODEMASTERS:
			if (pos < 0x4000)
				return m_rom->ReadByte((uint32_t(m_saveData._frame0) << 14) + pos);

			if (pos < 0x8000)
				return m_rom->ReadByte((uint32_t(m_saveData._frame1) << 14) + (pos - 0x4000));

			if (pos < 0xC000) {
				if ((pos >= 0xA000) && m_saveData._codemastersRam)
					return m_saveData._boardram[pos - 0xA000];

				return m_rom->ReadByte((uint32_t(m_saveData._frame2) << 14) + (pos - 0x8000));
			}

			if (pos < 0xE000)
				return m_saveData._ram[pos - 0xC000];

			return m_saveData._ram[pos - 0xE000];

		case MAPPER_SG1000:
			if (pos < 0xC000)
				return m_rom->ReadByte(pos);

			return m_saveData._ram[pos & 0x03FF];

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
	size += Sound::GetSaveSize();

	return size;
}

void Motherboard::LoadState(uint8_t *data) {
	memcpy(&m_saveData, data, sizeof(Motherboard::saveData));

	m_vdp->LoadState(&data[sizeof(Motherboard::saveData)]);
	m_z80.LoadState(&data[sizeof(Motherboard::saveData) + VDP::GetSaveSize()]);
	m_sound->LoadState(&data[sizeof(Motherboard::saveData) + VDP::GetSaveSize() + awui::Emulation::Processors::Z80::CPU::GetSaveSize()], m_z80.GetCycles());
}

void Motherboard::SaveState(uint8_t *data) {
	memcpy(data, &m_saveData, sizeof(Motherboard::saveData));

	m_vdp->SaveState(&data[sizeof(Motherboard::saveData)]);
	m_z80.SaveState(&data[sizeof(Motherboard::saveData) + VDP::GetSaveSize()]);
	m_sound->SaveState(&data[sizeof(Motherboard::saveData) + VDP::GetSaveSize() + awui::Emulation::Processors::Z80::CPU::GetSaveSize()]);
}
