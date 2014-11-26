/*
 * samples/awMasterGear/awMasterGear.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formMasterGear.h"

#include <awui/Emulation/MasterSystem/CPU.h>
#include <awui/Emulation/MasterSystem/Registers.h>
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
/*
			for (int i = 0; i<508000000; i++) {
				ms->GetCPU()->CP(10, 10, 0);
			}
			return 0;
*/
			while (!ms->GetCPU()->IsHalted())
				ms->RunOpcode();

			ms->GetCPU()->PrintLog();

			return 0;
		}
	}

	FormMasterGear *form = new FormMasterGear();
	for (int i = 1; i< argc; i++)
		form->LoadRom(argv[i]);

	Application::Run(form);

	return 0;
}
