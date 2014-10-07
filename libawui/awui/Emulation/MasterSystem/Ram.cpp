/*
 * awui/Emulation/MasterSystem/Ram.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Ram.h"
#include <stdlib.h>

using namespace awui::Emulation::MasterSystem;

Ram::Ram(int32_t size) {
	this->_data = (uint8_t *) calloc(size, sizeof(uint8_t));
}

Ram::~Ram() {
	free(this->_data);
}

uint8_t Ram::ReadByte(int64_t pos) const {
	return this->_data[pos];
}

void Ram::WriteByte(int64_t pos, uint8_t value) {
	this->_data[pos] = value;
}