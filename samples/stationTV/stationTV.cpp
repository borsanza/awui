/**
 * samples/stationTV/stationTV.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formArcade.h"

#include <awui/Emulation/MasterSystem/Motherboard.h>
#include <awui/Windows/Emulators/MasterSystem.h>
#include <awui/Windows/Forms/Application.h>

using namespace awui;
using namespace awui::Windows::Emulators;

int main(int argc, char ** argv) {
	if (argc == 3) {
		String name = argv[1];
		if (name == "--testsms") {
			MasterSystem * ms = new MasterSystem();
			ms->LoadRom(argv[2]);

			while (!ms->GetCPU()->IsEndlessLoop()) {
				ms->RunOpcode();
			}

			//ms->GetCPU()->PrintLog();
			return 0;
		}
	}

	FormArcade * form = new FormArcade();

//	for (int i = 1; i < argc; i++) {
//		form->LoadRom(argv[i]);
//	}

	Application::Run(form);

	return 0;
}
