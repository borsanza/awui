/**
 * awui/UI/Emulators/ArcadeContainer.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "ArcadeContainer.h"

#include <awui/Console.h>
#include <awui/IO/MemoryStream.h>
#include <awui/Localization.h>
#include <awui/UI/Station/StationUI.h>

#include <stdio.h>
#include <awui/IO/File.h>
#include <algorithm>
#include <cstring>
#include <vector>

using namespace awui::Drawing;
using namespace awui::UI::Emulators;
using namespace awui::IO;
using namespace awui::UI;
using namespace awui::UI::Station;
using namespace awui::UI::Input;

namespace {
	// Cabecera de los ficheros de estado. Los números van byte a byte en little-endian, igual en cualquier
	// compilador y máquina
	const uint8_t StateMagic[4] = {'A', 'W', 'S', 'T'};
	const uint16_t StateVersion = 1;
	const int StateHeaderSize = 4 + 2 + 4 + 4 + 4; // Marca, versión, sistema, CRC y tamaño de los datos

	void Put16(std::vector<uint8_t> &out, uint16_t value) {
		out.push_back(value & 0xFF);
		out.push_back(value >> 8);
	}

	void Put32(std::vector<uint8_t> &out, uint32_t value) {
		for (int i = 0; i < 4; i++)
			out.push_back((value >> (i * 8)) & 0xFF);
	}

	uint16_t Get16(const uint8_t *in) {
		return in[0] | (in[1] << 8);
	}

	uint32_t Get32(const uint8_t *in) {
		return in[0] | (in[1] << 8) | (in[2] << 16) | ((uint32_t) in[3] << 24);
	}
} // namespace

ArcadeContainer::ArcadeContainer() {
	memcpy(m_stateSystem, "    ", 4);
	m_stateCRC = 0;
	SetBackColor(Color::Black);
	SetDrawShadow(false);
	SetPreventChangeControl(true);
	m_station = NULL;
	SetFocusable(false);
}

void ArcadeContainer::SetStateIdentity(const char *system, uint32_t crc) {
	memset(m_stateSystem, ' ', 4);
	memcpy(m_stateSystem, system, std::min<size_t>(strlen(system), 4));
	m_stateCRC = crc;
}

uint32_t ArcadeContainer::GetFileCRC32(const String &file) {
	std::vector<uint8_t> bytes;
	if (!File::ReadAllBytes(file, bytes) || bytes.empty())
		return 0;

	MemoryStream stream((uint32_t) bytes.size());
	stream.Write(bytes.data(), (uint32_t) bytes.size());
	return stream.GetCRC32();
}

bool ArcadeContainer::WriteStateFile(const String &file, const uint8_t *data, int size) {
	std::vector<uint8_t> bytes;
	bytes.reserve(StateHeaderSize + size);
	bytes.insert(bytes.end(), StateMagic, StateMagic + 4);
	Put16(bytes, StateVersion);
	bytes.insert(bytes.end(), m_stateSystem, m_stateSystem + 4);
	Put32(bytes, m_stateCRC);
	Put32(bytes, (uint32_t) size);
	bytes.insert(bytes.end(), data, data + size);

	// Atómica: si se corta a medias, el estado anterior sigue entero
	if (!File::WriteAllBytes(file, bytes.data(), (int) bytes.size())) {
		Console::Error->WriteLine(String("No se puede guardar el estado: ") + file);
		ShowNotification(Localization::Tr("osd.stateSaveError"));
		return false;
	}

	return true;
}

bool ArcadeContainer::ReadStateFile(const String &file, uint8_t *data, int size) {
	std::vector<uint8_t> bytes;
	if (!File::ReadAllBytes(file, bytes))
		return false;

	const uint8_t *payload = bytes.data();
	size_t payloadSize = bytes.size();

	if ((bytes.size() >= StateHeaderSize) && (memcmp(bytes.data(), StateMagic, 4) == 0)) {
		uint16_t version = Get16(&bytes[4]);
		const uint8_t *system = &bytes[6];
		uint32_t crc = Get32(&bytes[10]);
		uint32_t declared = Get32(&bytes[14]);
		payload += StateHeaderSize;
		payloadSize -= StateHeaderSize;

		// De otro sistema o de otro juego (otra ROM con el mismo nombre, un fichero cambiado de sitio...)
		if ((memcmp(system, m_stateSystem, 4) != 0) || (crc != m_stateCRC)) {
			Console::Error->WriteLine("Estado de otro juego (%.4s, CRC %08X; este es %.4s, CRC %08X): no se carga %s", (const char *) system, crc, m_stateSystem, m_stateCRC, file.ToCharArray());
			ShowNotification(Localization::Tr("osd.stateOtherGame"));
			return false;
		}

		// De otra versión del formato o del emulador
		if ((version != StateVersion) || (declared != payloadSize)) {
			Console::Error->WriteLine("Estado de otra versión (formato %d, %u bytes; se esperaba formato %d): no se carga %s", version, declared, StateVersion, file.ToCharArray());
			ShowNotification(Localization::Tr("osd.stateIncompatible"));
			return false;
		}
	}
	// Sin cabecera: un estado de antes de que la hubiera. No se puede saber de qué juego es; se carga si tiene el
	// tamaño exacto, y la próxima vez se guarda ya con cabecera

	// Un estado de otro tamaño es de otra versión del emulador: no se carga
	if (payloadSize != (size_t) size) {
		Console::Error->WriteLine("Estado incompatible (%zu bytes, se esperaban %d): no se carga %s", payloadSize, size, file.ToCharArray());
		ShowNotification(Localization::Tr("osd.stateIncompatible"));
		return false;
	}

	memcpy(data, payload, size);
	return true;
}

void ArcadeContainer::SetStationUI(StationUI *station) {
	m_station = station;
}

bool ArcadeContainer::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	if (button & RemoteButtons::Menu)
		m_station->ExitingArcade();

	return Button::OnRemoteKeyUp(which, button);
}
