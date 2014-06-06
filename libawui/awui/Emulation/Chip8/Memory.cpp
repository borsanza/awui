/*
 * awui/Emulation/Chip8/Memory.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Memory.h"

#include <stdint.h>
#include <stdlib.h>
#include <awui/IO/FileStream.h>
#include <awui/IO/MemoryStream.h>

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
	this->_memory->SetPosition(0x200);

	FileStream * fs = new FileStream(file, FileMode::Open, FileAccess::Read);

	while (fs->GetPosition() < fs->GetLength()) {
		uint8_t b = fs->ReadByte();
//		Console::WriteLine(Convert::ToString(b));
		this->_memory->WriteByte(b);
	}

	fs->Close();

	delete fs;
}

uint8_t Memory::ReadByte(int pos) {
	this->_memory->SetPosition(pos);
	return this->_memory->ReadByte();
}