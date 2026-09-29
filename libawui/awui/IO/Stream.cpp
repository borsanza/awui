/**
 * awui/IO/Stream.cpp
 *
 * Copyright (C) 2017 Borja Sánchez Zamorano
 */

#include "Stream.h"

using namespace awui::IO;

uint32_t Stream::Read(uint8_t *buffer, uint32_t count) {
	uint32_t i = 0;
	for (; (i < count) && (GetPosition() < GetLength()); i++)
		buffer[i] = ReadByte();

	return i;
}

void Stream::Write(const uint8_t *buffer, uint32_t count) {
	for (uint32_t i = 0; i < count; i++)
		WriteByte(buffer[i]);
}

Stream::~Stream() {
}
