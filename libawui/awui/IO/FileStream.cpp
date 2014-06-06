/*
 * awui/IO/FileStream.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "FileStream.h"

#include <awui/String.h>
#include <assert.h>
#include <stdio.h>

using namespace awui::IO;

FileStream::FileStream(const String path, FileMode::Enum mode, FileAccess::Enum access) {
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

	fseek(this->_file, 0L, SEEK_END);
	this->_length = ftell(this->_file);
	fseek(this->_file, 0L, SEEK_SET);
	this->_pos = 0;
}

FileStream::FileStream(const String path, FileMode::Enum mode) {
	if (mode == FileMode::Enum::Append)
		FileStream(path, mode, FileAccess::Enum::Write);
	else
		FileStream(path, mode, FileAccess::Enum::ReadWrite);
}

FileStream::~FileStream() {
}

void FileStream::Close() {
	fclose(this->_file);
}

void FileStream::SetPosition(int64_t value) {
	this->_pos = value;
	fseek(this->_file, this->_pos, SEEK_SET);
}

int64_t FileStream::GetPosition() {
	return this->_pos;
}

int64_t FileStream::GetLength() {
	return _length;
}

uint8_t FileStream::ReadByte() {
	uint8_t r;
	fread(&r, 1, 1, this->_file);
	this->_pos++;
	return r;
}

void FileStream::WriteByte(uint8_t value) {
	fwrite(&value, 1, 1, this->_file);
	this->_pos++;
}