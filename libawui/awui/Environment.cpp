// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Environment.h"

#include <awui/Console.h>
#include <shlobj.h>

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
}
