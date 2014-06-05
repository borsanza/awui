/*
 * awui/IO/File.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "File.h"

#include <awui/IO/FileStream.h>
#include <awui/String.h>

using namespace awui::IO;

FileStream * File::Open(const String path, FileMode::Enum mode) {
	return new FileStream(path, mode);
}

FileStream * File::Open(const String path, FileMode::Enum mode, FileAccess::Enum access) {
	return new FileStream(path, mode, access);
}