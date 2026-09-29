/**
 * awui/IO/File.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "File.h"

#include <filesystem>
#include <stdio.h>
#include <sys/stat.h>

using namespace awui::IO;

bool File::Exists(String path) {
	struct stat buffer;
	return (stat(path.ToCharArray(), &buffer) == 0);
}

bool File::ReadAllBytes(const String &path, std::vector<uint8_t> &data) {
	FILE *file = fopen(path.ToCharArray(), "rb");
	if (!file)
		return false;

	bool ok = (fseek(file, 0, SEEK_END) == 0);
	long size = ok ? ftell(file) : -1;
	ok = ok && (size >= 0) && (fseek(file, 0, SEEK_SET) == 0);
	if (ok) {
		data.resize((size_t) size);
		ok = (fread(data.data(), 1, data.size(), file) == data.size());
	}

	fclose(file);
	return ok;
}

bool File::WriteAllBytes(const String &path, const uint8_t *data, size_t size) {
	std::string tmp = path.ToStdString() + ".tmp";
	FILE *file = fopen(tmp.c_str(), "wb");
	if (!file)
		return false;

	bool ok = (fwrite(data, 1, size, file) == size);
	ok = (fclose(file) == 0) && ok;

	// std::filesystem::rename sustituye el destino si ya existe (también en Windows, donde rename() no lo hace)
	std::error_code error;
	if (ok)
		std::filesystem::rename(tmp, path.ToStdString(), error);

	if (!ok || error) {
		std::filesystem::remove(tmp, error);
		return false;
	}

	return true;
}

bool File::WriteAllBytes(const String &path, const std::vector<uint8_t> &data) {
	return WriteAllBytes(path, data.data(), data.size());
}
