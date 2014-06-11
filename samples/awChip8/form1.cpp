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
	this->SetBackColor(Color::FromArgb(255, 0, 0, 0));

	this->_chip8 = new Chip8();
//	this->_chip8->SetLocation(32, 32);
//	this->_chip8->SetSize((64 + 2) * 2, (32 + 2) * 2);
	this->_chip8->SetDock(DockStyle::Fill);
	this->GetControls()->Add(this->_chip8);

	this->SetSize(((256 + 2) * 2), ((192 + 2) * 2) + 25);
	this->SetFullscreen(0);
}

void Form1::LoadRom(const awui::String file) {
	this->SetText(file);
	this->_chip8->LoadRom(file);
}