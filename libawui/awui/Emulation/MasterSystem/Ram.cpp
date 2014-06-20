/*
 * awui/Emulation/MasterSystem/Ram.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Ram.h"

#include <awui/IO/MemoryStream.h>

using namespace awui::Emulation::MasterSystem;
using namespace awui::IO;

Ram::Ram() {
	this->_memory = new MemoryStream(8192);
	this->_memory->SetLength(8192);
}

Ram::~Ram() {
	delete this->_memory;
}

uint8_t Ram::ReadByte(int64_t pos) {
	this->_memory->SetPosition(pos);
	return this->_memory->ReadByte();
}

void Ram::WriteByte(int64_t pos, uint8_t value) {
	this->_memory->SetPosition(pos);
	return this->_memory->WriteByte(value);
}