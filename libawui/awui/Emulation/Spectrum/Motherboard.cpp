/**
 * awui/Emulation/Spectrum/Motherboard.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Motherboard.h"

#include <assert.h>
#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/Emulation/Common/Rom.h>
#include <awui/Emulation/Common/Word.h>
#include <awui/Emulation/Processors/Z80/CPU.h>
#include <awui/Emulation/Spectrum/Sound.h>
#include <awui/Emulation/Spectrum/TapeCorder.h>
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

// Detección del cargador: lecturas del puerto 0xFE a menos de estos ciclos de la anterior cuentan como bucle de
// carga; con estas por frame se considera que hay un cargador; y sin él durante estos frames (2 s) se para la cinta
#define LOADER_READ_GAP 200
#define LOADER_READS_PER_FRAME 300
#define TAPE_IDLE_FRAMES 120

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
	m_percFrame = 0;
	m_countReadCycles = 0;
	m_lastCycles = 0;
	m_writeCassetteDataCB = NULL;
	m_readCassetteDataCB = NULL;

	m_z80 = new awui::Emulation::Processors::Z80::CPU();
	m_z80->SetWriteMemoryCB(WriteMemoryCB, this);
	m_z80->SetReadMemoryCB(ReadMemoryCB, this);
	m_z80->SetWritePortCB(WritePortCB, this);
	m_z80->SetReadPortCB(ReadPortCB, this);

	m_writeCassetteCB = 0;
	m_readCassetteCB = 0;
	m_lastWriteState = 0;
	m_lastWriteCycle = 0;
	m_lastReadCycle = 0;
	m_lastReadState = 0;

	m_ula = new ULA();
	m_sound = new Sound();
	m_cycles = 0;
	m_cyclesULA = 0;
	m_fast = false;
	m_tape = nullptr;
	m_lastEarReadCycle = 0;
	m_loaderReads = 0;
	m_framesWithoutLoader = 0;
	m_tapeWasPlaying = false;

	m_rom = new Common::Rom(16384);

	for (int i = 0; i < 8; i++)
		m_saveData.keys[i] = 0xFF;

	Reset();
}

Motherboard::~Motherboard() {
	delete m_rom;
	delete m_z80;
	delete m_ula;
}

void Motherboard::Reset() {
	m_z80->Reset();
	m_ula->Reset();

	memset(m_saveData.ram, 0, 32768 * sizeof(uint8_t));
}

void Motherboard::LoadRom(const String file) {
	m_rom->LoadRom(file);
}

void Motherboard::CheckInterrupts() {
	if (!m_z80->GetRegisters()->GetIFF1())
		return;

	Word newPC;
	switch (m_z80->GetRegisters()->GetIM()) {
		case 0:
			printf("Motherboard::CheckInterrupts: Mode 0 IM: No testeado\n");
			newPC.W = 0x0038;
			break;
		case 1:
			newPC.W = 0x0038;
			break;
		case 2: {
			Word address;
			address.H = m_z80->GetRegisters()->GetI();
			address.L = 0xFF;
			newPC.L = m_z80->ReadMemory(address.W);
			address.W++;
			newPC.H = m_z80->ReadMemory(address.W);
			break;
		}
		default:
			newPC.W = 0x0038;
			printf("Motherboard::CheckInterrupts: Mode %d IM: Desconocido\n", m_z80->GetRegisters()->GetIM());
			break;
	}

	m_z80->SetInInterrupt(true);
	m_z80->GetRegisters()->SetIFF1(false);
	m_z80->GetRegisters()->SetIFF2(false);
	m_z80->CallInterrupt(newPC.W);
}

void Motherboard::ProcessCassette() {
	int data = -2;
	if ((m_countReadCycles <= 0) && m_readCassetteCB)
		data = m_readCassetteCB(m_readCassetteDataCB);

	// Para testear si funciona
	// if (m_countReadCycles <= 0)
	//	data = 2168;

	switch (data) {
		case -2: // Hay que dejarlo para que no ejecute el default
			break;
		case -1: // Lee del cassette pero no hay datos
			m_countReadCycles = 0;
			break;
		default:
			m_countReadCycles += data;
			m_lastReadState = !m_lastReadState;
			if (!m_fast)
				m_sound->WriteSound(this, m_lastReadState ? 0x08 : 0x10);
			break;
	}

	if (m_countReadCycles > 0) {
		int16_t diff = (int16_t) (m_lastCycles - m_lastReadCycle);
		m_countReadCycles -= diff;
	}

	m_lastReadCycle = m_lastCycles;
}

/**
 * Carga instantánea de un bloque con la rutina LD-BYTES de la ROM, como hacen otros emuladores.
 * Se engancha en LD-START (0x056C), por donde la ROM pasa una y otra vez mientras espera el tono guía: así
 * funciona también si se activa el modo rápido con la carga ya empezada. En ese punto:
 *   A' = byte de bandera esperado, acarreo de F' = cargar (sin acarreo: verificar), IX = destino, DE = longitud,
 *   y en la pila está SA/LD-RET (0x053F), que restaura el borde, hace EI y vuelve a quien llamó a LD-BYTES.
 * Sale con acarreo si ha ido bien (con otra bandera, como la ROM, se consume el bloque y sale sin acarreo).
 * Si no está la ROM estándar o no quedan bloques, no hace nada y la ROM sigue a velocidad normal (un
 * cargador propio también sigue funcionando así).
 */
