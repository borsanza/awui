/*
 * samples/awMasterGear/formMasterGear.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formMasterGear.h"

#include <awui/Drawing/Color.h>
//#include <awui/Emulation/MasterSystem/CPU.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Emulators/DebuggerSMS.h>

using namespace awui::Drawing;
using namespace awui::Windows::Emulators;
using namespace awui::Windows::Forms;

FormMasterGear::FormMasterGear() {
	this->InitializeComponent();
}

FormMasterGear::~FormMasterGear() {
}

void FormMasterGear::InitializeComponent() {
	this->SetBackColor(Color::FromArgb(255, 0, 0, 0));

	this->_masterSystem = new MasterSystem();
	this->GetControls()->Add(this->_masterSystem);
	this->_masterSystem->SetLocation(50, 50);

	DebuggerSMS * debugger = new DebuggerSMS(this->_masterSystem);
	debugger->SetDock(DockStyle::Right);
	debugger->SetWidth(194);
	this->GetControls()->Add(debugger);

	this->SetSize(1200, 850);
	this->SetFullscreen(0);
}

void FormMasterGear::LoadRom(const awui::String file) {
	this->SetText(file);
	this->_masterSystem->LoadRom(file);
}
