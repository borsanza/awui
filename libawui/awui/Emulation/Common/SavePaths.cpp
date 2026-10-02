/**
 * awui/Emulation/Common/SavePaths.cpp
 *
 * Copyright (C) 2026 Borja Sánchez Zamorano
 */

#include "SavePaths.h"

#include <awui/Console.h>
#include <awui/Environment.h>

#include <filesystem>

using namespace awui;
using namespace awui::Emulation::Common;

namespace fs = std::filesystem;

void SavePaths::Configure(const String &saveDirectory, const std::vector<String> &romsDirectories) {
	s_saveDirectory = saveDirectory.ToCharArray();
	s_romsDirectories.clear();
	for (const String &directory : romsDirectories)
		s_romsDirectories.push_back(directory.ToCharArray());
}

String SavePaths::GetDefaultDirectory(const char *application) {
	String base = Environment::GetFolderPath(Environment::SpecialFolder::LocalApplicationData);
	if (base.GetLength() == 0)
		return "";

	return (fs::path(base.ToStdString()) / application).string();
}

std::string SavePaths::Translate(const std::string &path, Kind kind) {
	if (s_saveDirectory.empty())
		return path;

	std::error_code error;
	fs::path file = fs::weakly_canonical(path, error);

	// Dentro de una carpeta de ROMs se conserva la ruta relativa (sistema y subcarpetas); fuera, solo el nombre
	fs::path relative = fs::path("otros") / file.filename();
	for (const std::string &directory : s_romsDirectories) {
		fs::path inside = file.lexically_relative(fs::weakly_canonical(directory, error));
		if (!inside.empty() && (*inside.begin() != "..")) {
			relative = inside;
			break;
		}
	}

	return (fs::path(s_saveDirectory) / ((kind == Kind::Save) ? "saves" : "states") / relative).string();
}

String SavePaths::GetWritePath(const String &pathNextToRom, Kind kind) {
	std::string path = Translate(pathNextToRom.ToCharArray(), kind);

	std::error_code error;
	fs::path parent = fs::path(path).parent_path();
	if (!parent.empty() && !fs::create_directories(parent, error) && error)
		Console::Error->WriteLine(String("No se puede crear la carpeta de partidas: ") + parent.string().c_str());

	return path.c_str();
}

String SavePaths::GetReadPath(const String &pathNextToRom, Kind kind) {
	std::string path = Translate(pathNextToRom.ToCharArray(), kind);

	std::error_code error;
	if (!fs::exists(path, error) && fs::exists(pathNextToRom.ToCharArray(), error))
		return pathNextToRom;

	return path.c_str();
}
