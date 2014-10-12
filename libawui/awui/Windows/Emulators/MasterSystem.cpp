/*
 * awui/Windows/Emulators/MasterSystem.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "MasterSystem.h"

#include <awui/Drawing/Image.h>
#include <awui/Emulation/MasterSystem/CPU.h>
#include <awui/Emulation/MasterSystem/VDP.h>
#include <awui/OpenGL/GL.h>

using namespace awui::Drawing;
using namespace awui::OpenGL;
using namespace awui::Windows::Emulators;
using namespace awui::Emulation::MasterSystem;

MasterSystem::MasterSystem() {
	this->SetSize(1, 1);
	this->_image = new Drawing::Image(1, 1);
	this->_cpu = new CPU();
	this->SetTabStop(true);
	this->_multiply = 1;
}

MasterSystem::~MasterSystem() {
	delete this->_cpu;
	delete this->_image;
}

int MasterSystem::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::MasterSystem)
		return 1;

	return Control::IsClass(objectClass);
}

void MasterSystem::LoadRom(const String file) {
	this->SetName(file);
	this->_cpu->LoadRom(file);
}

void MasterSystem::OnTick() {
	this->_cpu->OnTick();
}

CPU * MasterSystem::GetCPU() {
	return this->_cpu;
}

void MasterSystem::OnPaint(GL* gl) {
	uint8_t c, r, g, b;
	uint8_t color[4] {0, 85, 170, 255};
	VDP * screen = this->_cpu->GetVDP();

//	¿Lo rellenamos con el registro 7?
//	c = screen->GetBackColor();
//	r = color[c & 0x3];
//	g = color[(c >> 2) & 0x3];
//	b = color[(c >> 4) & 0x3];
//	this->SetBackColor(Color::FromArgb(255, r, g, b));

	int width = screen->GetVisualWidth();
	int height = screen->GetVisualHeight();

	if ((width != this->_image->GetWidth()) || (height != this->_image->GetHeight())) {
		delete this->_image;
		this->_image = new Drawing::Image(width, height);
		this->SetSize(width * this->_multiply, height * this->_multiply);
	}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			c = screen->GetPixel(x, y);
			r = color[c & 0x3];
			g = color[(c >> 2) & 0x3];
			b = color[(c >> 4) & 0x3];
			this->_image->SetPixel(x, y, r, g, b);
		}
	}

	this->_image->Update();

	GL::DrawImageGL(this->_image, 0, 0, this->GetWidth(), this->GetHeight());
}

bool MasterSystem::OnKeyPress(Keys::Enum key) {
	if (key == Keys::Key_B) {
		VDP * screen = this->_cpu->GetVDP();
		screen->SetShowBorder(!screen->GetShowBorder());
		screen->Clear();;
	}

//	Console::WriteLine("OnKeyPress");
	return true;
}

void MasterSystem::SetMultiply(int multiply) {
	this->_multiply = multiply;
}