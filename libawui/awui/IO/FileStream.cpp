/**
 * awui/IO/FileStream.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "FileStream.h"

#include <assert.h>
#include <awui/String.h>
#include <stdio.h>

using namespace awui::IO;

FileStream::FileStream(const String path, FileMode::Enum mode, FileAccess::Enum access) {
	this->_file = NULL;
	this->_length = 0;
	this->_pos = 0;

	switch (mode) {
		case FileMode::Enum::Append:
			switch (access) {
				case FileAccess::Enum::Write:
					this->_file = fopen(path.ToCharArray(), "ab");
					break;
				case FileAccess::Enum::ReadWrite:
					this->_file = fopen(path.ToCharArray(), "a+b");
					break;
				case FileAccess::Enum::Read:
					assert(0 && "Esto no tiene sentido");
					break;
			}
			break;
		case FileMode::Enum::Open:
			switch (access) {
				case FileAccess::Enum::Write:
				case FileAccess::Enum::ReadWrite:
					this->_file = fopen(path.ToCharArray(), "r+b");
					break;
				case FileAccess::Enum::Read:
					this->_file = fopen(path.ToCharArray(), "rb");
					break;
			}
			break;
		case FileMode::Enum::Create:
			this->_file = fopen(path.ToCharArray(), "rb");
			break;
		case FileMode::Enum::CreateNew:
			this->_file = fopen(path.ToCharArray(), "rb");
			break;
		case FileMode::Enum::OpenOrCreate:
			this->_file = fopen(path.ToCharArray(), "r+b");
			break;
		case FileMode::Enum::Truncate:
			this->_file = fopen(path.ToCharArray(), "w+b");
			break;
	}

	// Si no se ha podido abrir (no existe, sin permisos...) queda como un fichero vacío:
	// longitud 0, las lecturas devuelven 0 y las escrituras no hacen nada
	if (!this->_file) {
		fprintf(stderr, "No se puede abrir el fichero: %s\n", path.ToCharArray());
		return;
	}

	fseek(this->_file, 0L, SEEK_END);
	this->_length = ftell(this->_file);
	if (this->_length < 0)
		this->_length = 0;
	fseek(this->_file, 0L, SEEK_SET);
}

FileStream::FileStream(const String path, FileMode::Enum mode) :
	FileStream(path, mode, (mode == FileMode::Enum::Append) ? FileAccess::Enum::Write : FileAccess::Enum::ReadWrite) {
}

FileStream::~FileStream() {
	Close();
}

bool FileStream::IsOpen() const {
	return this->_file != NULL;
}

void FileStream::Close() {
	if (this->_file) {
		fclose(this->_file);
		this->_file = NULL;
	}
}

void FileStream::SetPosition(uint32_t value) {
	this->_pos = value;
	if (this->_file)
		fseek(this->_file, this->_pos, SEEK_SET);
}

uint32_t FileStream::GetPosition() {
	return this->_pos;
}

uint32_t FileStream::GetLength() {
	return _length;
}

uint8_t FileStream::ReadByte() {
	uint8_t r = 0;
	if (!this->_file || !fread(&r, 1, 1, this->_file))
		r = 0;

	this->_pos++;
	return r;
}

void FileStream::WriteByte(uint8_t value) {
	if (this->_file)
		fwrite(&value, 1, 1, this->_file);
	this->_pos++;
}
