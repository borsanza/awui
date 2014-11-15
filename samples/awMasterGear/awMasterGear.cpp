/*
 * samples/awMasterGear/awMasterGear.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formMasterGear.h"

#include <awui/Windows/Forms/Application.h>
#include <awui/Windows/Emulators/MasterSystem.h>

#include <awui/Console.h>
#include <awui/IO/Directory.h>

using namespace awui::IO;
using namespace awui::Windows::Forms;

int main(int argc, char ** argv) {
	if (argc == 3) {
		String name = argv[1];
		if (name == "--test") {
			MasterSystem * ms = new MasterSystem();
			ms->LoadRom(argv[2]);
			while (1)
				ms->RunOpcode();
		}
	}

	FormMasterGear *form = new FormMasterGear();
	for (int i = 1; i< argc; i++)
		form->LoadRom(argv[i]);

	Application::Run(form);

	return 0;
}
