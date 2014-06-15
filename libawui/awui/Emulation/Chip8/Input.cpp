/*
 * awui/Emulation/Chip8/Input.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Input.h"

/*
 * This is the physical distribution
 *
 * 1 2 3 C
 * 4 5 6 D
 * 7 8 9 E
 * A 0 B F
 */

using namespace awui::Emulation::Chip8;

Input::Input() {
	this->_lastKey = -1;
	for (int i = 0; i <= 15; i++)
		this->_keys[i] = false;
}

Input::~Input() {
}

bool Input::IsKeyPressed(uint8_t key) {
	this->_lastKey = -1;
	return this->_keys[key];
}

int Input::GetKey() {
	int r = this->_lastKey;
	this->_lastKey = -1;
	return r;
}

void Input::KeyDown(uint8_t key) {
	this->_keys[key] = true;
	this->_lastKey = key;
}

void Input::KeyUp(uint8_t key) {
	this->_keys[key] = false;
	this->_lastKey = -1;
}