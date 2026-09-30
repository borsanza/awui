/*
 * awui/Emulation/Common/Rom.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Rom.h"

#include <awui/IO/File.h>
#include <vector>
#include <awui/String.h>
#include <stdint.h>
#include <stdlib.h>

using namespace awui::Emulation::Common;
using namespace awui::IO;

Rom::Rom(int32_t capacity) {
	m_size = 0;
	m_mask = 0;
	m_rom = new MemoryStream(capacity);
	m_rom->SetLength(capacity);
}

Rom::~Rom() {
	delete m_rom;
}

void Rom::LoadRom(const String file) {
	m_file = file;
	std::vector<uint8_t> data;
	if (!File::ReadAllBytes(file, data)) {
		fprintf(stderr, "No se puede abrir el fichero: %s\n", file.ToCharArray());
		data.clear();
	}

	if (m_rom->GetCapacity() < data.size())
		m_rom->SetCapacity((uint32_t) data.size());

	m_rom->SetPosition(0x0);
	// Si la ROM es más pequeña que la capacidad inicial, la longitud (y el CRC) debe ser la del fichero
	m_rom->SetLength(0);
	m_rom->Write(data.data(), (uint32_t) data.size());

	UpdateSize();
}

void Rom::UpdateSize() {
	m_size = m_rom->GetLength();
	m_mask = 1;
	while (m_mask < m_size)
		m_mask <<= 1;
	m_mask--;
}

void Rom::RemoveHeader(uint32_t bytes) {
	if (bytes >= m_size)
		return;

	for (uint32_t i = bytes; i < m_size; i++)
		m_rom->WriteByte(i - bytes, m_rom->ReadByte(i));

	m_rom->SetLength(m_size - bytes);
	UpdateSize();
}

void Rom::Reload() {
	m_rom->Clear();
	if (m_file != "")
		LoadRom(m_file);
}

uint32_t Rom::GetCRC32() const {
	return m_rom->GetCRC32();
}
