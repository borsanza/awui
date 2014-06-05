/*
 * awui/IO/BinaryReader.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "BinaryReader.h"

#include <awui/IO/Stream.h>

using namespace awui::IO;

BinaryReader::BinaryReader(Stream * input) {
	this->_stream = input;
}

BinaryReader::~BinaryReader() {
}

void BinaryReader::Close() {
}

uint8_t BinaryReader::ReadByte() {
	return this->_stream->ReadByte();
}