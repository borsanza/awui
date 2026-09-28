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
	// Al encender todas las líneas son entradas
	this->_ioControl = 0xFF;
}

void Ports::WriteByte(Motherboard *cpu, uint8_t port, uint8_t value) {
	// Game Gear: 0x00-0x05 son del enlace serie (no emulado, se ignoran) y 0x06 el reparto estéreo del PSG.
	// Los impares no deben caer en el control de E/S (0x3F)
	if (cpu->IsGameGear() && (port <= 0x06)) {
		if (port == 0x06)
			cpu->GetSound()->WriteStereo(cpu, value);
		return;
	}

	// SDSC (consola de depuración de los emuladores, no existe en el hardware)
	if (port == 0xFD) {
		printf("%c", value);
		fflush(stdout);
		return;
	}

	// El hardware solo mira los bits A7, A6 y A0 de la dirección: cada puerto se repite por todo su rango
	switch (port & 0xC1) {
		// 0x00-0x3F pares: control de memoria (no emulado)
		case 0x00:
			return;

		// 0x00-0x3F impares: control de E/S
		case 0x01: {
			// Subir la línea TH de cualquiera de los dos mandos (0 -> 1) captura el contador horizontal
			uint8_t oldTH = GetTHLevels(this->_ioControl);
			uint8_t newTH = GetTHLevels(value);
			if (~oldTH & newTH)
				cpu->GetVDP()->LatchHCounter();

			this->_ioControl = value;
			return;
		}

		// 0x40-0x7F: PSG
		case 0x40:
		case 0x41:
			cpu->GetSound()->WriteByte(cpu, value);
			return;

		// 0x80-0xBF: VDP
		case 0x80:
		case 0x81:
			cpu->GetVDP()->WriteByte(port, value);
			return;

		// 0xC0-0xFF: sin efecto
		default:
			return;
	}
}

// Nivel de una línea de los mandos configurable en el puerto 0x3F (TR o TH).
// Como entrada queda a 1 (resistencia de pull-up); como salida vale lo escrito,
// salvo en las consolas japonesas, donde TH se lee invertido (así detectan la región los juegos).
uint8_t Ports::GetPinLevel(uint8_t directionBit, uint8_t outputBit, bool isTH) const {
	if (this->_ioControl & directionBit)
		return 1;

	uint8_t level = (this->_ioControl & outputBit) ? 1 : 0;
	if (isTH && (DEFAULTREGION == 3))
		level ^= 1;

	return level;
}

uint8_t Ports::ReadByte(Motherboard *cpu, uint8_t port) const {
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

	switch (port & 0xC1) {
		// 0x40-0xBF: contadores y VDP
		case 0x40:
		case 0x41:
		case 0x80:
		case 0x81:
			return cpu->GetVDP()->ReadByte(port);

		// 0xC0-0xFF pares: puerto DC. Bits 0-5 mando 1 (arriba, abajo, izquierda, derecha, botón 1, botón 2), bits 6-7 arriba/abajo del mando 2
		case 0xC0: {
			uint8_t data = (cpu->GetPad2() << 6) | (cpu->GetPad1() & 0x3F);

			// Botón 2 del mando 1 (TR) configurado como salida
			if (!GetPinLevel(0x01, 0x10, false))
				data &= ~0x20;

			return data;
		}

		// 0xC0-0xFF impares: puerto DD. Bits 0-3 mando 2 (izquierda, derecha, botón 1, botón 2),
		// bit 4 RESET (1 = sin pulsar), bit 5 sin uso, bit 6 TH del mando 1, bit 7 TH del mando 2
		case 0xC1: {
			uint8_t data = 0x30 | ((cpu->GetPad2() & 0x3F) >> 2);

			// Botón 2 del mando 2 (TR) configurado como salida
			if (!GetPinLevel(0x04, 0x40, false))
				data &= ~0x08;

			data |= GetPinLevel(0x02, 0x20, true) << 6;
			data |= GetPinLevel(0x08, 0x80, true) << 7;

			return data;
		}

		// 0x00-0x3F: sin dispositivo
		default:
			return 0xFF;
	}
}
