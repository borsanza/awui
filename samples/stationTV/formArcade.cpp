/**
 * samples/stationTV/formArcade.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formArcade.h"

#include <awui/Console.h>
#include <awui/Emulation/Common/SavePaths.h>
#include <awui/UI/Station/Settings/SettingsStore.h>
#include <awui/UI/Station/StationUI.h>

#include <filesystem>
#include <vector>

using namespace awui::Drawing;
using namespace awui;
using namespace awui::UI;
using namespace awui::UI::Input;

FormArcade::FormArcade() {
	m_stationUI = NULL;
	InitializeComponent();
}

// m_stationUI es hijo del formulario (lo borra Control)
FormArcade::~FormArcade() {
}

void FormArcade::InitializeComponent() {
	SetBackColor(Color::Black);

	// Partidas (.sav, estados) fuera de roms/, que puede ser de solo lectura: en la carpeta de datos del usuario
	// o en la que diga "saveDirectory" en settings.json
	String saveDirectory = Settings::SettingsStore::Instance().GetString("saveDirectory").c_str();
	if (saveDirectory.GetLength() == 0)
		saveDirectory = awui::Emulation::Common::SavePaths::GetDefaultDirectory("stationtv");

	// ROMs: las del usuario (en su carpeta de datos, o en la que diga "romsDirectory" en settings.json) y las que trae
	// el programa (las de CHIP-8). Se ven juntas en la lista; la carpeta del usuario se crea para que sepa dónde van
	String userRoms = Settings::SettingsStore::Instance().GetString("romsDirectory").c_str();
	String dataDirectory = awui::Emulation::Common::SavePaths::GetDefaultDirectory("stationtv");
	if ((userRoms.GetLength() == 0) && (dataDirectory.GetLength() != 0))
		userRoms = (std::filesystem::path(dataDirectory.ToStdString()) / "roms").string().c_str();
	std::vector<String> roms;
	if (userRoms.GetLength() != 0) {
		std::error_code error;
		std::filesystem::create_directories(userRoms.ToCharArray(), error);
		roms.push_back(userRoms);
	}
	roms.push_back("./roms/");

	awui::Emulation::Common::SavePaths::Configure(saveDirectory, roms);
	awui::Console::WriteLine(String("Partidas guardadas en: ") + saveDirectory);
	awui::Console::WriteLine(String("ROMs en: ") + userRoms);

	m_stationUI = new StationUI();
	m_stationUI->SetPaths(roms);
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
