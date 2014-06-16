/*
 * samples/awChip8/form1.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "form1.h"

#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/Drawing/Color.h>
#include <awui/Emulation/Chip8/CPU.h>
#include <awui/Windows/Forms/ControlCollection.h>

using namespace awui::Drawing;
using namespace awui::Windows::Emulators;
using namespace awui::Windows::Forms;

Form1::Form1() {
	this->InitializeComponent();
}

Form1::~Form1() {
}

void Form1::InitializeComponent() {
	this->SetBackColor(Color::FromArgb(255, 0, 0, 0));

	for (int i = 0; i < TOTALCHIP8; i++) {
		this->_chip8[i] = new Chip8();
		this->GetControls()->Add(this->_chip8[i]);
	}

	this->_chip8[0]->SetInvertedColors(true);

	this->SetSize(((256 + 2) * 2), ((192 + 2) * 2) + 25);
	this->SetFullscreen(1);
}

void Form1::LoadRom(const awui::String file) {
	this->SetText(file);
	for (int i = 0; i < TOTALCHIP8; i++)
		this->_chip8[i]->LoadRom(file);
}

void Form1::AdjustSizeOfChip8(awui::Windows::Emulators::Chip8 * chip8) {
	int width, height;

	int multiply = 1;
	switch (chip8->GetChip8Mode()) {
		default:
		case awui::Emulation::Chip8::CHIP8:
			width = 64;
			height = 32;
			multiply = 8;
			break;
		case awui::Emulation::Chip8::SUPERCHIP8:
			width = 128;
			height = 64;
			multiply = 4;
			break;
		case awui::Emulation::Chip8::CHIP8HIRES:
			width = 64;
			height = 64;
			multiply = 6;
			break;
		case awui::Emulation::Chip8::MEGACHIP8:
			width = 256;
			height = 192;
			multiply = 2;
			break;
	}

	chip8->SetSize(((width + 2) * multiply), ((height + 2) * multiply));
}

void Form1::OnTick() {
	int x = 32;
	for (int i = 0; i < TOTALCHIP8; i++) {
		this->AdjustSizeOfChip8(this->_chip8[i]);
		this->_chip8[i]->SetLocation(x, (this->GetHeight() - this->_chip8[i]->GetHeight()) / 3);
		x += this->_chip8[i]->GetWidth() + 32;
	}
}
