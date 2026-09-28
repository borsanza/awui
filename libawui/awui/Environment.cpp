// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Environment.h"

#include <awui/Console.h>

#ifdef _WIN32
#include <shlobj.h>
#else
#include <cstdlib>
#endif

using namespace awui;

#ifdef __unix
#ifndef __unix__
#define __unix__
#endif
#endif

String Environment::GetNewLine() {
	static String newLine;

#ifdef __unix__
	newLine = String("\n");
#else
	newLine = String("\r\n");
#endif

	return newLine;
}

String Environment::GetFolderPath(SpecialFolder folder) {
#ifndef _WIN32
	switch (folder) {
		case SpecialFolder::LocalApplicationData: {
			const char *xdg = std::getenv("XDG_DATA_HOME");
			if (xdg && *xdg)
				return String(xdg);
			const char *home = std::getenv("HOME");
			if (home && *home)
				return String(home) + String("/.local/share");
			Console::Error->WriteLine("Error al obtener la ruta de la carpeta especial");
			return "";
		}
		default:
			Console::Error->WriteLine("Folder especial no soportado");
			return "";
	}
#else
	char path[MAX_PATH];
	HRESULT result;

	switch (folder) {
		case SpecialFolder::LocalApplicationData:
			result = SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path);
			break;
		default:
			Console::Error->WriteLine("Folder especial no soportado");
			return "";
	}

	if (result != S_OK) {
		Console::Error->WriteLine("Error al obtener la ruta de la carpeta especial");
		return "";
	}

	return String(path);
#endif
}
