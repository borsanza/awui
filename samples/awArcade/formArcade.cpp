/**
 * samples/awArcade/formArcade.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formArcade.h"

#include <awui/Console.h>
#include <awui/String.h>
#include <awui/Emulation/MasterSystem/Motherboard.h>
#include <awui/Windows/Emulators/Chip8.h>
#include <awui/Windows/Emulators/DebuggerSMS.h>
#include <awui/Windows/Emulators/MasterSystem.h>
#include <awui/Windows/Emulators/Spectrum.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Station/StationUI.h>

using namespace awui::Drawing;
using namespace awui::Windows::Emulators;

#define MULTIPLY 1

FormArcade::FormArcade() {
	this->InitializeComponent();
}

FormArcade::~FormArcade() {
}

void FormArcade::InitializeComponent() {
	this->SetBackColor(Color::FromArgb(0, 0, 0));

	this->_station = new StationUI();
	this->_station->SetPath("./roms/");
	this->_station->Refresh();
	this->_station->SetDock(DockStyle::Fill);

	this->GetControls()->Add(this->_station);

	this->SetSize((352 * MULTIPLY) + 50, (304 * MULTIPLY) + 50);
	this->SetFullscreen(0);
	this->SetText("awArcade");
}

/*
void FormArcade::LoadRom(const awui::String file) {
	ArrayList list = file.Split("/");
	int found = -1;
	String * system = 0;
	for (int i = 0; i < list.GetCount(); i++) {
		String * name = (String *)list.Get(i);

		if (name->CompareTo("roms") == 0)
			found = i + 1;

		if (found == i)
            system = name;
		else
			delete name;
	}

	if (!system)
		return;

	ArcadeContainer * arcade = 0;
	do {
		if (*system == "mastersystem") {
			MasterSystem * sms = new MasterSystem();
			sms->SetMultiply(MULTIPLY);
			sms->SetSize(256 * MULTIPLY, 262 * MULTIPLY);
			sms->LoadRom(file);
			arcade = sms;
			break;
		}

		if (*system == "zxspectrum") {
			Spectrum * szx = new Spectrum();
			szx->SetMultiply(MULTIPLY);
			szx->LoadRom(file);
			arcade = szx;
			break;
		}

		if (*system == "chip8") {
			Chip8 * ch8 = new Chip8();
			ch8->LoadRom(file);
			arcade = ch8;
			break;
		}
	} while (false);

	delete system;

	if (arcade) {
		String name = file;
		name = name.Substring(0, name.LastIndexOf("."));
		name = name.Substring(name.LastIndexOf("/") + 1);
		arcade->SetName(name);

		arcade->SetTabStop(true);
		this->_games->Add(arcade);
		this->_station->GetControls()->Add(arcade);

		if (this->_games->GetCount() == 1)
			arcade->SetFocus();
	}
}

void FormArcade::OnTick() {

	static ArcadeContainer * selected = NULL;

	if (selected != this->_station->GetControlSelected()) {
		if (selected != NULL) {
			this->_debugger->SetShow(false);
			selected->SetSoundEnabled(false);
			selected->SetDebugger(0);
			this->_debugger->SetRom(0);
		}

		selected = (ArcadeContainer *) this->_station->GetControlSelected();
		selected->SetSoundEnabled(true);
		if (selected->GetType() == 2)
			this->_debugger->SetRom((MasterSystem *) selected);

		selected->SetDebugger(this->_debugger);
		this->SetText(selected->GetName());
		// printf("case 0x%.8x: // %s\n", this->_debugger->GetCRC32(), selected->GetName().ToCharArray());
	}

}
*/
