/**
 * awui/UI/Emulators/ArcadeContainer.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "ArcadeContainer.h"

#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/Emulation/Common/SavePaths.h>
#include <awui/IO/MemoryStream.h>
#include <awui/Localization.h>
#include <awui/UI/Station/StationUI.h>

#include <algorithm>
#include <awui/IO/File.h>
#include <cstring>
#include <stdio.h>
#include <vector>

using namespace awui::Drawing;
using namespace awui::Emulation::Common;
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
	m_stateSlot = 0;
	m_paused = false;
	m_pausedByHelp = false;
	AddWidget(&m_keyHelp, WidgetOwnership::Borrowed);
	SetBackColor(Color::Black);
	SetDrawShadow(false);
	SetPreventChangeControl(true);
	m_station = NULL;
	SetFocusable(false);
}

void ArcadeContainer::SetGame(const String &file, const char *system, uint32_t crc) {
	m_gameFile = file;
	memset(m_stateSystem, ' ', 4);
	memcpy(m_stateSystem, system, std::min<size_t>(strlen(system), 4));
	m_stateCRC = crc;
}

// Junto al juego (SavePaths lo lleva a la carpeta de estados): <juego>.state en la ranura 0, <juego>.stateN en las
// demás
awui::String ArcadeContainer::GetStateFile() const {
	String name = String::Concat(m_gameFile, ".state");
	if (m_stateSlot > 0)
		name = String::Concat(name, Convert::ToString(m_stateSlot));

	return name;
}

void ArcadeContainer::SaveState() {
	if (GetStateSize() == 0)
		return;

	String name = SavePaths::GetWritePath(GetStateFile(), SavePaths::Kind::State);
	Console::WriteLine(String("Guardando: ") + name);

	std::vector<uint8_t> data(GetStateSize());
	SaveStateData(data.data());
	if (WriteStateFile(name, data.data(), (int) data.size()))
		ShowNotification(String(Localization::Tr("osd.stateSaved").ToCharArray(), m_stateSlot));
}

void ArcadeContainer::LoadState() {
	if (GetStateSize() == 0)
		return;

	String name = SavePaths::GetReadPath(GetStateFile(), SavePaths::Kind::State);
	if (!File::Exists(name)) {
		ShowNotification(String(Localization::Tr("osd.stateMissing").ToCharArray(), m_stateSlot));
		return;
	}

	Console::WriteLine(String("Cargando: ") + name);

	// Uno que no es de este juego o de esta versión no se carga (ReadStateFile lo avisa)
	std::vector<uint8_t> data(GetStateSize());
	if (ReadStateFile(name, data.data(), (int) data.size())) {
		LoadStateData(data.data());
		ShowNotification(String(Localization::Tr("osd.stateLoaded").ToCharArray(), m_stateSlot));
	}
}

void ArcadeContainer::SetStateSlot(int slot) {
	if (GetStateSize() == 0)
		return;

	m_stateSlot = ((slot % StateSlots) + StateSlots) % StateSlots;
	ShowNotification(String(Localization::Tr("osd.slot").ToCharArray(), m_stateSlot));
}

void ArcadeContainer::SetPaused(bool paused) {
	m_paused = paused;
	ShowNotification(Localization::Tr(paused ? "osd.paused" : "osd.resumed"));
}

void ArcadeContainer::Reset() {
	ResetMachine();
	ShowNotification(Localization::Tr("osd.reset"));
}

bool ArcadeContainer::SaveAutoState() {
	if (GetStateSize() == 0)
		return false;

	std::vector<uint8_t> data(GetStateSize());
	SaveStateData(data.data());
	return WriteStateFile(SavePaths::GetWritePath(String::Concat(m_gameFile, ".autostate"), SavePaths::Kind::State), data.data(), (int) data.size());
}

bool ArcadeContainer::LoadAutoState() {
	if (GetStateSize() == 0)
		return false;

	std::vector<uint8_t> data(GetStateSize());
	if (!ReadStateFile(SavePaths::GetReadPath(String::Concat(m_gameFile, ".autostate"), SavePaths::Kind::State), data.data(), (int) data.size()))
		return false;

	LoadStateData(data.data());
	return true;
}

void ArcadeContainer::ShowKeyHelp() {
	if (m_keyHelp.IsShowing())
		return;

	std::vector<KeyHelp::Section> sections;
	sections.push_back(GetSystemKeys());

	KeyHelp::Section general{Localization::Tr("help.general"), {}};
	general.rows.push_back({"F1", Localization::Tr("help.help")});
	if (GetStateSize() > 0) {
		general.rows.push_back({"F2 / F4", Localization::Tr("help.saveLoad")});
		general.rows.push_back({"F3", Localization::Tr("help.slot")});
	}
	general.rows.push_back({"F5", Localization::Tr("help.pause")});
	general.rows.push_back({"F10", Localization::Tr("help.vsync")});
	general.rows.push_back({"F11", Localization::Tr("help.fullscreen")});
	general.rows.push_back({"F12", Localization::Tr("help.reset")});
	general.rows.push_back({Localization::Tr("help.key.esc"), Localization::Tr("help.back")});
	sections.push_back(general);

	m_keyHelp.Show(Localization::Tr("help.title"), sections);
	MoveToEnd(&m_keyHelp);

	// Pausa sin aviso: se ve la ayuda
	m_pausedByHelp = !m_paused;
	m_paused = true;
}

void ArcadeContainer::HideKeyHelp() {
	if (!m_keyHelp.IsShowing())
		return;

	m_keyHelp.Hide();
	if (m_pausedByHelp)
		m_paused = false;
	m_pausedByHelp = false;
}

bool ArcadeContainer::OnEmulatorKey(Keys::Enum key, bool pressed) {
	// Con la ayuda abierta, cualquier tecla la cierra (y no llega al juego)
	if (m_keyHelp.IsShowing()) {
		if (pressed)
			HideKeyHelp();
		return true;
	}

	switch (key) {
		case Keys::Key_F1:
			if (pressed)
				ShowKeyHelp();
			return true;
		case Keys::Key_F2:
			if (pressed)
				SaveState();
			return true;
		case Keys::Key_F3:
			if (pressed)
				SetStateSlot(m_stateSlot + 1);
			return true;
		case Keys::Key_F4:
			if (pressed)
				LoadState();
			return true;
		case Keys::Key_F5:
			if (pressed)
				SetPaused(!m_paused);
			return true;
		case Keys::Key_F12:
			if (pressed)
				Reset();
			return true;
		default:
			return false;
	}
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
	// Escape (el botón de menú) con la ayuda abierta solo la cierra
	if ((button & RemoteButtons::Menu) && m_keyHelp.IsShowing()) {
		HideKeyHelp();
		return true;
	}

	if (button & RemoteButtons::Menu)
		m_station->ExitingArcade();

	return Button::OnRemoteKeyUp(which, button);
}
