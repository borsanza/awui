/*
 * awui/Emulation/Chip8/Stack.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Stack.h"

#include <awui/Collections/Stack.h>

using namespace awui::Emulation::Chip8;

Stack::Stack() {
	this->_stack = new awui::Collections::Stack();
}

Stack::~Stack() {
	delete this->_stack;
}

void Stack::Push(int value) {
	this->_stack->Push(new StackInt(value));
}

// Devuelve -1 si la pila está vacía
int Stack::Pop() {
	StackInt *o = (StackInt *) this->_stack->Pop();
	if (!o)
		return -1;

	int r = o->GetValue();
	delete o;
	return r;
}

// Se sacan con Pop para liberar cada StackInt (Collections::Stack::Clear solo quita los nodos)
void Stack::Clear() {
	while (this->Pop() != -1)
		;
}