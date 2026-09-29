/**
 * awui/Windows/Forms/Joystick/Controller.cpp
 *
 * Copyright (C) 2024 Borja Sánchez Zamorano
 */

#include "Controller.h"

#include <awui/Collections/ArrayList.h>
#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/String.h>

#include <SDL.h>

using namespace awui::Collections;
using namespace awui::Windows::Forms::Joystick;

ArrayList *Controller::m_controllersList = new ArrayList();

Controller::Controller(SDL_GameController *controller) {
	m_controller = controller;
	m_positionOrder = -1;
	m_buttons = 0;
	m_prevButtons = 0;
	m_axisX = 0;
	m_axisY = 0;

	SDL_Joystick *joystick = SDL_GameControllerGetJoystick(controller);
	m_which = SDL_JoystickInstanceID(joystick);
	Console::WriteLine(String("Added Controller: ") + Convert::ToString(m_which));
}

Controller::~Controller() {
	Console::WriteLine(String("Removed Controller: ") + Convert::ToString(m_which));

	SDL_GameControllerClose(m_controller);
}

Controller *Controller::AddOnce(SDL_GameController *gController) {
	Controller *controller;
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

// nullptr si el mando no está en la lista (por ejemplo, un evento que llega justo después de desconectarlo)
Controller *Controller::GetByWhich(SDL_JoystickID which) {
	for (int i = 0; i < m_controllersList->GetCount(); i++) {
		Controller *controller = (Controller *) m_controllersList->Get(i);
		if (controller->m_which == which) {
			return controller;
		}
	}

	return nullptr;
}

void Controller::Refresh() {
	Controller *controller;

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
	for (int i = m_controllersList->GetCount() - 1; i >= 0; i--) {
		Joystick::Controller *controller = (Joystick::Controller *) m_controllersList->Get(i);
		delete controller;
	}

	m_controllersList->Clear();
}

void Controller::OnButtonDown(uint32_t button) {
	uint32_t aux = m_buttons;
	m_buttons |= button;
	if (aux != m_buttons) {
		m_prevButtons = aux;
	}
	// printf("OnButtonDown: %X\n", m_buttons);
}

void Controller::OnButtonUp(uint32_t button) {
	uint32_t aux = m_buttons;
	m_buttons &= ~button;
	if (aux != m_buttons) {
		m_prevButtons = aux;
	}
	// printf("OnButtonUp: %X\n", m_buttons);
}

// Solo el stick izquierdo hace de cruceta. El derecho y los gatillos (ejes 2-5) se ignoran:
// devuelve false si el eje no cuenta, para no avisar a los controles
bool Controller::OnAxisMotion(uint8_t axis, int16_t value) {
	switch (axis) {
		case SDL_CONTROLLER_AXIS_LEFTX:
			m_axisX = value;
			return true;
		case SDL_CONTROLLER_AXIS_LEFTY:
			m_axisY = value;
			return true;
		default:
			return false;
	}
}
