/*
 * awui/Emulation/Chip8/Memory.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Memory.h"

#include <algorithm>

#include <awui/Console.h>
#include <awui/IO/File.h>
#include <vector>
#include <awui/IO/MemoryStream.h>
#include <awui/String.h>
#include <stdint.h>
#include <stdlib.h>

using namespace awui::Emulation::Chip8;
using namespace awui::IO;

Memory::Memory(int32_t capacity) {
	m_startAddress = 0x200;
	m_memory = new MemoryStream(capacity);
	m_memory->SetLength(capacity);
}

Memory::~Memory() {
	delete m_memory;
}

void Memory::LoadRom(const String file) {
	m_file = file;
	std::vector<uint8_t> data;
	if (!File::ReadAllBytes(file, data)) {
		Console::Error->WriteLine(String("No se puede abrir el fichero: ") + file);
		data.clear();
	}

	// La ROM empieza en 0x200 (0x600 en el ETI-660). Si no cabe, la memoria crece (MegaChip: hasta 16MB, lo que
	// alcanza I con 24 bits) conservando lo que ya hay, como las fuentes
	m_startAddress = DetectStartAddress(data);
	m_keyMap = KeyMap::Detect(data);
	int64_t needed = m_startAddress + (int64_t) data.size();
	if (needed > MaxCapacity) {
		Console::Error->WriteLine(String("ROM demasiado grande para Chip-8, se trunca: ") + file);
		needed = MaxCapacity;
	}

	if (m_memory->GetCapacity() < needed)
		m_memory->SetCapacity((uint32_t) needed);

	m_memory->SetPosition(m_startAddress);
	m_memory->Write(data.data(), (uint32_t) (needed - m_startAddress));
}

uint16_t Memory::DetectStartAddress(const std::vector<uint8_t> &rom) {
	int64_t size = (int64_t) rom.size();
	int at200 = 0;
	int at600 = 0;
	for (int64_t i = 0; i + 1 < size; i += 2) {
		uint16_t opcode = (rom[i] << 8) | rom[i + 1];
		uint8_t kind = opcode >> 12;
		if ((kind != 0x1) && (kind != 0x2) && (kind != 0xA))
			continue;

		uint16_t target = opcode & 0xFFF;
		if ((target >= 0x200) && (target < 0x200 + size))
			at200++;
		if ((target >= 0x600) && (target < 0x600 + size))
			at600++;
	}

	return ((at200 == 0) && (at600 > 0)) ? 0x600 : 0x200;
}

void Memory::Reload() {
	m_memory->Clear();
	if (m_file != "")
		LoadRom(m_file);
}

// Fuera de la memoria se lee 0 y no se escribe (I puede apuntar a cualquier sitio)
uint8_t Memory::ReadByte(int64_t pos) {
	if ((pos < 0) || (pos >= m_memory->GetCapacity()))
		return 0;

	return m_memory->ReadByte((uint32_t) pos);
}

// MegaChip direcciona hasta 16 MB y los juegos usan como RAM lo que hay por encima de la ROM: escribir más allá de
// la memoria reservada la hace crecer (de 64 KB en 64 KB, sin pasar del máximo), como si siempre hubiera estado ahí.
// Antes se ignoraban esas escrituras y los juegos que guardan datos ahí fallaban
void Memory::WriteByte(int64_t pos, uint8_t value) {
	if ((pos < 0) || (pos >= MaxCapacity))
		return;

	if (pos >= m_memory->GetCapacity()) {
		int64_t capacity = std::min(((pos >> 16) + 1) << 16, MaxCapacity);
		m_memory->SetCapacity((uint32_t) capacity);
	}

	m_memory->WriteByte((uint32_t) pos, value);
}