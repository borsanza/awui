/*
 * awui/Windows/Emulators/Chip8.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Chip8.h"

#include <awui/Console.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Image.h>
#include <awui/Emulation/Chip8/CPU.h>
#include <awui/Emulation/Chip8/Screen.h>
#include <awui/Math.h>
#include <awui/OpenGL/GL.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;

Chip8::Chip8() {
	this->SetSize(74, 42);
	this->SetBackColor(Color::FromArgb(163, 218, 2));
	this->_image = new Drawing::Image(64, 32);
	this->_cpu = new CPU();
	this->SetTabStop(true);
}

Chip8::~Chip8() {
	delete this->_cpu;
}

int Chip8::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Chip8)
		return 1;

	return Control::IsClass(objectClass);
}

void Chip8::LoadRom(const String file) {
	this->_cpu->LoadRom(file);
}

void Chip8::OnTick() {
	this->_cpu->OnTick();
}

void Chip8::OnPaint(GL* gl) {
	if (this->_cpu->GetImageUpdated()) {
		Screen * screen = this->_cpu->GetScreen();

		if ((screen->GetWidth() != this->_image->GetWidth()) || (screen->GetHeight() != this->_image->GetHeight())) {
			delete this->_image;
			this->_image = new Drawing::Image(screen->GetWidth(), screen->GetHeight());
		}

		for (int y = 0; y < screen->GetHeight(); y++) {
			for (int x = 0; x < screen->GetWidth(); x++) {
				if (screen->GetPixel(x, y))
					this->_image->SetPixel(x, y, 50, 88, 4, 255);
//					this->_image->SetPixel(x, y, 255, 255, 255, 255);
				else
					this->_image->SetPixel(x, y, 128, 185, 0, 255);
//					this->_image->SetPixel(x, y, 0, 0, 0, 255);
			}
		}

		this->_image->Update();
		this->_cpu->SetImageUpdated(false);
	}

	int border = 1;

	int borderx = Math::Round((this->GetWidth() * border) / ((float)this->_image->GetWidth() + (border * 2)));
	int bordery = Math::Round((this->GetHeight() * border) / ((float)this->_image->GetHeight() + (border * 2)));

	GL::DrawImageGL(this->_image, borderx, bordery, this->GetWidth() - (borderx * 2), this->GetHeight() - (bordery * 2));
}