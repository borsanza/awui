/*
 * samples/awSpectrum/formSpectrum.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formSpectrum.h"

#include <awui/Drawing/Color.h>
#include <awui/Emulation/Spectrum/CPU.h>
#include <awui/Windows/Emulators/Spectrum.h>
#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/SliderBrowser.h>

using namespace awui::Drawing;
using namespace awui::Windows::Emulators;
using namespace awui::Windows::Forms;

#define MULTIPLY 3

FormSpectrum::FormSpectrum() {
	this->_games = new ArrayList();
	this->InitializeComponent();
}

FormSpectrum::~FormSpectrum() {
	for (int i = 0; i < this->_games->GetCount(); i++) {
		Spectrum * ms = (Spectrum *)this->_games->Get(i);
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

	int height = 262 * MULTIPLY;
	if (height < 480)
		height = 480;
	this->SetSize((256 * MULTIPLY) + 50, height);
	this->SetFullscreen(0);
	this->SetText("awSpectrum");
}

void FormSpectrum::LoadRom(const awui::String file) {
	Spectrum * ms = new Spectrum();
	ms->GetCPU()->SetMapper(awui::Emulation::Spectrum::MAPPER_SPECTRUM);
	ms->SetMultiply(MULTIPLY);
	ms->SetSize(256 * MULTIPLY, 262 * MULTIPLY);
	ms->LoadRom(file);
	ms->SetTabStop(true);
	this->_games->Add(ms);
	this->_slider->GetControls()->Add(ms);

	if (this->_games->GetCount() == 1)
		ms->SetFocus();
}

void FormSpectrum::OnTick() {
	static Spectrum * selected = NULL;

	if (selected != this->_slider->GetControlSelected()) {
		if (selected != NULL)
			selected->SetSoundEnabled(false);

		selected = (Spectrum *) this->_slider->GetControlSelected();
		selected->SetSoundEnabled(true);
		this->SetText(selected->GetName());
		// printf("case 0x%.8x: // %s\n", this->_debugger->GetCRC32(), selected->GetName().ToCharArray());
	}
}
