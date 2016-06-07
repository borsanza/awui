/**
 * samples/awArcade/formArcade.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formArcade.h"

#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Station/StationUI.h>

using namespace awui::Drawing;

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

	this->SetSize(1280, 720);
	this->SetFullscreen(0);
	this->SetText("StationTV");
}
