/*
 * awui/Emulation/Common/Rom.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Rom.h"

#include <awui/IO/File.h>
#include <vector>
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
	std::vector<uint8_t> data;
	if (!File::ReadAllBytes(file, data)) {
		fprintf(stderr, "No se puede abrir el fichero: %s\n", file.ToCharArray());
		data.clear();
	}

	if (this->_rom->GetCapacity() < data.size())
		this->_rom->SetCapacity((uint32_t) data.size());

	this->_rom->SetPosition(0x0);
	// Si la ROM es más pequeña que la capacidad inicial, la longitud (y el CRC) debe ser la del fichero
	this->_rom->SetLength(0);
	this->_rom->Write(data.data(), (uint32_t) data.size());

	this->UpdateSize();
}

void Rom::UpdateSize() {
	this->_size = this->_rom->GetLength();
	this->_mask = 1;
	while (this->_mask < this->_size)
		this->_mask <<= 1;
	this->_mask--;
}

void Rom::RemoveHeader(uint32_t bytes) {
	if (bytes >= this->_size)
		return;

	for (uint32_t i = bytes; i < this->_size; i++)
		this->_rom->WriteByte(i - bytes, this->_rom->ReadByte(i));

	this->_rom->SetLength(this->_size - bytes);
	this->UpdateSize();
}

void Rom::Reload() {
	this->_rom->Clear();
	if (this->_file != "")
		this->LoadRom(this->_file);
}

uint32_t Rom::GetCRC32() const {
	return this->_rom->GetCRC32();
}
