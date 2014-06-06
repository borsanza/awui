/*
 * awui/Windows/Emulators/Chip8.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Chip8.h"

#include <awui/Console.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Image.h>
#include <awui/Emulation/Chip8/Processor.h>
#include <awui/Emulation/Chip8/Screen.h>
#include <awui/OpenGL/GL.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;

Chip8::Chip8() {
	this->SetSize(74, 42);
	this->SetBackColor(Color::FromArgb(163, 218, 2));
	this->_image = new Drawing::Image(64, 32);
	this->_processor = new Processor();
}

Chip8::~Chip8() {
	delete this->_processor;
}

int Chip8::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Chip8)
		return 1;

	return Control::IsClass(objectClass);
}

void Chip8::LoadRom(const String file) {
	this->_processor->LoadRom(file);
}

void Chip8::OnTick() {
	this->_processor->OnTick();
}

void Chip8::OnPaint(GL* gl) {
	if (this->_processor->GetImageUpdated()) {
		Screen * screen = this->_processor->GetScreen();
		for (int y = 0; y < screen->GetHeight(); y++) {
			for (int x = 0; x < screen->GetWidth(); x++) {
				if (screen->GetPixel(x, y))
					this->_image->SetPixel(x, y, 50, 88, 4, 255);
				else
					this->_image->SetPixel(x, y, 128, 185, 0, 255);
			}
		}

		this->_image->Update();
		this->_processor->SetImageUpdated(false);
	}

	GL::DrawImageGL(this->_image, 5, 5);
}