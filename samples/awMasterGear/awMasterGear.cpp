/*
 * samples/awMasterGear/awMasterGear.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formMasterGear.h"

#include <awui/Windows/Forms/Application.h>

#include <awui/Console.h>
#include <awui/IO/Directory.h>

using namespace awui::IO;
using namespace awui::Windows::Forms;

int main(int argc, char ** argv) {
	FormMasterGear *form = new FormMasterGear();
	Console::WriteLine(Directory::GetCurrentDirectory());
//	form->LoadRom(argv[1]);
//	form->LoadRom("./tests/64 Color Palette Test Program by Charles MacDonald V1.00 (PD).sms");
	form->LoadRom("./tests/1.sms");

	Application::Run(form);

	return 0;
}
