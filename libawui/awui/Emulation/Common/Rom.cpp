/*
 * awui/Emulation/Common/Rom.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Rom.h"

#include <awui/IO/FileStream.h>
#include <awui/String.h>
#include <stdint.h>
#include <stdlib.h>

using namespace awui::Emulation::Common;
using namespace awui::IO;

Rom::Rom(int32_t capacity) {
	this->_size = 0;
	this->_mask = 0;
	this->_rom = new MemoryStream(capacity);
	this->_rom->SetLength(capacity);
}

Rom::~Rom() {
	delete this->_rom;
}

void Rom::LoadRom(const String file) {
	this->_file = file;
	FileStream *fs = new FileStream(file, FileMode::Open, FileAccess::Read);

	if (this->_rom->GetCapacity() < fs->GetLength())
		this->_rom->SetCapacity(fs->GetLength());

	this->_rom->SetPosition(0x0);
	// Si la ROM es más pequeña que la capacidad inicial, la longitud (y el CRC) debe ser la del fichero
	this->_rom->SetLength(0);

	while (fs->GetPosition() < fs->GetLength()) {
		uint8_t b = fs->ReadByte();
		//		Console::WriteLine(Convert::ToString(b));
		this->_rom->WriteByte(b);
	}

	fs->Close();

	delete fs;

	this->_size = this->_rom->GetLength();
	this->_mask = 1;
	while (this->_mask < this->_size)
		this->_mask <<= 1;
	this->_mask--;
}

void Rom::Reload() {
	this->_rom->Clear();
	if (this->_file != "")
		this->LoadRom(this->_file);
}

uint32_t Rom::GetCRC32() const {
	return this->_rom->GetCRC32();
}
