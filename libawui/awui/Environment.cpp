// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Environment.h"

#include <awui/Console.h>

#ifdef _WIN32
// windows.h primero: define lo que usan las otras dos (EXTERN_C...)
#include <windows.h>
#include <knownfolders.h>
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
			// Según la especificación XDG, una ruta relativa no vale y se ignora
			const char *xdg = std::getenv("XDG_DATA_HOME");
			if (xdg && (xdg[0] == '/'))
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
	KNOWNFOLDERID id;
	switch (folder) {
		case SpecialFolder::LocalApplicationData:
			id = FOLDERID_LocalAppData;
			break;
		default:
			Console::Error->WriteLine("Folder especial no soportado");
			return "";
	}

	// Se pide en UTF-16 y se devuelve en UTF-8, como el resto de rutas de awui: la versión ANSI (SHGetFolderPathA)
	// estropea los nombres de usuario con letras fuera de la página de códigos (ñ, acentos en otros idiomas...)
	PWSTR wide = NULL;
	if (FAILED(SHGetKnownFolderPath(id, 0, NULL, &wide))) {
		CoTaskMemFree(wide);
		Console::Error->WriteLine("Error al obtener la ruta de la carpeta especial");
		return "";
	}

	int size = WideCharToMultiByte(CP_UTF8, 0, wide, -1, NULL, 0, NULL, NULL);
	std::string utf8(size > 0 ? size - 1 : 0, '\0');
	if (size > 1)
		WideCharToMultiByte(CP_UTF8, 0, wide, -1, utf8.data(), size, NULL, NULL);
	CoTaskMemFree(wide);

	return String(std::move(utf8));
#endif
}
