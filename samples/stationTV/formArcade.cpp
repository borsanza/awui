/**
 * samples/stationTV/formArcade.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formArcade.h"

#include <awui/Console.h>
#include <awui/Emulation/Common/SavePaths.h>
#include <awui/Windows/Forms/Station/Settings/SettingsStore.h>
#include <awui/Windows/Forms/Station/StationUI.h>

using namespace awui;
using namespace awui::Windows::Forms;

FormArcade::FormArcade() {
	m_stationUI = NULL;
	InitializeComponent();
}

// m_stationUI es hijo del formulario (lo borra Control)
FormArcade::~FormArcade() {
}

void FormArcade::InitializeComponent() {
	SetBackColor(Color::FromArgb(0, 0, 0));

	// Partidas (.sav, estados) fuera de roms/, que puede ser de solo lectura: en la carpeta de datos del usuario
	// o en la que diga "saveDirectory" en settings.json
	String saveDirectory = Settings::SettingsStore::Instance().GetString("saveDirectory").c_str();
	if (saveDirectory.GetLength() == 0)
		saveDirectory = awui::Emulation::Common::SavePaths::GetDefaultDirectory("stationtv");
	awui::Emulation::Common::SavePaths::Configure(saveDirectory, "./roms/");
	awui::Console::WriteLine(String("Partidas guardadas en: ") + saveDirectory);

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
