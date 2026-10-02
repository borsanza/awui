/**
 * samples/stationTV/stationTV.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formArcade.h"

#include <awui/Drawing/Graphics.h>
#include <awui/Emulation/MasterSystem/Motherboard.h>
#include <awui/Environment.h>
#include <awui/UI/Application.h>
#include <awui/UI/Emulators/MasterSystem.h>
#include <awui/UI/Station/Settings/SettingsStore.h>

#include <SDL.h>

#include <filesystem>
#include <fstream>
#include <vector>

using namespace awui;
using namespace awui::UI::Emulators;

namespace fs = std::filesystem;

// Los recursos (images/, lang/, fonts/, menu-settings.json y las ROMs que trae el programa) se cargan con rutas
// relativas: se entra en su carpeta. Es la actual si los tiene (al compilar, se lanza desde build/samples/stationTV);
// si no, la del ejecutable (Windows, o una copia portable) o la del paquete instalado (/usr/share/stationtv)
static void EnterResourceDirectory() {
	std::error_code error;
	if (fs::exists("menu-settings.json", error))
		return;

	std::vector<fs::path> candidates;
	char *base = SDL_GetBasePath();
	if (base) {
		candidates.push_back(base);
		SDL_free(base);
	}
#ifdef STATIONTV_DATA_DIR
	candidates.push_back(STATIONTV_DATA_DIR);
#endif

	for (const fs::path &directory : candidates) {
		if (fs::exists(directory / "menu-settings.json", error)) {
			fs::current_path(directory, error);
			return;
		}
	}
}

// settings.json se queda junto a los recursos si se puede escribir ahí (al compilar, o una copia portable). Si no
// (instalado en /usr/share), o si lo instaló el instalador de Windows (junto a su desinstalador: así no acaba en
// Archivos de programa al abrirlo como administrador), va a la configuración del usuario: ~/.config/stationtv (en
// Windows, %APPDATA%\stationtv)
static void ChooseSettingsFile() {
	std::error_code error;
	if (!fs::exists("Uninstall.exe", error)) {
		bool existed = fs::exists("settings.json", error);
		bool writable = std::ofstream("settings.json", std::ios::app).is_open();
		if (!existed)
			fs::remove("settings.json", error);
		if (writable)
			return;
	}

	String config = Environment::GetFolderPath(Environment::SpecialFolder::ApplicationData);
	if (config.GetLength() == 0)
		return;

	fs::path directory = fs::path(config.ToStdString()) / "stationtv";
	fs::create_directories(directory, error);
	UI::Station::Settings::SettingsStore::SetValuesFile((directory / "settings.json").string().c_str());
}

int main(int argc, char **argv) {
	if (argc == 3) {
		String name = argv[1];
		if (name == "--testsms") {
			MasterSystem *ms = new MasterSystem();
			ms->LoadRom(argv[2]);

			while (!ms->GetCPU()->IsEndlessLoop()) {
				ms->RunOpcode();
			}

			// ms->GetCPU()->PrintLog();
			return 0;
		}
	}

	EnterResourceDirectory();
	ChooseSettingsFile();

	// La fuente de la interfaz (Liberation Sans) va con el programa: Windows no la trae. Antes de crear nada que
	// dibuje texto
	Drawing::Graphics::AddFontsFromDirectory("./fonts");

	FormArcade *form = new FormArcade();

	//	for (int i = 1; i < argc; i++) {
	//		form->LoadRom(argv[i]);
	//	}

	Application::Run(form);

	return 0;
}
