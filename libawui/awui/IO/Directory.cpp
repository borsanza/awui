/**
* awui/IO/Directory.cpp
*
* Copyright (C) 2014 Borja Sánchez Zamorano
*/

#include "Directory.h"

#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>
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

const String Directory::GetCurrentDirectory() {
	char path[FILENAME_MAX];
	char dest[FILENAME_MAX];
	pid_t pid = getpid();
	sprintf(path, "/proc/%d/exe", pid);
	if (readlink(path, dest, FILENAME_MAX) != -1)
		return dirname(dest);

	return "";
}

const String Directory::GetWorkingDirectory() {
	char workingPath[FILENAME_MAX];

	if (GetCurrentDir(workingPath, sizeof(workingPath))) {
		workingPath[sizeof(workingPath) - 1] = '\0';
		return workingPath;
	}

	return "";
}

const ArrayList * Directory::GetFiles(const String path) {
	ArrayList * ar = new ArrayList();
	DIR *d;
	struct dirent *dir;
	d = opendir(path.ToCharArray());
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (strcmp(dir->d_name, ".") == 0)
				continue;
			if (strcmp(dir->d_name, "..") == 0)
				continue;

			String newFile = path;

			if (!path.EndsWith("/"))
				newFile += "/";

			newFile += dir->d_name;

			// 4 Directory
			// 8 Files
			if (dir->d_type == DT_REG)
				ar->Add(new String(newFile));

			if (dir->d_type == DT_DIR)
				Directory::GetFiles(newFile);
		}

		closedir(d);
	}

	return ar;
}
