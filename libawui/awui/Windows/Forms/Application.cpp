/**
 * awui/Windows/Forms/Application.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Application.h"

#include <awui/Console.h>
#include <awui/Convert.h>
#include <awui/Stopwatch.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Joystick/Controller.h>
#include <awui/Windows/Forms/Statistics/Stats.h>
#include <awui/Math.h>
#include <SDL.h>
#include <SDL_opengl.h>

using namespace awui;
using namespace awui::Windows::Forms;
using namespace awui::Windows::Forms::Statistics;

int Application::quit = 0;

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
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
		SDL_Log("[ERROR] SDL_Init failed: %s", SDL_GetError());
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// No se ve si no pongo modo de compatiblidad
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	Joystick::Controller::Refresh();

	form->Init();

	atexit(SDL_Quit);

	Stats * stats = Stats::Instance();

	Stopwatch stopwatch;

	stopwatch.StartNew();

	// Lo inicializo en una frecuencia de 60Hz
	float lastDeltaSeconds = 1.0f/60.0f;
	while (!Application::quit) {
		ProcessEvents();

		stopwatch.Stop();
		float deltaSeconds = stopwatch.GetDeltaSeconds();
		stopwatch.StartNew();

		// Se comporta mejor en fullscreen si amortiguo el deltaseconds
		lastDeltaSeconds = Math::Interpolate(lastDeltaSeconds, deltaSeconds, 0.2, false);

		form->OnTickPre(lastDeltaSeconds);

		glViewport(0, 0, form->GetWidth(), form->GetHeight());
		glClearColor(form->GetBackColor().GetR() / 255.0f, form->GetBackColor().GetG() / 255.0f, form->GetBackColor().GetB() / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		form->OnPaintForm();

		stats->SetTimeBeforeIddle();
		form->SwapGL();
		stats->SetTimeAfterIddle();
	}

	Joystick::Controller::CloseAll();

	SDL_Quit();
}

void Application::ProcessEvents() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		bool ret = false;
		// Console::WriteLine(String("Event [") + Convert::ToString((int)event.type) + "]");
		for (int i = 0; i < Form::m_formsList->GetCount(); i++) {
			Form * formW = (Form *)Form::m_formsList->Get(i);
			switch(event.type) {
				case SDL_JOYDEVICEADDED:
				case SDL_JOYDEVICEREMOVED:
					ret = true;
					break;
				case SDL_CONTROLLERDEVICEADDED:
				case SDL_CONTROLLERDEVICEREMOVED:
					Joystick::Controller::Refresh();
					ret = true;
					break;
				case SDL_CONTROLLERAXISMOTION: {
					// Console::WriteLine(String("SDL_CONTROLLERAXISMOTION [") + Convert::ToString((int)event.caxis.axis) + " - " + Convert::ToString((int)event.caxis.value) + "]");
					Joystick::Controller * controller = Joystick::Controller::GetByWhich(event.cbutton.which);
					if (controller) {
						controller->OnAxisMotion(event.caxis.axis, event.caxis.value);
						formW->OnJoystickAxisMotionPre(controller->GetOrder(), controller->GetAxisX(), controller->GetAxisY());
						ret = true;
					}
					break;
				}
				case SDL_CONTROLLERBUTTONDOWN: {
					Joystick::Controller * controller = Joystick::Controller::GetByWhich(event.cbutton.which);
					if (controller) {
						controller->OnButtonDown(2 << event.cbutton.button);
						formW->OnJoystickButtonDownPre(controller->GetOrder(), 2 << event.cbutton.button, controller->GetButtons(), controller->GetPrevButtons());
						ret = true;
					}
					break;
				}
				case SDL_CONTROLLERBUTTONUP: {
					Joystick::Controller * controller = Joystick::Controller::GetByWhich(event.cbutton.which);
					if (controller) {
						controller->OnButtonUp(2 << event.cbutton.button);
						formW->OnJoystickButtonUpPre(controller->GetOrder(), 2 << event.cbutton.button, controller->GetButtons(), controller->GetPrevButtons());
						ret = true;
					}
					break;
				}
				// TODO: Mostrar la bateria del Joystick
				case SDL_JOYBATTERYUPDATED:
					ret = true;
					break;
				// Ya las he implementado como Controllers
				case SDL_JOYHATMOTION:
				case SDL_JOYAXISMOTION:
				case SDL_JOYBUTTONDOWN:
				case SDL_JOYBUTTONUP:
					ret = true;
					break;
			}
		}

		if (ret) {
			continue;
		}

		Uint32 windowID = 0;
		switch(event.type) {
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				windowID = event.key.windowID;
				break;
			case SDL_MOUSEWHEEL:
				windowID = event.wheel.windowID;
				break;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				windowID = event.button.windowID;
				break;
			case SDL_MOUSEMOTION:
				windowID = event.motion.windowID;
				break;
			case SDL_WINDOWEVENT:
				windowID = event.window.windowID;
				break;
		}

		if (windowID != 0) {
			for (int i = 0; i < Form::m_formsList->GetCount(); i++) {
				Form * formW = (Form *)Form::m_formsList->Get(i);
				if (windowID == formW->GetWindowID()) {
					formW->ProcessEvents(&event);
				}
			}

			continue;
		}

		switch(event.type) {
			case SDL_QUIT:
				Application::Quit();
				break;

			default:
				// Console::WriteLine("Event [%d]", event.type);
				break;
		}
	}
}
