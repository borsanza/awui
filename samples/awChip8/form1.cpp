/*
 * samples/awChip8/form1.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "form1.h"

#include <awui/Drawing/Color.h>
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
	this->SetText("Test Form1");
	this->SetBackColor(Color::FromArgb(255, 0, 0, 0));

	this->_chip8 = new Chip8();
	this->GetControls()->Add(this->_chip8);

	this->SetSize(120, 10);
	this->SetFullscreen(0);
}

void Form1::LoadRom(const awui::String file) {
	this->_chip8->LoadRom(file);
}
