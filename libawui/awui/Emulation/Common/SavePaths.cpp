/**
 * awui/Emulation/Common/SavePaths.cpp
 *
 * Copyright (C) 2026 Borja Sánchez Zamorano
 */

#include "SavePaths.h"

#include <awui/Console.h>

#include <filesystem>
#include <stdlib.h>

using namespace awui;
using namespace awui::Emulation::Common;

namespace fs = std::filesystem;

void SavePaths::Configure(const String &saveDirectory, const String &romsDirectory) {
	m_saveDirectory = saveDirectory.ToCharArray();
	m_romsDirectory = romsDirectory.ToCharArray();
}

String SavePaths::GetDefaultDirectory(const char *application) {
	fs::path base;
#ifdef _WIN32
	if (const char *appData = getenv("APPDATA"))
		base = appData;
#else
	if (const char *dataHome = getenv("XDG_DATA_HOME"); dataHome && *dataHome)
		base = dataHome;
	else if (const char *home = getenv("HOME"))
		base = fs::path(home) / ".local" / "share";
#endif

	if (base.empty())
		return "";

	return (base / application).string().c_str();
}

std::string SavePaths::Translate(const std::string &path) {
	if (m_saveDirectory.empty())
		return path;

	std::error_code error;
	fs::path file = fs::weakly_canonical(path, error);
	fs::path roms = fs::weakly_canonical(m_romsDirectory, error);

	// Dentro de la carpeta de ROMs se conserva la ruta relativa (sistema y subcarpetas); fuera, solo el nombre
	fs::path relative = file.lexically_relative(roms);
	if (relative.empty() || (*relative.begin() == ".."))
		relative = fs::path("otros") / file.filename();

	return (fs::path(m_saveDirectory) / relative).string();
}

String SavePaths::GetWritePath(const String &pathNextToRom) {
	std::string path = Translate(pathNextToRom.ToCharArray());

	std::error_code error;
	fs::path parent = fs::path(path).parent_path();
	if (!parent.empty() && !fs::create_directories(parent, error) && error)
		Console::Error->WriteLine(String("No se puede crear la carpeta de partidas: ") + parent.string().c_str());

	return path.c_str();
}

String SavePaths::GetReadPath(const String &pathNextToRom) {
	std::string path = Translate(pathNextToRom.ToCharArray());

	std::error_code error;
	if (!fs::exists(path, error) && fs::exists(pathNextToRom.ToCharArray(), error))
		return pathNextToRom;

	return path.c_str();
}
