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
	this->SetBackColor(Color::FromArgb(0, 0, 0));
	this->SetTabStop(false);
	this->SetDrawShadow(false);
	this->SetPreventChangeControl(true);
	this->_station = NULL;
}

ArcadeContainer::~ArcadeContainer() {
}

void ArcadeContainer::SetStationUI(StationUI * station) {
	this->_station = station;
}

bool ArcadeContainer::OnRemoteKeyUp(int which, RemoteButtons::Enum button) {
	if (button & RemoteButtons::Menu)
		this->_station->ExitingArcade();

	return Button::OnRemoteKeyUp(which, button);
}
