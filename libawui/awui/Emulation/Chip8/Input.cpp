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
	m_lastKey = -1;
	for (int i = 0; i <= 15; i++)
		m_keys[i] = false;
}

Input::~Input() {
}

bool Input::IsKeyPressed(uint8_t key) {
	return m_keys[key];
}

int Input::TakeLastKey() {
	int r = m_lastKey;
	m_lastKey = -1;
	return r;
}

void Input::KeyDown(uint8_t key) {
	m_keys[key] = true;
	m_lastKey = key;
}

void Input::KeyUp(uint8_t key) {
	m_keys[key] = false;
	m_lastKey = -1;
}