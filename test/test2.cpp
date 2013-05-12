// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "test2.h"

#include <awui/Drawing/Color.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Keyboard.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

Test2::Test2() {
	this->InitializeComponent();
}

Test2::~Test2() {
}

void Test2::InitializeComponent() {
	this->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	this->SetDock(DockStyle::Fill);

	Keyboard * keyboard = new Keyboard();
	keyboard->SetDock(DockStyle::Fill);
	this->GetControls()->Add(keyboard);
}
