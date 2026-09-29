/**
 * samples/stationTV/formArcade.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formArcade.h"

#include <awui/Windows/Forms/Station/Settings/SettingsStore.h>
#include <awui/Windows/Forms/Station/StationUI.h>

using namespace awui;
using namespace awui::Windows::Forms;

FormArcade::FormArcade() {
	m_stationUI = NULL;
	InitializeComponent();
}

FormArcade::~FormArcade() {
	// Es hijo del formulario: si no se saca, ~Control lo borraría otra vez
	RemoveWidget(m_stationUI);
	delete m_stationUI;
}

void FormArcade::InitializeComponent() {
	SetBackColor(Color::FromArgb(0, 0, 0));

	m_stationUI = new StationUI();
	m_stationUI->SetPath("./roms/");
	m_stationUI->Refresh();
	m_stationUI->SetDock(DockStyle::Fill);

	AddWidget(m_stationUI);

	SetSize(1280, 720);
	SetText("StationTV");

	// Pantalla completa, vsync, reloj, FPS y sonido según settings.json
	m_stationUI->ApplySettings();
}

// Si se cierra con un juego abierto, se guarda la partida para continuarla
void FormArcade::OnClosing() {
	m_stationUI->OnClosing();
}

bool FormArcade::OnKeyPress(Keys::Enum key) {
	bool ret = false;
	switch (key) {
		case Keys::Key_F11:
			// Form cambia la pantalla completa al soltar F11: se guarda ya el valor nuevo para que no lo
			// deshaga el siguiente cambio en el menú de ajustes
			Settings::SettingsStore::Instance().SetBool("fullScreen", !GetFullscreen());
			break;
		case Keys::Key_5:
			// Se guarda para que el menú de ajustes muestre el valor real
			SetSwapInterval(!GetSwapInterval());
			Settings::SettingsStore::Instance().SetBool("vsync", GetSwapInterval());
			ret = true;
			break;
	}

	return ret;
}
