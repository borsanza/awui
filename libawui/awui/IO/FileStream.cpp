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
	m_file = NULL;
	m_length = 0;
	m_pos = 0;

	switch (mode) {
		case FileMode::Enum::Append:
			switch (access) {
				case FileAccess::Enum::Write:
					m_file = fopen(path.ToCharArray(), "ab");
					break;
				case FileAccess::Enum::ReadWrite:
					m_file = fopen(path.ToCharArray(), "a+b");
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
					m_file = fopen(path.ToCharArray(), "r+b");
					break;
				case FileAccess::Enum::Read:
					m_file = fopen(path.ToCharArray(), "rb");
					break;
			}
			break;
		case FileMode::Enum::Create:
			m_file = fopen(path.ToCharArray(), "rb");
			break;
		case FileMode::Enum::CreateNew:
			m_file = fopen(path.ToCharArray(), "rb");
			break;
		case FileMode::Enum::OpenOrCreate:
			m_file = fopen(path.ToCharArray(), "r+b");
			break;
		case FileMode::Enum::Truncate:
			m_file = fopen(path.ToCharArray(), "w+b");
			break;
	}

	// Si no se ha podido abrir (no existe, sin permisos...) queda como un fichero vacío:
	// longitud 0, las lecturas devuelven 0 y las escrituras no hacen nada
	if (!m_file) {
		fprintf(stderr, "No se puede abrir el fichero: %s\n", path.ToCharArray());
		return;
	}

	fseek(m_file, 0L, SEEK_END);
	m_length = ftell(m_file);
	if (m_length < 0)
		m_length = 0;
	fseek(m_file, 0L, SEEK_SET);
}

FileStream::FileStream(const String path, FileMode::Enum mode) :
	FileStream(path, mode, (mode == FileMode::Enum::Append) ? FileAccess::Enum::Write : FileAccess::Enum::ReadWrite) {
}

FileStream::~FileStream() {
	Close();
}

bool FileStream::IsOpen() const {
	return m_file != NULL;
}

void FileStream::Close() {
	if (m_file) {
		fclose(m_file);
		m_file = NULL;
	}
}

void FileStream::SetPosition(uint32_t value) {
	m_pos = value;
	if (m_file)
		fseek(m_file, m_pos, SEEK_SET);
}

uint32_t FileStream::GetPosition() const {
	return m_pos;
}

uint32_t FileStream::GetLength() const {
	return m_length;
}

uint8_t FileStream::ReadByte() {
	uint8_t r = 0;
	if (!m_file || !fread(&r, 1, 1, m_file))
		r = 0;

	m_pos++;
	return r;
}

void FileStream::WriteByte(uint8_t value) {
	Write(&value, 1);
}

uint32_t FileStream::Read(uint8_t *buffer, uint32_t count) {
	if (!m_file)
		return 0;

	uint32_t read = (uint32_t) fread(buffer, 1, count, m_file);
	m_pos += read;
	return read;
}

// Escribir más allá del final alarga el fichero (antes GetLength seguía dando la longitud inicial)
void FileStream::Write(const uint8_t *buffer, uint32_t count) {
	if (m_file)
		fwrite(buffer, 1, count, m_file);

	m_pos += count;
	if (m_pos > m_length)
		m_length = m_pos;
}
