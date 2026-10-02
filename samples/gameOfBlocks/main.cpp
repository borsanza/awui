/**
 * samples/stationTV/stationTV.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formGOB.h"

#include <awui/UI/Application.h>

using namespace awui;

int main(int argc, char **argv) {
	// El motor 3D pinta aún con el modo inmediato de OpenGL, que solo existe en el contexto de compatibilidad
	UI::Application::SetOpenGLProfile(UI::OpenGLProfile::Compatibility);

	FormGOB *form = new FormGOB();

	Application::Run(form);

	return 0;
}
