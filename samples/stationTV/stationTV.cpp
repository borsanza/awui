/**
 * samples/stationTV/stationTV.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formArcade.h"

#include <awui/Emulation/MasterSystem/Motherboard.h>
#include <awui/Windows/Emulators/MasterSystem.h>
#include <awui/Windows/Forms/Application.h>
#include <awui/Scripting/JavaScript/JavaScript.h>

using namespace awui;
using namespace awui::Windows::Emulators;
using namespace awui::Scripting::JavaScript;

int main(int argc, char ** argv) {
	JavaScript * js = new JavaScript();
	js->Run("ls(1, 2, 3); cd(1); add(1); cp(1); rm(1); mv(1); set(1); unset(1); get(1); cat(1); backup(1);");
	return 0;

	if (argc == 3) {
		String name = argv[1];
		if (name == "--testsms") {
			MasterSystem * ms = new MasterSystem();
			ms->LoadRom(argv[2]);

			while (!ms->GetCPU()->IsEndlessLoop())
				ms->RunOpcode();

			//ms->GetCPU()->PrintLog();
			return 0;
		}
	}

	FormArcade * form = new FormArcade();

//	for (int i = 1; i < argc; i++)
//		form->LoadRom(argv[i]);

	Application::Run(form);

	return 0;
}
