// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Memory.h"

#include <stdlib.h>
#include <awui/String.h>

using namespace awui::Emulation::Chip8;

Memory::Memory(unsigned int n) {
	this->_data = (unsigned char *) malloc (sizeof(unsigned char *) * n);
	for (unsigned int i = 0; i < n; i++)
		this->_data[i] = 0;
}

Memory::~Memory() {
	free(this->_data);
}

void Memory::LoadRom(const String file) {

}
