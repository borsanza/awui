/*
* awui/IO/Directory.cpp
*
* Copyright (C) 2014 Borja Sánchez Zamorano
*/

#include "Directory.h"

#include <libgen.h>
#include <stdio.h>
#include <unistd.h>

#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
 #endif

using namespace awui;
using namespace awui::IO;

String Directory::GetCurrentDirectory() {
	char path[FILENAME_MAX];
	char dest[FILENAME_MAX];
	pid_t pid = getpid();
	sprintf(path, "/proc/%d/exe", pid);
	if (readlink(path, dest, FILENAME_MAX) != -1)
		return dirname(dest);

	return "";
}

String Directory::GetWorkingDirectory() {
	char workingPath[FILENAME_MAX];

	if (GetCurrentDir(workingPath, sizeof(workingPath))) {
		workingPath[sizeof(workingPath) - 1] = '\0';
		return workingPath;
	}

	return "";
}