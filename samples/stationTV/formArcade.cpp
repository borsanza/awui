/**
 * samples/stationTV/formArcade.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formArcade.h"

#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Station/StationUI.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

FormArcade::FormArcade() {
	m_stationUI = NULL;
	InitializeComponent();
}

FormArcade::~FormArcade() {
	delete m_stationUI;
}

void FormArcade::InitializeComponent() {
	SetBackColor(Color::FromArgb(0, 0, 0));

	m_stationUI = new StationUI();
	m_stationUI->SetPath("./roms/");
	m_stationUI->Refresh();
	m_stationUI->SetDock(DockStyle::Fill);

	GetControls()->Add(m_stationUI);

	SetSize(1280, 720);
	SetFullscreen(0);
	SetText("StationTV");
}

bool FormArcade::OnKeyPress(Keys::Enum key) {
	bool ret = false;
	switch (key) {
		case Keys::Key_5:
			SetSwapInterval(!GetSwapInterval());
			ret = true;
			break;
	}

	return ret;
}
