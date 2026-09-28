/*
 * awui/Emulation/MasterSystem/Ports.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Ports.h"

using namespace awui::Emulation::MasterSystem;

#include <assert.h>
#include <awui/Emulation/MasterSystem/Motherboard.h>
#include <awui/Emulation/MasterSystem/Sound.h>
#include <awui/Emulation/MasterSystem/VDP.h>
#include <stdio.h>

/*
 *|-----------------------------------|
 *| Port |   Input   |     Output     |
 *|------|-----------|----------------|
 *| 0x3E |           | Memory control |
 *| 0x3F |      I/O port control      |
 *| 0x7E | V counter |                |
 *| 0x7F | H counter |       PSG      |
 *| 0xBE |           |   VDP (data)   |
 *| 0xBF |           |  VDP (control) |
 *| 0xC0 |         Controllers        |
 *| 0xC1 |         Controllers        |
 *| 0xDC |         Controllers        |
 *| 0xDD |         Controllers        |
 *|-----------------------------------|
 */

// 0: USA-EUR (default)
// 1: USA-EUR
// 2: USA-EUR
// 3: Japonesa

#define DEFAULTREGION 0

// Nivel de TH de los dos mandos (bit 0: puerto A, bit 1: puerto B) según el valor del puerto 0x3F
static uint8_t GetTHLevels(uint8_t control) {
	uint8_t a = (control & 0x02) ? 1 : ((control >> 5) & 1);
	uint8_t b = (control & 0x08) ? 1 : ((control >> 7) & 1);
	return a | (b << 1);
}

Ports::Ports() {
	this->_region = DEFAULTREGION;
	this->_getRegion = false;
	this->_maskRegion = 0x00;
}

void Ports::WriteByte(Motherboard *cpu, uint8_t port, uint8_t value) {
	//	printf("Write Port: %.2X    Value: %.2X\n", port, value);
	if (port == 0x7f || port == 0x7e) {
		// printf("Write Port: %.2X    Value: %.2X\n", port, value);
		cpu->GetSound()->WriteByte(cpu, value);
		return;
	}

	if (port >= 0x40 && port <= 0xBF) {
		cpu->GetVDP()->WriteByte(port, value);
		return;
	}

	// Game Gear: reparto estéreo de los canales del PSG
	if ((port == 0x06) && cpu->IsGameGear()) {
		cpu->GetSound()->WriteStereo(cpu, value);
		return;
	}

	// SDSC
	if (port == 0xFD) {
		printf("%c", value);
		fflush(stdout);
		return;
	}

	if (port == 0x3F) {
		// Subir la línea TH de cualquiera de los dos mandos (0 -> 1) captura el contador horizontal.
		// TH A: dirección bit 1 (1 = entrada, queda a 1), salida bit 5. TH B: dirección bit 3, salida bit 7.
		uint8_t oldTH = GetTHLevels(this->_maskRegion);
		uint8_t newTH = GetTHLevels(value);
		if (~oldTH & newTH)
			cpu->GetVDP()->LatchHCounter();

		if (value & 0x01)
			this->_region = (this->_region & 0x02) | ((value >> 5) & 0x01);
		else
			this->_region = (this->_region & 0x02) | (DEFAULTREGION & 0x01);

		if (value & 0x04)
			this->_region = (this->_region & 0x01) | ((value >> 6) & 0x02);
		else
			this->_region = (this->_region & 0x01) | (DEFAULTREGION & 0x02);

		if (DEFAULTREGION == 3)
			this->_region = 3;

		this->_maskRegion = value;

		return;
	}

	//	assert(false);
}

uint8_t Ports::ReadByte(Motherboard *cpu, uint8_t port) const {
	//	printf("Read Port: %.2X\n", port);

	if (port >= 0x40 && port <= 0xBF)
		return cpu->GetVDP()->ReadByte(port);

	// Game Gear: puerto 0x00 = START (bit 7, activo a 0), bit 6 = versión no japonesa, bit 5 = NTSC.
	// Los puertos 0x01-0x05 son del enlace serie y devuelven sus valores por defecto.
	if (cpu->IsGameGear() && port <= 0x06) {
		static const uint8_t serial[6] = {0x00, 0x7F, 0xFF, 0x00, 0xFF, 0x00};
		if (port == 0x00)
			return (cpu->GetStartButton() ? 0x00 : 0x80) | 0x40;
		if (port <= 0x05)
			return serial[port];
		return 0xFF;
	}

	if (port == 0xC0 || port == 0xDC)
		return ((cpu->GetPad2() << 6) | ((cpu->GetPad1() & 0x3F)));

	if (port == 0xC1 || port == 0xDD) {
		uint8_t data = 0x30 | ((cpu->GetPad2() & 0x3F) >> 2);
		if (this->_maskRegion & 0x01)
			data |= (this->_region & 0x01) << 6;
		else
			data |= 0x40;

		if (this->_maskRegion & 0x04)
			data |= (this->_region & 0x02) << 6;
		else
			data |= 0x80;

		return data;
	}

	//	assert(false);
	return 0xFF;
}
