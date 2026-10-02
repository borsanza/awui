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

// Las partidas de antes pasan a su sitio: estaban directamente en la carpeta de datos (<datos>/mastersystem/...), o
// todas en states/. Los .sav (lo que guarda el juego) van a saves/ y el resto (estados del emulador) a states/. Si ya
// hay una en el sitio nuevo, se deja la vieja donde estaba
static void MoveOldSaves(const std::filesystem::path &data) {
	namespace fs = std::filesystem;
	std::error_code error;

	// Carpetas de donde mover, y respecto a qué se mantiene la ruta (sistema/juego)
	std::vector<std::pair<fs::path, fs::path>> sources;
	for (const fs::directory_entry &entry : fs::directory_iterator(data, error)) {
		std::string name = entry.path().filename().string();
		if (entry.is_directory(error) && (name != "roms") && (name != "saves") && (name != "states"))
			sources.push_back({entry.path(), data});
	}
	sources.push_back({data / "states", data / "states"});

	bool moved = false;
	for (const auto &[source, base] : sources) {
		bool oldFolder = (source != data / "states");

		// Primero la lista: mover mientras se recorre la misma carpeta no es fiable
		std::vector<fs::path> files;
		for (const fs::directory_entry &entry : fs::recursive_directory_iterator(source, error))
			if (entry.is_regular_file(error))
				files.push_back(entry.path());

		for (const fs::path &file : files) {
			bool save = (file.extension() == ".sav");
			if (!oldFolder && !save)
				continue;

			fs::path target = data / (save ? "saves" : "states") / fs::relative(file, base, error);
			if (fs::exists(target, error))
				continue;

			fs::create_directories(target.parent_path(), error);
			fs::rename(file, target, error);
			moved = true;
		}

		// Las carpetas que se han quedado vacías, de dentro afuera (states/ se queda)
		std::vector<fs::path> directories;
		if (oldFolder)
			directories.push_back(source);
		for (const fs::directory_entry &entry : fs::recursive_directory_iterator(source, error))
			if (entry.is_directory(error))
				directories.push_back(entry.path());
		for (auto it = directories.rbegin(); it != directories.rend(); ++it)
			if (fs::is_empty(*it, error))
				fs::remove(*it, error);
	}

	if (moved)
		awui::Console::WriteLine("Partidas de versiones anteriores repartidas entre saves/ y states/");
}

// m_stationUI es hijo del formulario (lo borra Control)
FormArcade::~FormArcade() {
}

void FormArcade::InitializeComponent() {
	SetBackColor(Color::Black);

	String dataDirectory = awui::Emulation::Common::SavePaths::GetDefaultDirectory("stationtv");

	// Partidas fuera de roms/, que puede ser de solo lectura: en la carpeta de datos del usuario (o en la que diga
	// "saveDirectory" en settings.json), por sistema y juego. En saves/ lo que guarda el juego (la RAM del cartucho) y
	// en states/ los estados del emulador (ver SavePaths)
	String saveDirectory = Settings::SettingsStore::Instance().GetString("saveDirectory").c_str();
	if ((saveDirectory.GetLength() == 0) && (dataDirectory.GetLength() != 0)) {
		saveDirectory = dataDirectory;
		MoveOldSaves(dataDirectory.ToStdString());
	}

	// ROMs: las del usuario (en su carpeta de datos, o en la que diga "romsDirectory" en settings.json) y las que trae
	// el programa (las de CHIP-8). Se ven juntas en la lista
	String userRoms = Settings::SettingsStore::Instance().GetString("romsDirectory").c_str();
	if ((userRoms.GetLength() == 0) && (dataDirectory.GetLength() != 0))
		userRoms = (std::filesystem::path(dataDirectory.ToStdString()) / "roms").string().c_str();
	std::vector<String> roms;
	if (userRoms.GetLength() != 0)
		roms.push_back(userRoms);
	roms.push_back("./roms/");

	// Una subcarpeta por sistema en cada carpeta de ROMs (en la del programa, si se puede escribir en ella: al
	// compilar o en una copia portable), para que se sepa dónde va cada juego. Las vacías no salen en el menú
	for (const String &path : roms)
		StationUI::CreateSystemFolders(path);

	awui::Emulation::Common::SavePaths::Configure(saveDirectory, roms);
	std::filesystem::path base(saveDirectory.ToStdString());
	awui::Console::WriteLine(String("ROMs: ") + userRoms);
	awui::Console::WriteLine(String("Partidas del juego (.sav): ") + (base / "saves").string().c_str());
	awui::Console::WriteLine(String("Estados del emulador (.state, .autostate): ") + (base / "states").string().c_str());

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
