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

#define GAMES 500

FormMasterGear::FormMasterGear() {
	this->_games = new ArrayList();
	this->InitializeComponent();
}

FormMasterGear::~FormMasterGear() {
	for (int i = 0; i < GAMES; i++) {
		MasterSystem * ms = (MasterSystem *)this->_games->Get(i);
		delete ms;
	}

	this->_games->Clear();
}

void FormMasterGear::InitializeComponent() {
	this->_slider = new SliderBrowser();
	this->_slider->SetDock(DockStyle::Fill);
	this->_slider->SetMargin(25);
	
	this->SetBackColor(Color::FromArgb(255, 0, 0, 0));

	for (int i = 0; i < GAMES; i++) {
		MasterSystem * ms = new MasterSystem();
		this->_slider->GetControls()->Add(ms);
		ms->SetSize(342, 313);
		this->_games->Add(ms);
	}

	DebuggerSMS * debugger = new DebuggerSMS((MasterSystem*)this->_games->Get(0));
	debugger->SetDock(DockStyle::Right);
	debugger->SetTabStop(false);
	debugger->SetWidth(194);
	this->GetControls()->Add(debugger);
	this->GetControls()->Add(_slider);

	this->SetSize(1200, 850);
	this->SetFullscreen(0);
}

void FormMasterGear::LoadRom(const awui::String file) {
	this->SetText(file);
	for (int i = 0; i < GAMES; i++) {
		((MasterSystem *)this->_games->Get(i))->LoadRom(file);
	}
}
