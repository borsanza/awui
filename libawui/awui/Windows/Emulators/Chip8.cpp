/**
 * awui/Windows/Emulators/Chip8.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Chip8.h"

#include <awui/Drawing/Image.h>
#include <awui/Emulation/Chip8/CPU.h>
#include <awui/Emulation/Chip8/Screen.h>
#include <awui/OpenGL/GL.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;

Chip8::Chip8() {
	this->_image = new Drawing::Image(64, 32);
	this->_cpu = new CPU();
	this->SetTabStop(false);
	this->_invertedColors = false;
}

Chip8::~Chip8() {
	delete this->_cpu;
}

int Chip8::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Chip8)
		return 1;

	return Control::IsClass(objectClass);
}

void Chip8::CheckBackcolor() {
	if (this->_cpu->GetChip8Mode() == MEGACHIP8)
		this->SetBackColor(Color::FromArgb(0, 0, 0));
	else {
		if (!this->_invertedColors)
			this->SetBackColor(Color::FromArgb(163, 218, 2));
		else
			this->SetBackColor(Color::FromArgb(0, 0, 0));
	}
}

void Chip8::LoadRom(const String file) {
	this->_cpu->LoadRom(file);
	this->SetName(file);

	this->_cpu->OnTick();
	this->CheckBackcolor();
}

void Chip8::OnTick() {
	this->_cpu->OnTick();
}

void Chip8::OnPaint(GL* gl) {
	Screen * screen = this->_cpu->GetScreen();

	if (this->_cpu->GetImageUpdated()) {
		this->CheckBackcolor();

		if ((screen->GetWidth() != this->_image->GetWidth()) || (screen->GetHeight() != this->_image->GetHeight())) {
			delete this->_image;
			this->_image = new Drawing::Image(screen->GetWidth(), screen->GetHeight());
		}

		if (this->_cpu->GetChip8Mode() == MEGACHIP8) {
			for (int y = 0; y < screen->GetHeight(); y++) {
				for (int x = 0; x < screen->GetWidth(); x++) {
					uint32_t pixel = screen->GetPixel(x, y);
					this->_image->SetPixel(x, y, (pixel >> 16) & 0xFF, (pixel >> 8) & 0xFF, pixel & 0xFF, 255);
				}
			}
		} else {
			for (int y = 0; y < screen->GetHeight(); y++) {
				for (int x = 0; x < screen->GetWidth(); x++) {
					if (!this->_invertedColors) {
						if (screen->GetPixel(x, y))
							this->_image->SetPixel(x, y, 50, 88, 4, 255);
						else
							this->_image->SetPixel(x, y, 128, 185, 0, 255);
					} else {
						if (screen->GetPixel(x, y))
							this->_image->SetPixel(x, y, 255, 255, 255, 255);
						else
							this->_image->SetPixel(x, y, 0, 0, 0, 255);
					}
				}
			}
		}

		this->_image->Update();
		this->_cpu->SetImageUpdated(false);
	}

	int width = screen->GetWidth();
	int height = screen->GetHeight();

	float w = width;
	float h = height;

	float ratio = w / h;
	w = this->GetWidth();
	h = this->GetWidth() / ratio;

	if (h > this->GetHeight()) {
		h = this->GetHeight();
		w = this->GetHeight() * ratio;
	}

	int ratio2 = w / width;
	if (ratio2 >= 1) {
		w = width * ratio2;
		h = height * ratio2;
	}

	GL::DrawImageGL(this->_image, int(this->GetWidth() - w) >> 1, int(this->GetHeight() - h) >> 1, w, h);
}

int Chip8::GetChip8Mode() {
	return this->_cpu->GetChip8Mode();
}

int Chip8::ConvertKeyAwToChip8(Keys::Enum key) {
	int keyPressed = -1;

	switch (key) {
		case Keys::Key_X:
		case Keys::Key_0:
			keyPressed = 0;
			break;

		case Keys::Key_1:
		case Keys::Key_KP7:
			keyPressed = 1;
			break;

		case Keys::Key_KP8:
		case Keys::Key_2:
			keyPressed = 2;
			break;

		case Keys::Key_3:
		case Keys::Key_KP9:
			keyPressed = 3;
			break;

		case Keys::Key_Q:
		case Keys::Key_KP4:
			keyPressed = 4;
			break;

		case Keys::Key_W:
		case Keys::Key_KP5:
		case Keys::Key_5:
			keyPressed = 5;
			break;

		case Keys::Key_E:
		case Keys::Key_KP6:
		case Keys::Key_6:
			keyPressed = 6;
			break;

		case Keys::Key_A:
		case Keys::Key_7:
		case Keys::Key_KP1:
			keyPressed = 7;
			break;

		case Keys::Key_S:
		case Keys::Key_KP2:
		case Keys::Key_8:
			keyPressed = 8;
			break;

		case Keys::Key_D:
		case Keys::Key_9:
		case Keys::Key_KP3:
			keyPressed = 9;
			break;

		case Keys::Key_Z:
			keyPressed = 10;
			break;

		case Keys::Key_C:
		case Keys::Key_B:
			keyPressed = 11;
			break;

		case Keys::Key_4:
			keyPressed = 12;
			break;

		case Keys::Key_R:
			keyPressed = 13;
			break;

		case Keys::Key_F:
			keyPressed = 14;
			break;

		case Keys::Key_V:
			keyPressed = 15;
			break;

		default:
			break;
	}

	return keyPressed;
}

bool Chip8::OnKeyPress(Keys::Enum key) {
	int keypressed = this->ConvertKeyAwToChip8(key);
	if (keypressed >= 0)
		this->_cpu->KeyDown(keypressed);

	if (key == Keys::Key_I)
		this->SetInvertedColors(!this->_invertedColors);

	return true;
}

bool Chip8::OnKeyUp(Keys::Enum key) {
	int keypressed = this->ConvertKeyAwToChip8(key);
	if (keypressed >= 0)
		this->_cpu->KeyUp(keypressed);

	return true;
}

void Chip8::SetInvertedColors(bool mode) {
	this->_invertedColors = mode;
	this->_cpu->SetImageUpdated(true);
}
