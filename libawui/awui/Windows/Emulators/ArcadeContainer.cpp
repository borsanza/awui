/**
 * awui/Windows/Emulators/ArcadeContainer.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "ArcadeContainer.h"

#include <awui/Windows/Forms/Station/StationUI.h>

using namespace awui::Drawing;
using namespace awui::Windows::Emulators;

ArcadeContainer::ArcadeContainer() {
	SetBackColor(Color::FromArgb(0, 0, 0));
	SetTabStop(false);
	SetDrawShadow(false);
	SetPreventChangeControl(true);
	m_station = NULL;
}

ArcadeContainer::~ArcadeContainer() {
}

bool ArcadeContainer::IsClass(Classes objectClass) const {
	if (objectClass == Classes::ArcadeContainer) {
		return true;
	}

	return Button::IsClass(objectClass);
}

void ArcadeContainer::SetStationUI(StationUI * station) {
	m_station = station;
}

bool ArcadeContainer::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	if (button & RemoteButtons::Menu)
		m_station->ExitingArcade();

	return Button::OnRemoteKeyUp(which, button);
}
