/*
 * awui/Windows/Emulators/Chip8.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Chip8.h"

#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Image.h>
#include <awui/Emulation/Chip8/CPU.h>
#include <awui/Emulation/Chip8/Screen.h>
#include <awui/Emulation/Chip8/Opcode.h>
#include <awui/Math.h>
#include <awui/OpenGL/GL.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;

Chip8::Chip8() {
	this->SetSize(74, 42);
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
		if (this->_cpu->GetChip8Mode() == MEGACHIP8)
			this->SetBackColor(Color::FromArgb(0, 0, 0));
		else
			this->SetBackColor(Color::FromArgb(163, 218, 2));

		Screen * screen = this->_cpu->GetScreen();

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
					if (screen->GetPixel(x, y))
						this->_image->SetPixel(x, y, 50, 88, 4, 255);
					else
						this->_image->SetPixel(x, y, 128, 185, 0, 255);
				}
			}
		}

		this->_image->Update();
		this->_cpu->SetImageUpdated(false);
	}

	int border = (this->_cpu->GetChip8Mode() == MEGACHIP8) ? 0 : 2;

	int borderx = (border * this->GetWidth()) / (this->_image->GetWidth() + border);
	int bordery = (border * this->GetHeight()) / (this->_image->GetHeight() + border);

	int controlWidth = this->GetWidth() - borderx;
	int controlHeight = this->GetHeight() - bordery;

	float ratio = this->_image->GetWidth() / this->_image->GetHeight();
	int width = controlHeight * ratio;
	int height = controlHeight;
	if (width > controlWidth) {
		width = controlWidth;
		height = controlWidth / ratio;
	}

	int left = (this->GetWidth() - width) / 2.0f;
	int top = (this->GetHeight() - height) / 2.0f;

	GL::DrawImageGL(this->_image, left, top, width, height);
}

int Chip8::GetChip8Mode() {
	return this->_cpu->GetChip8Mode();
}

int Chip8::ConvertKeyAwToChip8(Keys::Enum key) {
	int keyPressed = -1;

	switch (key) {
		case Keys::Key_0:
			keyPressed = 0;
			break;

		case Keys::Key_1:
			keyPressed = 1;
			break;

		case Keys::Key_KP8:
		case Keys::Key_2:
			keyPressed = 2;
			break;

		case Keys::Key_3:
			keyPressed = 3;
			break;

		case Keys::Key_KP4:
		case Keys::Key_4:
			keyPressed = 4;
			break;

		case Keys::Key_5:
			keyPressed = 5;
			break;

		case Keys::Key_KP6:
		case Keys::Key_6:
			keyPressed = 6;
			break;

		case Keys::Key_7:
			keyPressed = 7;
			break;

		case Keys::Key_KP2:
		case Keys::Key_8:
			keyPressed = 8;
			break;

		case Keys::Key_9:
			keyPressed = 9;
			break;

		case Keys::Key_A:
			keyPressed = 10;
			break;

		case Keys::Key_B:
			keyPressed = 11;
			break;

		case Keys::Key_C:
			keyPressed = 12;
			break;

		case Keys::Key_D:
			keyPressed = 13;
			break;

		case Keys::Key_E:
			keyPressed = 14;
			break;

		case Keys::Key_F:
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

//	Console::WriteLine("OnKeyPress");
	return true;
}

bool Chip8::OnKeyUp(Keys::Enum key) {
	int keypressed = this->ConvertKeyAwToChip8(key);
	if (keypressed >= 0)
		this->_cpu->KeyUp(keypressed);

//	Console::WriteLine("OnKeyUp");
	return true;

}