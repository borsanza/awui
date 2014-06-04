/*
 * awui/IO/MemoryStream.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "MemoryStream.h"

#include <stdint.h>
#include <stdlib.h>

using namespace awui::IO;

MemoryStream::MemoryStream(int32_t capacity) {
	this->_data = 0;
	this->SetCapacity(capacity);
	this->_position = 0;
	this->_length = 0;
}

MemoryStream::~MemoryStream() {
	if (this->_data)
		free(this->_data);
}

void MemoryStream::Close() {
}

void MemoryStream::SetPosition(int64_t value) {
	this->_position = value;
}

int64_t MemoryStream::GetPosition() {
	return this->_position;
}

int64_t MemoryStream::GetLength() {
	return this->_length;
}

void MemoryStream::SetLength(int64_t value) {
	this->_length = value;
}

void MemoryStream::SetCapacity(int32_t value) {
	if (this->_data)
		free(this->_data);

	this->_data = (uint8_t *) malloc (sizeof(uint8_t *) * value);
	for (int32_t i = 0; i < value; i++)
		this->_data[i] = 0;

	this->_capacity = value;
}

int32_t MemoryStream::GetCapacity() {
	return this->_capacity;
}

uint8_t MemoryStream::ReadByte() {
	uint8_t r = this->_data[this->_position];
	if (this->_position < this->_length)
		this->_position++;
	return r;
}

void MemoryStream::WriteByte(uint8_t value) {
	this->_data[this->_position] = value;
	this->_position++;
	if (this->_position > this->_length)
		this->_length = this->_position;
}