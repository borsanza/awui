/**
 * awui/UI/Emulators/ArcadeContainer.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "ArcadeContainer.h"

#include <awui/Console.h>
#include <awui/UI/Station/StationUI.h>

#include <stdio.h>
#include <awui/IO/File.h>
#include <cstring>
#include <vector>

using namespace awui::Drawing;
using namespace awui::UI::Emulators;
using namespace awui::IO;
using namespace awui::UI;
using namespace awui::UI::Station;
using namespace awui::UI::Input;

ArcadeContainer::ArcadeContainer() {
	SetBackColor(Color::FromArgb(0, 0, 0));
	SetDrawShadow(false);
	SetPreventChangeControl(true);
	m_station = NULL;
	SetFocusable(false);
}

bool ArcadeContainer::WriteStateFile(const String &file, const uint8_t *data, int size) {
	// Atómica: si se corta a medias, el estado anterior sigue entero
	if (!File::WriteAllBytes(file, data, size)) {
		Console::Error->WriteLine(String("No se puede guardar el estado: ") + file);
		return false;
	}

	return true;
}

bool ArcadeContainer::ReadStateFile(const String &file, uint8_t *data, int size) {
	std::vector<uint8_t> bytes;
	if (!File::ReadAllBytes(file, bytes))
		return false;

	// Un estado de otro tamaño es de otra versión del emulador: no se carga
	if (bytes.size() != (size_t) size) {
		Console::Error->WriteLine("Estado incompatible (%zu bytes, se esperaban %d): no se carga %s", bytes.size(), size, file.ToCharArray());
		return false;
	}

	memcpy(data, bytes.data(), size);
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
