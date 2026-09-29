/*
 * awui/Emulation/Chip8/Memory.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Memory.h"

#include <awui/Console.h>
#include <awui/IO/FileStream.h>
#include <awui/IO/MemoryStream.h>
#include <awui/String.h>
#include <stdint.h>
#include <stdlib.h>

using namespace awui::Emulation::Chip8;
using namespace awui::IO;

Memory::Memory(int32_t capacity) {
	this->_memory = new MemoryStream(capacity);
	this->_memory->SetLength(capacity);
}

Memory::~Memory() {
	delete this->_memory;
}

void Memory::LoadRom(const String file) {
	this->_file = file;
	FileStream *fs = new FileStream(file, FileMode::Open, FileAccess::Read);

	// La ROM empieza en 0x200. Si no cabe, la memoria crece (MegaChip: hasta 16MB, lo que alcanza I con
	// 24 bits) conservando lo que ya hay, como las fuentes
	int64_t needed = 0x200 + fs->GetLength();
	if (needed > MaxCapacity) {
		Console::Error->WriteLine(String("ROM demasiado grande para Chip-8, se trunca: ") + file);
		needed = MaxCapacity;
	}

	if (this->_memory->GetCapacity() < needed)
		this->_memory->SetCapacity((uint32_t) needed);

	this->_memory->SetPosition(0x200);

	while ((fs->GetPosition() < fs->GetLength()) && (this->_memory->GetPosition() < MaxCapacity)) {
		uint8_t b = fs->ReadByte();
		//		Console::WriteLine(Convert::ToString(b));
		this->_memory->WriteByte(b);
	}

	fs->Close();

	delete fs;
}

void Memory::Reload() {
	this->_memory->Clear();
	if (this->_file != "")
		this->LoadRom(this->_file);
}

// Fuera de la memoria se lee 0 y no se escribe (I puede apuntar a cualquier sitio)
uint8_t Memory::ReadByte(int64_t pos) {
	if ((pos < 0) || (pos >= this->_memory->GetCapacity()))
		return 0;

	return this->_memory->ReadByte((uint32_t) pos);
}

void Memory::WriteByte(int64_t pos, uint8_t value) {
	if ((pos < 0) || (pos >= this->_memory->GetCapacity()))
		return;

	this->_memory->WriteByte((uint32_t) pos, value);
}