bool Motherboard::FlashLoad() {
	// INC D; EX AF,AF'; DEC D: principio de LD-BYTES en la ROM del 48K
	if (!m_tape || (ReadMemory(0x0556) != 0x14) || (ReadMemory(0x0557) != 0x08) || (ReadMemory(0x0558) != 0x15))
		return false;

	TapeBlock *block = m_tape->TakeNextBlock();
	if (!block)
		return false;

	Processors::Z80::Registers *regs = m_z80->GetRegisters();
	regs->AlternateAF();
	uint8_t flag = regs->GetA();
	bool load = (regs->GetF() & Processors::Z80::Flag_C) != 0;
	regs->AlternateAF();
	uint16_t ix = regs->GetIX();
	uint16_t de = regs->GetDE();
	int length = block->GetLength();

	bool ok = (length > 0) && (block->GetByte(0) == flag);
	if (ok) {
		uint8_t parity = block->GetByte(0);
		int pos = 1;
		while ((de > 0) && (pos < length)) {
			uint8_t value = block->GetByte(pos++);
			parity ^= value;
			if (load)
				WriteMemory(ix, value);
			else if (ReadMemory(ix) != value)
				ok = false;
			ix++;
			de--;
		}

		// Byte de comprobación
		if (pos < length)
			parity ^= block->GetByte(pos);

		ok = ok && (de == 0) && (pos < length) && (parity == 0);
	}

	regs->SetIX(ix);
	regs->SetDE(de);
	regs->SetF(ok ? (regs->GetF() | Processors::Z80::Flag_C) : (regs->GetF() & ~Processors::Z80::Flag_C));

	// RET (a SA/LD-RET)
	uint16_t sp = regs->GetSP();
	regs->SetPC(ReadMemory(sp) | (ReadMemory(sp + 1) << 8));
	regs->SetSP(sp + 2);

	return true;
}

void Motherboard::OnTick() {
	double speed = 3500000.0f;
	if (m_fast)
		speed *= 9;

	double cyclesFrame = speed / 59.922743404f;

	m_percFrame = 0;
	do {
		// Modo rápido: la rutina de carga de la ROM se sustituye por copiar el bloque de la cinta
		if (m_fast && (m_z80->GetPC() == 0x056C) && FlashLoad())
			continue;

		m_lastCycles = m_z80->GetCycles();
		m_z80->RunOpcode();
		m_cycles += m_z80->GetCycles() - m_lastCycles;
		m_cyclesULA += m_z80->GetCycles() - m_lastCycles;

		m_percFrame = m_cycles / cyclesFrame;

		ProcessCassette();

		while (m_cyclesULA > 0) {
			if (m_ula->OnTick(0))
				CheckInterrupts();
			if (m_ula->OnTick(0))
				CheckInterrupts();
			m_cyclesULA--;
		}

		if (m_cycles > cyclesFrame)
			break;
	} while (true);

	m_cycles -= cyclesFrame;

	// En modo rápido el sonido no tiene sentido (va 9 veces más deprisa): se encola silencio
	m_sound->EndFrame(FrameSeconds, m_fast);

	UpdateTapeMotor();
}

