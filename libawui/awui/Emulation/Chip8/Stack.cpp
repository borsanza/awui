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

int Stack::Pop() {
	StackInt * o = (StackInt *)this->_stack->Pop();
	int r = o->GetValue();
	delete o;
	return r;
}

void Stack::Clear() {
	this->_stack->Clear();
}