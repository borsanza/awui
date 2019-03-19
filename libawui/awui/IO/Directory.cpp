/**
* awui/IO/Directory.cpp
*
* Copyright (C) 2014 Borja Sánchez Zamorano
*/

#include "Directory.h"

#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

using namespace awui;
using namespace awui::IO;

const String Directory::GetWorkingDirectory() {
	char workingPath[FILENAME_MAX];

	if (GetCurrentDir(workingPath, sizeof(workingPath))) {
		workingPath[sizeof(workingPath) - 1] = '\0';
		return workingPath;
	}

	return "";
}
