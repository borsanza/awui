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
#include <awui/Windows/Emulators/Chip8.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/SliderBrowser.h>

#define MULTIPLY 1

using namespace awui::Drawing;
using namespace awui::Windows::Emulators;
using namespace awui::Windows::Forms;

Form1::Form1() {
	this->_invertedColors = true;
	this->_games = new ArrayList();
	this->InitializeComponent();
}

Form1::~Form1() {
	for (int i = 0; i < this->_games->GetCount(); i++) {
		Chip8 * c8 = (Chip8 *)this->_games->Get(i);
		delete c8;
	}

	this->_games->Clear();
}

void Form1::InitializeComponent() {
	this->SetBackColor(Color::FromArgb(255, 8, 8, 8));

	this->_slider = new SliderBrowser();
	this->_slider->SetDock(DockStyle::Fill);
	this->_slider->SetMargin(25);

	this->GetControls()->Add(this->_slider);

	this->SetSize(600, 400);
	this->SetFullscreen(0);
	this->SetText("awChip8");
}

void Form1::LoadRom(const awui::String file) {
	Chip8 * c8 = new Chip8();
	c8->SetSize(256 * MULTIPLY, 192 * MULTIPLY);
	c8->LoadRom(file);
	c8->SetInvertedColors(this->_invertedColors);

	c8->SetTabStop(true);
	this->_games->Add(c8);
	this->_slider->GetControls()->Add(c8);

	if (this->_games->GetCount() == 1)
		c8->SetFocus();
}

void Form1::AdjustSizeOfChip8(awui::Windows::Emulators::Chip8 * chip8) {
	int width, height;

	int multiply = 1;
	int margin = 2;
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
			margin = 0;
			break;
	}

	chip8->SetSize(((width + margin) * multiply), ((height + margin) * multiply));
}

void Form1::OnTick() {
	static Chip8 * selected = NULL;

	if (selected != this->_slider->GetControlSelected()) {
		selected = (Chip8 *) this->_slider->GetControlSelected();
		this->SetText(selected->GetName());
	}
}

bool Form1::OnKeyPress(Keys::Enum key) {
	if (key == Keys::Key_I) {
		this->_invertedColors = !this->_invertedColors;
		for (int i = 0; i < this->_games->GetCount(); i++) {
			Chip8 * c8 = (Chip8 *)this->_games->Get(i);
			c8->SetInvertedColors(this->_invertedColors);
		}
	}

	return Form::OnKeyPress(key);
}
