/*
 * awui/Emulation/Common/Ram.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Ram.h"
#include <stdlib.h>
#include <string.h>

using namespace awui::Emulation::Common;

Ram::Ram(uint32_t size) {
	m_data = (uint8_t *) calloc(size, sizeof(uint8_t));
	m_size = size;
}

Ram::~Ram() {
	free(m_data);
}

void Ram::Clear() {
	memset(m_data, 0, m_size * sizeof(uint8_t));
}

void Ram::Resize(uint32_t size) {
	if (m_size != size) {
		free(m_data);
		m_data = (uint8_t *) calloc(size, sizeof(uint8_t));
		m_size = size;
	}
}
