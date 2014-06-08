/*
 * awui/Emulation/Chip8/Input.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Input.h"

using namespace awui::Emulation::Chip8;

Input::Input() {
}

Input::~Input() {
}

bool Input::IsKeyPressed(uint8_t key) {
	return false;
}

int Input::GetKey() {
	return -1;
}