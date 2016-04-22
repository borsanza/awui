/*
 * awui/IO/File.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "File.h"

#include <sys/stat.h>

using namespace awui::IO;

bool File::Exists(String path) {
	struct stat buffer;
	return (stat (path.ToCharArray(), &buffer) == 0); 
}
