/*
 * awui/Emulation/MasterSystem/Motherboard.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Motherboard.h"

#include <assert.h>
#include <awui/Console.h>
#include <awui/Emulation/Common/Rom.h>
#include <awui/Emulation/Common/SavePaths.h>
#include <awui/Emulation/MasterSystem/Ports.h>
#include <awui/Emulation/MasterSystem/Sound.h>
#include <awui/Emulation/MasterSystem/VDP.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <awui/IO/File.h>
#include <algorithm>

using namespace awui;
using namespace awui::Emulation::Common;
using namespace awui::IO;
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

// Placas vivas, para guardar sus partidas al salir del programa: la aplicación no destruye
// los emuladores al cerrar, así que no se puede contar con el destructor
static std::vector<Motherboard *> &GetMotherboards() {
	static std::vector<Motherboard *> *list = new std::vector<Motherboard *>(); // Nunca se destruye: se usa en atexit
	return *list;
}

void Motherboard::FlushAllBoardRam() {
	for (Motherboard *motherboard : GetMotherboards())
		motherboard->FlushBoardRam();
}

Motherboard::Motherboard() {
	static bool atexitRegistered = false;
	if (!atexitRegistered) {
		atexit(Motherboard::FlushAllBoardRam);
		atexitRegistered = true;
	}
	GetMotherboards().push_back(this);

	m_seconds = 0.0;
	m_boardRamIdleFrames = -1;
	m_vdpCycles = 0;
	m_frameDone = false;

	m_z80.SetWriteMemoryCB(MasterGearWriteMemoryCB, this);
	m_z80.SetReadMemoryCB(MasterGearReadMemoryCB, this);
	m_z80.SetWritePortCB(MasterGearWritePortCB, this);
	m_z80.SetReadPortCB(MasterGearReadPortCB, this);

	m_saveData.mapper = MAPPER_SEGA;
	m_rom = new Rom(4096);
	m_sound = new Sound();
	m_sound->SetCPU(this);
	m_startButton = false;

	m_vdp = new VDP(this);
	m_saveData.addressBus.W = 0;
	m_saveData.frameAccumulator = 0;

	m_showLog = false;
	m_showLogInt = false;
	m_showNotImplemented = true;

	m_saveData.wantPause = false;
	m_saveData.pad1 = 0xFF;
	m_saveData.pad2 = 0xFF;
	memset(m_saveData.boardram, 0, 32768 * sizeof(uint8_t));

	Reset();
}

Motherboard::~Motherboard() {
	FlushBoardRam();

	std::vector<Motherboard *> &list = GetMotherboards();
	for (size_t i = 0; i < list.size(); i++) {
		if (list[i] == this) {
			list.erase(list.begin() + i);
			break;
		}
	}

	delete m_sound;
	delete m_rom;
	delete m_vdp;
}

void Motherboard::Reset() {
	m_saveData.controlbyte = 0;
	m_saveData.frame0 = 0;
	m_saveData.frame1 = 1;
	m_saveData.frame2 = 2;
	for (int i = 0; i < 4; i++)
		m_saveData.banks8k[i] = 2 + i;
	m_saveData.codemastersRam = false;
	m_saveData.wantPause = false; // Una pausa pulsada justo antes del reset no debe llegar como NMI
	m_saveData.ports = Ports();	 // El reset vuelve a dejar todas las líneas de los mandos como entradas
	m_z80.Reset();
	m_vdpCycles = m_z80.GetCycles();

	// La RAM del cartucho no se borra: lleva pila y en ella están las partidas guardadas
	memset(m_saveData.ram, 0, 8192 * sizeof(uint8_t));
	m_vdp->Reset();
	m_sound->Reset();
}

// Juegos coreanos sin cabecera que usan mappers propios (comprobados uno a uno)
static uint8_t GetKoreanMapper(uint32_t crc) {
	switch (crc) {
		case 0x89b79e77: // Dallyeora Pigu-Wang (Korea) (Unl)
		case 0x18fb98a3: // Jang Pung 3 (Korea) (Unl)
		case 0x97d03541: // Sangokushi 3 (Korea) (Unl)
			return MAPPER_KOREA;

		case 0x77efe84a: // Cyborg Z (Korea)
		case 0x06965ed9: // F-1 Spirit - The Way to Formula-1 (Korea) (Unl) (Pirate)
		case 0xf89af3cc: // Knightmare II - The Maze of Galious (Korea)
		case 0x445525e2: // Penguin Adventure (Korea) (Unl) (Pirate)
		case 0x83f0eede: // Street Master (Korea) (Unl)
		case 0xa05258f5: // Wonsiin (Korea) (Pirate)
			return MAPPER_MSX;

		case 0xe316c06d: // Nemesis (Korea)
			return MAPPER_MSX_NEMESIS;
	}

	return 0;
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

	// Algunos volcados llevan delante la cabecera de 512 bytes de las copiadoras de cartuchos.
	// Los tamaños reales son múltiplos de 8KB, así que si sobran 512 bytes es esa cabecera
	uint32_t size = m_rom->GetSize();
	if ((size >= 0x2200) && ((size & 0x1FFF) == 0x200)) {
		m_rom->RemoveHeader(0x200);
		printf("Quitada la cabecera de copiadora de 512 bytes\n");
	}

	// Las partidas guardadas van junto a la ROM, con el mismo nombre y extensión .sav
	int dot = file.LastIndexOf(".");
	m_savePath = String::Concat((dot > file.LastIndexOf("/")) ? file.Substring(0, dot) : file, ".sav");
	LoadBoardRam();

	m_vdp->SetGameGear(file.ToLower().EndsWith(".gg") && !IsGameGearInSmsMode(m_rom->GetCRC32()));
	if (file.ToLower().EndsWith(".sg"))
		m_saveData.mapper = MAPPER_SG1000;
	else if (uint8_t korean = GetKoreanMapper(m_rom->GetCRC32()))
		m_saveData.mapper = korean;
	else if (IsCodemastersRom())
		m_saveData.mapper = MAPPER_CODEMASTERS;
	// Los cartuchos de hasta 48KB no llevan mapper: escribir en 0xFFFC-0xFFFF solo toca la RAM
	else if (m_rom->GetSize() <= 0xC000)
		m_saveData.mapper = MAPPER_NONE;
	else
		m_saveData.mapper = MAPPER_SEGA;
}

// Las ROMs de Codemasters llevan en 0x7FE6 una suma de comprobación y en 0x7FE8 su complemento (suman 0x10000)
bool Motherboard::IsCodemastersRom() const {
	if (m_rom->GetSize() < 0x8000)
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

	Processors::Z80::Registers *registers = m_z80.GetRegisters();
	registers->SetIFF1(false);
	registers->SetIFF2(false);

	// El reconocimiento de la IRQ lleva 2 ciclos de espera más que la NMI: 13 en IM 0/1 y 19 en IM 2
	m_z80.IncCycles(2);

	// Nadie pone nada en el bus de datos durante el reconocimiento, así que se lee 0xFF:
	// en IM 0 se ejecuta RST 38h (como en IM 1) y en IM 2 el vector se lee de (I << 8) | 0xFF
	uint16_t vector = 0x0038;
	if (registers->GetIM() == 2) {
		uint16_t address = (registers->GetI() << 8) | 0xFF;
		vector = m_z80.ReadMemory(address);
		vector |= m_z80.ReadMemory(address + 1) << 8;
	}

	m_z80.CallInterrupt(vector);
}

void Motherboard::RunOpcode() {
	m_z80.RunOpcode();
}

// http://www.smspower.org/forums/viewtopic.php?p=69680
// 53693175 / (15 * 228 * 262) ~ 59.922743404 frames per second for NTSC
// 53203424 / (15 * 228 * 313) ~ 49.7014591858 frames per second for PAL

// El emulador avanza en ticks de 1/60 s (el refresco de la pantalla). Los acumuladores solo guardan
// lo pendiente, así no pierden precisión aunque la partida dure horas.
void Motherboard::OnTick(float deltaSeconds) {
	const double tick = 1.0 / 60.0;

	m_seconds += deltaSeconds;

	// Tras un parón (por ejemplo, la ventana congelada) no se intenta recuperar todo el tiempo perdido
	if (m_seconds > 0.25)
		m_seconds = tick;

	while (m_seconds >= tick) {
		m_seconds -= tick;
		DoTick();
	}
}

void Motherboard::DoTick() {
	double fps = m_vdp->GetNTSC() ? 59.922743404 : 49.7014591858;
	// NTSC emula un frame por tick; PAL, 49.70 de cada 59.92 (se salta uno de cada seis ticks, repartidos)
	m_saveData.frameAccumulator += fps / 59.922743404;
	if (m_saveData.frameAccumulator < 1.0)
		return;

	m_saveData.frameAccumulator -= 1.0;
	RunFrame();

	if (m_frameCallback)
		m_frameCallback();
}

void Motherboard::RunFrame() {
	// El frame dura lo que tarda el VDP en llegar al VSYNC. Los ciclos de la última instrucción que se pasan
	// del frame (y los de reconocer las interrupciones) no se pierden: el VDP los recupera en el siguiente
	m_frameDone = false;
	while (!m_frameDone) {
		RunOpcode();

		// NMI del botón de pausa: no se puede enmascarar, entra aunque haya una IRQ en curso.
		// IFF2 conserva el estado de IFF1 para que RETN lo restaure
		if (m_saveData.wantPause) {
			m_z80.GetRegisters()->SetIFF1(false);
			m_z80.CallInterrupt(0x0066);
			m_saveData.wantPause = false;
		}

		SyncVDP();
		CheckInterrupts();
	}

	m_sound->EndFrame(this);

	// La RAM del cartucho se guarda en disco un segundo después de la última escritura:
	// así un juego que guarda muchos bytes seguidos no escribe el fichero en cada frame
	if ((m_boardRamIdleFrames >= 0) && (++m_boardRamIdleFrames >= 60))
		FlushBoardRam();
}

// Guarda ya la RAM del cartucho si hay escrituras pendientes de pasar a disco
void Motherboard::FlushBoardRam() {
	if (m_boardRamIdleFrames < 0)
		return;

	SaveBoardRam();
	m_boardRamIdleFrames = -1;
}

// m_savePath es la ruta junto a la ROM; SavePaths la lleva a la carpeta de partidas (y lee la antigua si solo
// existe esa)
void Motherboard::LoadBoardRam() {
	String path = SavePaths::GetReadPath(m_savePath, SavePaths::Kind::Save);
	std::vector<uint8_t> data;
	if (!File::ReadAllBytes(path, data))
		return;

	size_t size = std::min(data.size(), sizeof(m_saveData.boardram));
	memcpy(m_saveData.boardram, data.data(), size);
	printf("Partida guardada cargada: %s (%zu bytes)\n", path.ToCharArray(), size);
}

void Motherboard::SaveBoardRam() {
	if (m_savePath.GetLength() == 0)
		return;

	// No se crea un .sav vacío para juegos que nunca han guardado nada
	bool empty = true;
	for (size_t i = 0; empty && (i < sizeof(m_saveData.boardram)); i++)
		empty = (m_saveData.boardram[i] == 0);

	if (empty && !File::Exists(SavePaths::GetReadPath(m_savePath, SavePaths::Kind::Save)))
		return;

	// Atómica: no deja el .sav a medias si algo falla
	String path = SavePaths::GetWritePath(m_savePath, SavePaths::Kind::Save);
	if (!File::WriteAllBytes(path, m_saveData.boardram, sizeof(m_saveData.boardram)))
		printf("No se puede guardar la partida en %s\n", path.ToCharArray());
}

// El VDP avanza exactamente 1,5 píxeles por ciclo de CPU (reloj maestro / 10 frente a / 15)
void Motherboard::SyncVDP() {
	int64_t cycles = m_z80.GetCycles();
	int64_t pixels = ((cycles * 3) >> 1) - ((m_vdpCycles * 3) >> 1);
	m_vdpCycles = cycles;

	for (; pixels > 0; pixels--)
		if (m_vdp->OnTick(0))
			m_frameDone = true;
}

uint16_t Motherboard::GetAddressBus() const {
	return m_saveData.addressBus.W;
}

void Motherboard::SetAddressBus(uint16_t data) {
	m_saveData.addressBus.W = data;
}

bool Motherboard::IsEndlessLoop() const {
	return m_z80.IsEndlessLoop();
}

void Motherboard::CallPaused() {
	m_saveData.wantPause = true;
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
	return ((m_saveData.controlbyte & 0x04) ? 0x4000 : 0x0000) + (pos - 0x8000);
}

void Motherboard::WriteMemory(uint16_t pos, uint8_t value) {
	//	if (pos == 0xc092)
	//		printf("Writing: %.2X\n", value);

	switch (m_saveData.mapper) {
		default:
		case MAPPER_SEGA:
		case MAPPER_KOREA:
			if (pos < 0xC000) {
				// Mapper coreano: además de los registros de Sega, 0xA000 elige el banco de 0x8000-0xBFFF
				if ((pos == 0xA000) && (m_saveData.mapper == MAPPER_KOREA)) {
					m_saveData.frame2 = value;
					return;
				}

				if ((pos >= 0x8000) && (m_saveData.controlbyte & 0x08)) {
					m_saveData.boardram[GetBoardRamOffset(pos)] = value;
					MarkBoardRamDirty();
				}
				return;
			}

			// RAM or RAM (mirror)
			if (pos < 0xE000) {
				m_saveData.ram[pos - 0xC000] = value;
				return;
			}

			if (pos >= 0xFFFC) {
				switch (pos) {
					case 0xFFFC:
						m_saveData.controlbyte = value;
						break;
					case 0xFFFD:
						m_saveData.frame0 = value;
						// printf("Frames: %.2X %.2X %.2X\n", d.frame0, d.frame1, d.frame2);
						break;
					case 0xFFFE:
						m_saveData.frame1 = value;
						// printf("Frames: %.2X %.2X %.2X\n", m_saveData.frame0, m_saveData.frame1, m_saveData.frame2);
						break;
					case 0xFFFF:
						m_saveData.frame2 = value;
						// printf("Frames: %.2X %.2X %.2X\n", m_saveData.frame0, m_saveData.frame1, m_saveData.frame2);
						break;
				}
			}

			m_saveData.ram[pos - 0xE000] = value;
			break;

		// Mapper de Codemasters: escribir en 0x0000, 0x4000 o 0x8000 elige el banco de 16KB de esa zona.
		// En 0x4000 el bit 7 activa los 8KB de RAM del cartucho en 0xA000-0xBFFF (Ernie Els Golf)
		case MAPPER_CODEMASTERS:
			if (pos < 0xC000) {
				if ((pos >= 0xA000) && m_saveData.codemastersRam) {
					m_saveData.boardram[pos - 0xA000] = value;
					MarkBoardRamDirty();
					return;
				}

				switch (pos) {
					case 0x0000:
						m_saveData.frame0 = value;
						break;
					case 0x4000:
						m_saveData.frame1 = value & 0x7F;
						m_saveData.codemastersRam = (value & 0x80) != 0;
						break;
					case 0x8000:
						m_saveData.frame2 = value;
						break;
				}
				return;
			}

			if (pos < 0xE000) {
				m_saveData.ram[pos - 0xC000] = value;
				return;
			}

			m_saveData.ram[pos - 0xE000] = value;
			return;

		// SG-1000: ROM en 0x0000-0xBFFF y 1KB de RAM repetido en 0xC000-0xFFFF
		case MAPPER_SG1000:
			if (pos >= 0xC000)
				m_saveData.ram[pos & 0x03FF] = value;
			return;

		// Mapper MSX de 8KB (conversiones de MSX de Zemina): los primeros 16KB son fijos
		// y las escrituras en 0x0000-0x0003 eligen bancos de 8KB para 0x4000-0xBFFF
		case MAPPER_MSX:
		case MAPPER_MSX_NEMESIS:
			if (pos <= 0x0003) {
				// 0x0000 -> 0x8000, 0x0001 -> 0xA000, 0x0002 -> 0x4000, 0x0003 -> 0x6000
				static const int slots[4] = {2, 3, 0, 1};
				m_saveData.banks8k[slots[pos]] = value;
				return;
			}

			if (pos >= 0xC000)
				m_saveData.ram[pos & 0x1FFF] = value;
			return;

		case MAPPER_NONE:
			// En la rom no se escribe
			if (pos < 0xC000)
				return;

			if (pos < 0xE000) {
				m_saveData.ram[pos - 0xC000] = value;
				return;
			}

			m_saveData.ram[pos - 0xE000] = value;
			return;
	}
}

uint8_t Motherboard::ReadMemory(uint16_t pos) const {
	switch (m_saveData.mapper) {
		default:
		case MAPPER_SEGA:
		case MAPPER_KOREA:
			if (pos < 0xC000) {
				if (pos < 0x400)
					return m_rom->ReadByte(pos);

				if (pos < 0x4000)
					return m_rom->ReadByte((uint32_t(m_saveData.frame0) << 14) + pos);

				if (pos < 0x8000)
					return m_rom->ReadByte((uint32_t(m_saveData.frame1) << 14) + (pos - 0x4000));

				if (m_saveData.controlbyte & 0x08) {
					return m_saveData.boardram[GetBoardRamOffset(pos)];
				} else {
					return m_rom->ReadByte((uint32_t(m_saveData.frame2) << 14) + (pos - 0x8000));
				}
			}

			// RAM or RAM (mirror)
			if (pos < 0xE000)
				return m_saveData.ram[pos - 0xC000];

			return m_saveData.ram[pos - 0xE000];

		case MAPPER_CODEMASTERS:
			if (pos < 0x4000)
				return m_rom->ReadByte((uint32_t(m_saveData.frame0) << 14) + pos);

			if (pos < 0x8000)
				return m_rom->ReadByte((uint32_t(m_saveData.frame1) << 14) + (pos - 0x4000));

			if (pos < 0xC000) {
				if ((pos >= 0xA000) && m_saveData.codemastersRam)
					return m_saveData.boardram[pos - 0xA000];

				return m_rom->ReadByte((uint32_t(m_saveData.frame2) << 14) + (pos - 0x8000));
			}

			if (pos < 0xE000)
				return m_saveData.ram[pos - 0xC000];

			return m_saveData.ram[pos - 0xE000];

		case MAPPER_SG1000:
			if (pos < 0xC000)
				return m_rom->ReadByte(pos);

			return m_saveData.ram[pos & 0x03FF];

		case MAPPER_MSX:
		case MAPPER_MSX_NEMESIS:
			// Variante de Nemesis: los primeros 8KB muestran el último banco de 8KB de la ROM
			if ((pos < 0x2000) && (m_saveData.mapper == MAPPER_MSX_NEMESIS))
				return m_rom->ReadByte(m_rom->GetSize() - 0x2000 + pos);

			if (pos < 0x4000)
				return m_rom->ReadByte(pos);

			if (pos < 0xC000)
				return m_rom->ReadByte((uint32_t(m_saveData.banks8k[(pos - 0x4000) >> 13]) << 13) + (pos & 0x1FFF));

			return m_saveData.ram[pos & 0x1FFF];

		case MAPPER_NONE:
			if (pos < 0xC000)
				return m_rom->ReadByte(pos);

			if (pos < 0xE000)
				return m_saveData.ram[pos - 0xC000];

			return m_saveData.ram[pos - 0xE000];
	}
}

void Motherboard::WritePort(uint8_t port, uint8_t value) {
	m_saveData.ports.WriteByte(this, port, value);
}

uint8_t Motherboard::ReadPort(uint8_t port) {
	return m_saveData.ports.ReadByte(this, port);
}

uint32_t Motherboard::GetCRC32() {
	return m_rom->GetCRC32();
}

void Motherboard::SetMapper(uint8_t mapper) {
	m_saveData.mapper = mapper;
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
	m_vdpCycles = m_z80.GetCycles();
	m_sound->LoadState(&data[sizeof(Motherboard::saveData) + VDP::GetSaveSize() + awui::Emulation::Processors::Z80::CPU::GetSaveSize()], m_z80.GetCycles());
}

void Motherboard::SaveState(uint8_t *data) {
	memcpy(data, &m_saveData, sizeof(Motherboard::saveData));

	m_vdp->SaveState(&data[sizeof(Motherboard::saveData)]);
	m_z80.SaveState(&data[sizeof(Motherboard::saveData) + VDP::GetSaveSize()]);
	m_sound->SaveState(&data[sizeof(Motherboard::saveData) + VDP::GetSaveSize() + awui::Emulation::Processors::Z80::CPU::GetSaveSize()]);
}
