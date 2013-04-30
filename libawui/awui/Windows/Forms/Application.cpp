// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Application.h"

#include <awui/Windows/Forms/Form.h>
#include <SDL.h>
#include <SDL_opengl.h>

using namespace awui::Windows::Forms;

int Application::quit = 0;

int Application::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Application)
		return 1;

	return Object::IsClass(objectClass);
}

void Application::Quit() {
	Application::quit = 1;
}

void Application::Run(Form * form = NULL) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
		return;

	form->Init();

	atexit(SDL_Quit);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);

	while (!Application::quit) {
		form->ProcessEvents();

		form->OnTickPre();
		form->OnPaintForm();
		glFinish();
		SDL_GL_SwapBuffers();
	}

	SDL_Quit();
}
