// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"

#include <awui/Drawing/Color.h>
#include <awui/Windows/Emulators/Chip8.h>
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

	Chip8 * chip8 = new Chip8();
	chip8->LoadRom("maze.ch8");
	this->GetControls()->Add(chip8);

	this->SetSize(460, 460);
	this->SetFullscreen(0);
}