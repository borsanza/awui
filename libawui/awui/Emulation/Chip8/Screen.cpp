/*
 * awui/Emulation/Chip8/Screen.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Screen.h"

#include <stdlib.h>
#include <awui/Console.h>
#include <awui/String.h>

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

void Screen::WriteConsole() {
	Console::WriteLine("");
	Console::Write(" ");
	for (int j=0; j<this->_width + 2; j++)
		Console::Write("-");
	Console::WriteLine("");
	for (int i=0; i<this->_height; i++) {
		Console::Write(" |");
		for (int j=0; j<this->_width; j++) {
			if (this->_data[(i * this->_width) + j] == 1)
				Console::Write("0");
			else
				Console::Write(" ");
		}
		Console::Write("|");
		Console::WriteLine("");
	}

	Console::Write(" ");
	for (int j=0; j<this->_width + 2; j++)
		Console::Write("-");
	Console::WriteLine("");
}

bool Screen::SetPixelXOR(int x, int y, bool value) {
	int r = false;
	int offset = (y * this->_width) + x;

	int oldValue = this->_data[offset];
	int newValue = oldValue ^ value;

	if (oldValue != newValue) {
		r = true;
		this->_data[offset] = newValue;
	}

	return r;
}

bool Screen::GetPixel(int x, int y) {
	return this->_data[(y * this->_width) + x];
}