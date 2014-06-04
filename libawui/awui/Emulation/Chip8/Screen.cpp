/*
 * awui/Emulation/Chip8/Screen.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Screen.h"

#include <stdlib.h>

using namespace awui::Emulation::Chip8;

Screen::Screen(int width, int height) {
	this->_width = width;
	this->_height = height;
	this->_data = (unsigned char *) malloc (sizeof(unsigned char *) * width * height);
	this->Clear();
}

Screen::~Screen() {
	free(this->_data);
}

void Screen::Clear() {
	for (int i = 0; i < this->_width * this->_height; i++)
		this->_data[i] = 0;
}