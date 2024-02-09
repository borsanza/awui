/**
 * awui/Windows/Forms/Application.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Application.h"

#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Statistics/Stats.h>
#include <SDL.h>
#include <SDL_opengl.h>

using namespace awui::Windows::Forms;
using namespace awui::Windows::Forms::Statistics;

int Application::quit = 0;
SDL_Joystick * Application::stick1 = NULL;
SDL_Joystick * Application::stick2 = NULL;

bool Application::IsClass(Classes objectClass) const {
	if (objectClass == Classes::Application) {
		return true;
	}

	return Object::IsClass(objectClass);
}

void Application::Quit() {
	Application::quit = 1;
}

void Application::Run(Form * form = NULL) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
		SDL_Log("[ERROR] SDL_Init failed: %s", SDL_GetError());
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// No se ve si no pongo modo de compatiblidad
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	if (SDL_NumJoysticks() > 0)
		Application::stick1 = SDL_JoystickOpen(0);

	if (SDL_NumJoysticks() > 1)
		Application::stick2 = SDL_JoystickOpen(1);

	form->Init();

	atexit(SDL_Quit);

	Stats * stats = Stats::Instance();

	while (!Application::quit) {
		form->ProcessEvents();

		form->OnTickPre();

		glViewport(0, 0, form->GetWidth(), form->GetHeight());
		glClearColor(form->GetBackColor().GetR() / 255.0f, form->GetBackColor().GetG() / 255.0f, form->GetBackColor().GetB() / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		form->OnPaintForm();

		stats->SetTimeBeforeIddle();
		form->SwapGL();
		stats->SetTimeAfterIddle();
	}

	if (Application::stick1)
		SDL_JoystickClose(stick1);

	if (Application::stick2)
		SDL_JoystickClose(stick2);

	SDL_Quit();
}
