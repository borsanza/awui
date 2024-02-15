/**
 * awui/Windows/Forms/Joystick/Controller.cpp
 *
 * Copyright (C) 2024 Borja Sánchez Zamorano
 */

#include "Controller.h"

#include <awui/Collections/ArrayList.h>

#include <SDL.h>

using namespace awui::Windows::Forms::Joystick;

Controller::Controller(SDL_GameController * controller) {
	m_controller = controller;
	m_positionOrder = -1;
}

Controller::~Controller() {
	SDL_GameControllerClose(m_controller);
}

Controller *Controller::AddOnce(SDL_GameController *gController) {
	Controller * controller;
	for (int i = 0; i < m_controllersList->GetCount(); i++) {
		controller = (Controller *) m_controllersList->Get(i);
		if (controller->m_controller == gController) {
			return controller;
		}
	}

	controller = new Controller(gController);
	m_controllersList->Add(controller);

	return controller;
}

void Controller::Refresh() {
	Controller * controller;

	for (int i = 0; i < m_controllersList->GetCount(); i++) {
		controller = (Controller *) m_controllersList->Get(i);
		controller->SetOrder(-1);
	}

	int pos = 0;
	for (int i = 0; i < SDL_NumJoysticks(); i++) {
		if (SDL_IsGameController(i)) {
			controller = AddOnce(SDL_GameControllerOpen(i));
			controller->SetOrder(pos);
			pos++;
		}
	}

	for (int i = m_controllersList->GetCount() - 1; i >= 0; i--) {
		controller = (Controller *) m_controllersList->Get(i);
		if (controller->GetOrder() == -1) {
			delete controller;
			m_controllersList->RemoveAt(i);
		}
	}
}

void Controller::CloseAll() {
	for (int i = 0; i < m_controllersList->GetCount(); i++) {
		Joystick::Controller * controller = (Joystick::Controller *) m_controllersList->Get(i);
		delete controller;
	}
	
	m_controllersList->Clear();
}
