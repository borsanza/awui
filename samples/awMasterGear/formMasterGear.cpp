/*
 * samples/awMasterGear/formMasterGear.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formMasterGear.h"

#include <awui/Drawing/Color.h>
#include <awui/Windows/Emulators/DebuggerSMS.h>
#include <awui/Windows/Emulators/MasterSystem.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/SliderBrowser.h>

using namespace awui::Drawing;
using namespace awui::Windows::Emulators;
using namespace awui::Windows::Forms;

FormMasterGear::FormMasterGear() {
	this->_games = new ArrayList();
	this->InitializeComponent();
}

FormMasterGear::~FormMasterGear() {
	for (int i = 0; i < this->_games->GetCount(); i++) {
		MasterSystem * ms = (MasterSystem *)this->_games->Get(i);
		delete ms;
	}

	this->_games->Clear();
}

void FormMasterGear::InitializeComponent() {
	this->SetBackColor(Color::FromArgb(255, 0, 0, 0));

	this->_slider = new SliderBrowser();
	this->_slider->SetDock(DockStyle::Fill);
	this->_slider->SetMargin(25);

	this->_debugger = new DebuggerSMS();
	this->_debugger->SetDock(DockStyle::Right);
	this->_debugger->SetTabStop(false);
	this->_debugger->SetWidth(194);

	this->GetControls()->Add(this->_debugger);
	this->GetControls()->Add(this->_slider);

	this->SetSize(1200, 850);
	this->SetFullscreen(0);
	this->SetText("awMasterGear");
}

void FormMasterGear::LoadRom(const awui::String file) {
	MasterSystem * ms = new MasterSystem();
	this->_debugger->SetRom(ms);
	ms->SetSize(342, 313);
	ms->LoadRom(file);
	this->_games->Add(ms);
	this->_slider->GetControls()->Add(ms);
}