/*
 * awui/Windows/Emulators/MasterSystem.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "MasterSystem.h"

#include <awui/Emulation/MasterSystem/CPU.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;

MasterSystem::MasterSystem() {
	this->SetSize(256, 192);
	this->_cpu = new CPU();
	this->SetTabStop(true);
}

MasterSystem::~MasterSystem() {
	delete this->_cpu;
}

int MasterSystem::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::MasterSystem)
		return 1;

	return Control::IsClass(objectClass);
}

void MasterSystem::LoadRom(const String file) {
	this->_cpu->LoadRom(file);
}

void MasterSystem::OnTick() {
	this->_cpu->OnTick();
}