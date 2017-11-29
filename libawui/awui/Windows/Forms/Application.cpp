/**
 * awui/Windows/Forms/Application.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Application.h"

#include <awui/Drawing/Shader.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Statistics/Stats.h>
#include <SDL.h>
#include <SDL_opengl.h>

using namespace awui::Windows::Forms;
using namespace awui::Windows::Forms::Statistics;

int Application::quit = 0;
SDL_Joystick * Application::stick1 = NULL;
SDL_Joystick * Application::stick2 = NULL;

int Application::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Application)
		return 1;

	return Object::IsClass(objectClass);
}

void Application::Quit() {
	Application::quit = 1;
}

void Application::Run(Form * form = NULL) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
		return;

	if (SDL_NumJoysticks() > 0)
		Application::stick1 = SDL_JoystickOpen(0);

	if (SDL_NumJoysticks() > 1)
		Application::stick2 = SDL_JoystickOpen(1);

	form->Init();

	atexit(SDL_Quit);

	Stats * stats = Stats::Instance();

	// awui::Drawing::Shader * shader = new awui::Drawing::Shader();

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
