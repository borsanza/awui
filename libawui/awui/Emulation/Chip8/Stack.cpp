/*
 * awui/Emulation/Chip8/Stack.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Stack.h"

using namespace awui::Emulation::Chip8;

void Stack::Push(int value) {
	m_stack.push_back(value);
}

// Devuelve -1 si la pila está vacía
int Stack::Pop() {
	if (m_stack.empty())
		return -1;

	int value = m_stack.back();
	m_stack.pop_back();
	return value;
}

void Stack::Clear() {
	m_stack.clear();
}
