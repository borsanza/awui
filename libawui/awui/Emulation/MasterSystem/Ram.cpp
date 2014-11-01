/*
 * awui/Emulation/MasterSystem/Ram.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Ram.h"
#include <stdlib.h>
#include <string.h>

using namespace awui::Emulation::MasterSystem;

Ram::Ram(uint32_t size) {
	this->_data = (uint8_t *) calloc (size, sizeof(uint8_t));
	this->_size = size;
}

Ram::~Ram() {
	free(this->_data);
}

void Ram::Clear() {
	memset(this->_data, 0, this->_size * sizeof(uint8_t));
}

void Ram::Resize(uint32_t size) {
	if (this->_size != size) {
		free(this->_data);
		this->_data = (uint8_t *) calloc (size, sizeof(uint8_t));
		this->_size = size;
	}
}
