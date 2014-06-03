// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Chip8.h"

#include <awui/Emulation/Chip8/Processor.h>

using namespace awui::Windows::Emulators;

Chip8::Chip8() {
	this->_processor = new Processor();
}

Chip8::~Chip8() {
	delete this->_processor;
}

int Chip8::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Chip8)
		return 1;

	return Control::IsClass(objectClass);
}

void Chip8::LoadRom(const String file) {
	this->_processor->LoadRom(file);
}
