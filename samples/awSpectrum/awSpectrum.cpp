/*
 * samples/awSpectrum/awSpectrum.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formSpectrum.h"

#include <awui/Windows/Forms/Application.h>

using namespace awui::Windows::Forms;

int main(int argc, char ** argv) {
	FormSpectrum * form = new FormSpectrum();
	for (int i = 1; i< argc; i++)
		form->LoadRom(argv[i]);

	Application::Run(form);

	return 0;
}
