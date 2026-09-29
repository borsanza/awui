/*
 * awui/Emulation/Chip8/Stack.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Stack.h"

using namespace awui::Emulation::Chip8;

void Stack::Push(int value) {
	this->_stack.push_back(value);
}

// Devuelve -1 si la pila está vacía
int Stack::Pop() {
	if (this->_stack.empty())
		return -1;

	int value = this->_stack.back();
	this->_stack.pop_back();
	return value;
}

void Stack::Clear() {
	this->_stack.clear();
}
