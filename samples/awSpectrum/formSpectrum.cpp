/*
 * samples/awSpectrum/formSpectrum.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formSpectrum.h"

#include <awui/Console.h>
#include <awui/String.h>
#include <awui/Windows/Emulators/Chip8.h>
#include <awui/Windows/Emulators/Spectrum.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/SliderBrowser.h>

using namespace awui::Drawing;
using namespace awui::Windows::Emulators;

#define MULTIPLY 2

FormSpectrum::FormSpectrum() {
	this->_games = new ArrayList();
	this->InitializeComponent();
}

FormSpectrum::~FormSpectrum() {
	for (int i = 0; i < this->_games->GetCount(); i++) {
		Object * ms = this->_games->Get(i);
		delete ms;
	}

	this->_games->Clear();
	delete this->_games;
}

void FormSpectrum::InitializeComponent() {
	this->SetBackColor(Color::FromArgb(255, 8, 8, 8));

	this->_slider = new SliderBrowser();
	this->_slider->SetDock(DockStyle::Fill);
	this->_slider->SetMargin(25);

	this->GetControls()->Add(this->_slider);

	this->SetSize((352 * MULTIPLY) + 50, (288 * MULTIPLY) + 50);
	this->SetFullscreen(0);
	this->SetText("awArcade");
}

void FormSpectrum::LoadRom(const awui::String file) {
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
	if (system->CompareTo("zxspectrum") == 0) {
		Spectrum * ms = new Spectrum();
		ms->SetMultiply(1);
	//	ms->LoadOS("");
		ms->LoadRom(file);
		arcade = ms;
	}

	if (system->CompareTo("chip8") == 0) {
		Chip8 * ch8 = new Chip8();
		ch8->LoadRom(file);
		//ch8->SetInvertedColors(this->_invertedColors);
		arcade = ch8;
	}

	delete system;

	if (arcade) {
		arcade->SetTabStop(true);
		this->_games->Add(arcade);
		this->_slider->GetControls()->Add(arcade);

		if (this->_games->GetCount() == 1)
			arcade->SetFocus();
	}
}

void FormSpectrum::OnTick() {
	static ArcadeContainer * selected = NULL;

	if (selected != this->_slider->GetControlSelected()) {
		selected = (ArcadeContainer *) this->_slider->GetControlSelected();
		this->SetText(selected->GetName());
		// printf("case 0x%.8x: // %s\n", this->_debugger->GetCRC32(), selected->GetName().ToCharArray());
	}
}

/*
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
*/
