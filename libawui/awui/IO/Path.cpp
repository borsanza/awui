/**
 * awui/IO/Path.cpp
 *
 * Copyright (C) 2024 Borja Sánchez Zamorano
 */

#include "Path.h"

#include <awui/String.h>

using namespace awui;
using namespace awui::IO;

String Path::Combine(const String &path1, const String &path2) {

	if (path1.GetLength() > 0 && !path1.EndsWith(DirectorySeparatorChar)) {
		return path1 + (String) DirectorySeparatorChar + path2;
	}

	return path1 + path2;
}

String Path::Combine(const String &path1, const String &path2, const String &path3) {
	return Path::Combine(Path::Combine(path1, path2), path3);
}

String Path::Combine(const String &path1, const String &path2, const String &path3, const String &path4) {
	return Path::Combine(Path::Combine(path1, path2), Path::Combine(path3, path4));
}