// Como el motor de las cintas con control remoto: si un cargador está leyendo, la cinta suena; si durante un
// rato nadie la lee (el juego ya ha cargado, o se ha pulsado F9 sin hacer LOAD ""), se para donde está
void Motherboard::UpdateTapeMotor() {
	bool loading = m_loaderReads >= LOADER_READS_PER_FRAME;
	m_loaderReads = 0;
	m_framesWithoutLoader = loading ? 0 : m_framesWithoutLoader + 1;

	if (!m_tape)
		return;

	// Si la cinta acaba de arrancar (F9, por ejemplo) tiene su margen entero antes de pararse
	if (m_tape->IsPlaying() && !m_tapeWasPlaying)
		m_framesWithoutLoader = 0;

	if (loading && !m_tape->IsPlaying() && !m_tape->IsAtEnd())
		m_tape->Play();
	else if (!loading && m_tape->IsPlaying() && (m_framesWithoutLoader >= TAPE_IDLE_FRAMES))
		m_tape->Stop();

	m_tapeWasPlaying = m_tape->IsPlaying();
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
			m_z80->IncCycles(content_states[m_z80->GetCycles() % 8]);
			m_ula->WriteByte(offset & 0x3FFF, data);
			break;

		// 8000 - FFFF
		case 2:
		case 3:
			m_saveData.ram[offset & 0x7FFF] = data;
			break;
	}
}

uint8_t Motherboard::ReadMemory(uint16_t offset) const {
	uint8_t data;

	switch (offset >> 14) {
		// 0000 - 3FFF
		case 0:
			data = m_rom->ReadByte(offset);
			break;

		// 4000 - 7FFF
		case 1:
			m_z80->IncCycles(content_states[m_z80->GetCycles() % 8]);
			data = m_ula->ReadByte(offset & 0x3FFF);
			break;

		// 8000 - FFFF
		case 2:
		case 3:
			data = m_saveData.ram[offset & 0x7FFF];
			break;
	}

	return data;
}

void Motherboard::WritePort(uint8_t port, uint8_t value) {
	if (port == 0xFE) {
		m_ula->SetBackColor(value & 0x07);
		if (!m_fast && m_countReadCycles == 0)
			m_sound->WriteSound(this, value);

		if (((value >> 3) & 0x01) != m_lastWriteState) {
			m_lastWriteState = ((value >> 3) & 0x01);
			int32_t diff = (int32_t) m_lastCycles - m_lastWriteCycle;
			if (m_writeCassetteCB)
				m_writeCassetteCB(diff, m_writeCassetteDataCB);
			m_lastWriteCycle = m_lastCycles;
		}

		return;
	}

	printf("WritePort: %.2X: %.2X\n", port, value);
}

uint8_t Motherboard::ReadPort(uint8_t port) const {
	if ((port & 0x20) == 0)
		return m_saveData.kempston;

	// Entra en todas las direcciones pares
	if ((port & 0x01) == 0) {
		assert(m_z80->GetAddressBus().L == port);
		uint8_t row = m_z80->GetAddressBus().H;

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
				value &= m_saveData.keys[i];
			row >>= 1;
		}

		// Un cargador lee el puerto a cada momento (decenas de ciclos entre lecturas)
		int64_t cycles = m_z80->GetCycles();
		if ((cycles - m_lastEarReadCycle) < LOADER_READ_GAP)
			m_loaderReads++;
		m_lastEarReadCycle = cycles;

		if (m_lastReadState)
			value |= 0x40;
		else
			value &= 0xBF;

		return value;
	}

	printf("ReadPort: %.2X\n", port);
	return 0xFF;
}

