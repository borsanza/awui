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
	this->_cpu->LoadRom(file);
}

void MasterSystem::OnTick() {
	this->_cpu->OnTick();
}

CPU * MasterSystem::GetCPU() {
	return this->_cpu;
}

void MasterSystem::OnPaint(GL* gl) {
	this->SetBackColor(Color::FromArgb(0, 0, 0, 0));

	VDP * screen = this->_cpu->GetVDP();
	int width = screen->GetTotalWidth();
	int height = screen->GetTotalHeight();

	if ((width != this->_image->GetWidth()) || (height != this->_image->GetHeight())) {
		delete this->_image;
		this->_image = new Drawing::Image(width, height);
		this->SetSize(width * 2, height * 2);
	}

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			uint8_t c = screen->GetPixel(x, y);
			uint8_t r = (c & 0x3) * 85;
			uint8_t g = ((c >> 2) & 0x3) * 85;
			uint8_t b = ((c >> 4) & 0x3) * 85;
			this->_image->SetPixel(x, y, r, g, b, 255);
		}
	}

	this->_image->Update();

	int controlWidth = this->GetWidth();
	int controlHeight = this->GetHeight();

	float ratio = (float)this->_image->GetWidth() / (float)this->_image->GetHeight();

	width = controlHeight * ratio;
	height = controlHeight;

	if (width > controlWidth) {
		width = controlWidth;
		height = (float)controlWidth / ratio;
	}

	int left = (this->GetWidth() - width) / 2.0f;
	int top = (this->GetHeight() - height) / 2.0f;

	// Console::Write(Convert::ToString(width));
	// Console::Write("x");
	// Console::WriteLine(Convert::ToString(height));

	GL::DrawImageGL(this->_image, left, top, width, height);
}