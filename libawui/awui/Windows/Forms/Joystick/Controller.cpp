/**
 * awui/Windows/Forms/Joystick/Controller.cpp
 *
 * Copyright (C) 2024 Borja Sánchez Zamorano
 */

#include "Controller.h";

#include <SDL.h>

Controller::Controller(SDL_GameController * controller) {
    m_controller = controller;
}

Controller::~Controller() {
    SDL_GameControllerClose(m_controller);
}