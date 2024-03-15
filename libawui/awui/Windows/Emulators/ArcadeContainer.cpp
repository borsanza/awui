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
	m_class = Classes::ArcadeContainer;
	SetBackColor(Color::FromArgb(0, 0, 0));
	SetDrawShadow(false);
	SetPreventChangeControl(true);
	m_station = NULL;
}

bool ArcadeContainer::IsClass(Classes objectClass) const {
	return (objectClass == Classes::ArcadeContainer) || Button::IsClass(objectClass);
}

void ArcadeContainer::SetStationUI(StationUI *station) {
	m_station = station;
}

bool ArcadeContainer::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	if (button & RemoteButtons::Menu)
		m_station->ExitingArcade();

	return Button::OnRemoteKeyUp(which, button);
}
