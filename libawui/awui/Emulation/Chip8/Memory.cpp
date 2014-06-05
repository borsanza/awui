/*
 * awui/Emulation/Chip8/Memory.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Memory.h"

#include <stdint.h>
#include <stdlib.h>
#include <awui/IO/BinaryReader.h>
#include <awui/IO/File.h>
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

	FileStream * fs = File::Open(file, FileMode::Open, FileAccess::Read);
	BinaryReader * br = new BinaryReader(fs);

	while (fs->GetPosition() < fs->GetLength())
		this->_memory->WriteByte(br->ReadByte());

	br->Close();
	fs->Close();

	delete br;
	delete fs;
}