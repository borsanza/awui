/**
 * samples/stationTV/formArcade.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formArcade.h"

#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Station/StationUI.h>

using namespace awui::Drawing;

FormArcade::FormArcade() {
	m_station = NULL;
	InitializeComponent();
}

FormArcade::~FormArcade() {
	delete m_station;
}

void FormArcade::InitializeComponent() {
	SetBackColor(Color::FromArgb(0, 0, 0));

	m_station = new StationUI();
	m_station->SetPath("./roms/");
	m_station->Refresh();
	m_station->SetDock(DockStyle::Fill);

	GetControls()->Add(m_station);

	SetSize(1280, 720);
	SetFullscreen(0);
	SetText("StationTV");
}
