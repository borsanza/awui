/*
 * awui/Emulation/MasterSystem/Rom.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Rom.h"

#include <stdint.h>
#include <stdlib.h>
#include <awui/IO/FileStream.h>
#include <awui/IO/MemoryStream.h>
#include <awui/String.h>

using namespace awui::Emulation::MasterSystem;
using namespace awui::IO;

Rom::Rom(int32_t capacity) {
	this->_rom = new MemoryStream(capacity);
	this->_rom->SetLength(capacity);
}

Rom::~Rom() {
	delete this->_rom;
}

void Rom::LoadRom(const String file) {
	this->_file = file;
	FileStream * fs = new FileStream(file, FileMode::Open, FileAccess::Read);

	if (this->_rom->GetCapacity() < fs->GetLength())
		this->_rom->SetCapacity(fs->GetLength());

	this->_rom->SetPosition(0x0);

	while (fs->GetPosition() < fs->GetLength()) {
		uint8_t b = fs->ReadByte();
//		Console::WriteLine(Convert::ToString(b));
		this->_rom->WriteByte(b);
	}

	fs->Close();

	delete fs;
}

void Rom::Reload() {
	this->_rom->Clear();
	if (this->_file != "")
		this->LoadRom(this->_file);
}

uint8_t Rom::ReadByte(int64_t pos) {
	this->_rom->SetPosition(pos);
	return this->_rom->ReadByte();
}

void Rom::WriteByte(int64_t pos, uint8_t value) {
	this->_rom->SetPosition(pos);
	return this->_rom->WriteByte(value);
}

uint32_t Rom::GetCRC32() {
	return this->_rom->GetCRC32();
}
