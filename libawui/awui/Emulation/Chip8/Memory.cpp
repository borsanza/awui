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
#include <awui/String.h>

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
	FileStream * fs = new FileStream(file, FileMode::Open, FileAccess::Read);

	if (this->_memory->GetCapacity() < fs->GetLength())
		this->_memory->SetCapacity(fs->GetLength());

	this->_memory->SetPosition(0x200);

	while (fs->GetPosition() < fs->GetLength()) {
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

uint8_t Memory::ReadByte(int pos) {
	this->_memory->SetPosition(pos);
	return this->_memory->ReadByte();
}

void Memory::WriteByte(int pos, uint8_t value) {
	this->_memory->SetPosition(pos);
	return this->_memory->WriteByte(value);
}