/**
 * awui/Windows/Emulators/ArcadeContainer.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "ArcadeContainer.h"

#include <awui/Console.h>
#include <awui/Windows/Forms/Station/StationUI.h>

#include <stdio.h>

using namespace awui::Drawing;
using namespace awui::Windows::Emulators;

ArcadeContainer::ArcadeContainer() {
	SetBackColor(Color::FromArgb(0, 0, 0));
	SetDrawShadow(false);
	SetPreventChangeControl(true);
	m_station = NULL;
	SetFocusable(false);
}

bool ArcadeContainer::WriteStateFile(const String &file, const uint8_t *data, int size) {
	// Se escribe en un temporal y se renombra: si se corta a medias, el estado anterior sigue entero
	String tmp = String::Concat(file, ".tmp");
	FILE *f = fopen(tmp.ToCharArray(), "wb");
	if (!f) {
		Console::Error->WriteLine(String("No se puede guardar el estado: ") + file);
		return false;
	}

	bool ok = fwrite(data, 1, size, f) == (size_t) size;
	ok = (fclose(f) == 0) && ok;
	if (!ok || (rename(tmp.ToCharArray(), file.ToCharArray()) != 0)) {
		remove(tmp.ToCharArray());
		Console::Error->WriteLine(String("No se puede guardar el estado: ") + file);
		return false;
	}

	return true;
}

bool ArcadeContainer::ReadStateFile(const String &file, uint8_t *data, int size) {
	FILE *f = fopen(file.ToCharArray(), "rb");
	if (!f)
		return false;

	// Un estado de otro tamaño es de otra versión del emulador: no se carga
	fseek(f, 0, SEEK_END);
	long length = ftell(f);
	fseek(f, 0, SEEK_SET);
	bool ok = (length == size) && (fread(data, 1, size, f) == (size_t) size);
	fclose(f);

	if (!ok)
		Console::Error->WriteLine(String("Estado incompatible, no se carga: ") + file);

	return ok;
}

void ArcadeContainer::SetStationUI(StationUI *station) {
	m_station = station;
}

bool ArcadeContainer::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	if (button & RemoteButtons::Menu)
		m_station->ExitingArcade();

	return Button::OnRemoteKeyUp(which, button);
}
