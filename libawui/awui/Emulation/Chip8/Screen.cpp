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
	x = x % this->_width;

	int r = false;
	int offset = (y * this->_width) + x;

	int oldValue = this->_data[offset];
	int newValue = oldValue ^ value;

	if (oldValue != newValue) {
		if (newValue == 0)
			r = true;
		this->_data[offset] = newValue;
	}

	return r;
}

bool Screen::GetPixel(int x, int y) {
	return this->_data[(y * this->_width) + x];
}

int Screen::GetWidth() {
	return this->_width;
}

int Screen::GetHeight() {
	return this->_height;
}

void Screen::ScrollDown(int lines) {
	for (int i = this->_height - 1; i >= lines; i--)
		for (int j = 0; j < this->_width; j++)
			this->_data[(i * this->_width) + j] = this->_data[((i - lines) * this->_width) + j];

	for (int i = 0; i < lines; i++)
		for (int j = 0; j < this->_width; j++)
			this->_data[(i * this->_width) + j] = 0;
}

void Screen::ScrollLeft(int columns) {
	for (int scroll = 0; scroll < columns; scroll++) {
		for (int i = 0; i < this->_height; i++) {
			int aux = this->_data[(i * this->_width)];
			for (int j = 0; j < this->_width - 1; j++)
				this->_data[(i * this->_width) + j] = this->_data[(i * this->_width) + (j + 1)];
			this->_data[(i * this->_width) + (this->_width - 1)] = aux;
		}
	}
}

void Screen::ScrollRight(int columns) {
	for (int scroll = 0; scroll < columns; scroll++) {
		for (int i = 0; i < this->_height; i++) {
			int aux = this->_data[(i * this->_width) + (this->_width - 1)];
			for (int j = this->_width - 1; j >= 1; j--)
				this->_data[(i * this->_width) + j] = this->_data[(i * this->_width) + (j - 1)];
			this->_data[(i * this->_width)] = aux;
		}
	}
}