uint32_t Motherboard::GetCRC32() {
	return m_rom->GetCRC32();
}

namespace {
	// Lo que además va en el estado: por dónde va el frame y la cinta, y el detector del cargador que la arranca y
	// la para. Sin esto, al cargar un estado (o rebobinar) en mitad de una carga, la cinta seguiría donde estaba y
	// la carga fallaría
	struct MachineState {
		int64_t cycles;
		int64_t lastEarReadCycle;
		int32_t cyclesULA;
		int32_t loaderReads;
		int32_t framesWithoutLoader;
		uint8_t tapeWasPlaying;
		awui::Emulation::Spectrum::TapeCorder::Position tape;
	};
} // namespace

int Motherboard::GetSaveSize() {
	int size = sizeof(Motherboard::saveData);
	size += awui::Emulation::Processors::Z80::CPU::GetSaveSize();
	size += awui::Emulation::Spectrum::ULA::GetSaveSize();
	size += sizeof(MachineState);

	return size;
}

void Motherboard::LoadState(uint8_t *data) {
	int offset = 0;
	memcpy(&m_saveData, data, sizeof(Motherboard::saveData));
	offset += sizeof(Motherboard::saveData);
	m_z80->LoadState(&data[offset]);
	offset += awui::Emulation::Processors::Z80::CPU::GetSaveSize();
	m_ula->LoadState(&data[offset]);
	offset += awui::Emulation::Spectrum::ULA::GetSaveSize();

	MachineState machine;
	memcpy(&machine, &data[offset], sizeof(machine));
	m_cycles = machine.cycles;
	m_lastEarReadCycle = machine.lastEarReadCycle;
	m_cyclesULA = (int8_t) machine.cyclesULA;
	m_loaderReads = machine.loaderReads;
	m_framesWithoutLoader = machine.framesWithoutLoader;
	m_tapeWasPlaying = machine.tapeWasPlaying != 0;
	if (m_tape)
		m_tape->SetPosition(machine.tape);
}

void Motherboard::SaveState(uint8_t *data) {
	int offset = 0;
	memcpy(data, &m_saveData, sizeof(Motherboard::saveData));
	offset += sizeof(Motherboard::saveData);
	m_z80->SaveState(&data[offset]);
	offset += awui::Emulation::Processors::Z80::CPU::GetSaveSize();
	m_ula->SaveState(&data[offset]);
	offset += awui::Emulation::Spectrum::ULA::GetSaveSize();

	MachineState machine;
	memset(&machine, 0, sizeof(machine)); // Sin basura en el relleno: el historial guarda lo que cambia byte a byte
	machine.cycles = m_cycles;
	machine.lastEarReadCycle = m_lastEarReadCycle;
	machine.cyclesULA = m_cyclesULA;
	machine.loaderReads = m_loaderReads;
	machine.framesWithoutLoader = m_framesWithoutLoader;
	machine.tapeWasPlaying = m_tapeWasPlaying ? 1 : 0;
	if (m_tape)
		machine.tape = m_tape->GetPosition();
	memcpy(&data[offset], &machine, sizeof(machine));
}

void Motherboard::OnKeyPress(uint8_t row, uint8_t key) {
	m_saveData.keys[row] &= ~key;
	// printf("Press %d: %x\n", row, m_saveData.keys[row]);
}

void Motherboard::ReleaseAllKeys() {
	for (int i = 0; i < 8; i++)
		m_saveData.keys[i] = 0xFF;
}

void Motherboard::OnKeyUp(uint8_t row, uint8_t key) {
	m_saveData.keys[row] |= key;
	// printf("Up %d: %x\n", row, m_saveData.keys[row]);
}

void Motherboard::OnPadEvent(uint8_t status) {
	m_saveData.kempston = status;
}

void Motherboard::SetWriteCassetteCB(void (*fun)(int32_t, void *), void *data) {
	m_writeCassetteCB = fun;
	m_writeCassetteDataCB = data;
}

void Motherboard::SetReadCassetteCB(int32_t (*fun)(void *), void *data) {
	m_readCassetteCB = fun;
	m_readCassetteDataCB = data;
}
