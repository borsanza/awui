/**
 * samples/awArcade/awArcade.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formArcade.h"

#include <awui/Collections/ArrayList.h>
#include <awui/IO/Directory.h>
#include <awui/Emulation/MasterSystem/Motherboard.h>
#include <awui/Windows/Emulators/MasterSystem.h>
#include <awui/Windows/Forms/Application.h>
#include <awui/Windows/Station/SearchFiles.h>

using namespace awui;
using namespace awui::Windows::Station;

/**
 * Chip8: *.ch8, *.c8x
 * Master System: *.sms, *.sg
 * Spectrum: *.rom, *.tap
 */

int main(int argc, char ** argv) {
	if (argc == 3) {
		String name = argv[1];
		if (name == "--testsms") {
			MasterSystem * ms = new MasterSystem();
			ms->LoadRom(argv[2]);
/*
			for (int i = 0; i<137476075; i++) {
				ms->GetCPU()->ADDXXpp(Reg_HL, 15, 11, 0);
			}
			return 0;
*/

			while (!ms->GetCPU()->IsEndlessLoop())
				ms->RunOpcode();

			//ms->GetCPU()->PrintLog();

			return 0;
		}
	}

	SearchFiles search;
	search.SetPath("./roms/");
	search.Refresh();
	ArrayList list;
	search.GetList(&list, 0);
	// search.Clear();

	FormArcade * form = new FormArcade();
	for (int i = 1; i < list.GetCount(); i++) {
		String * name = (String *)list.Get(i);
		form->LoadRom(*name);
	}

//	for (int i = 1; i < argc; i++)
//		form->LoadRom(argv[i]);

	Application::Run(form);

	return 0;
}
