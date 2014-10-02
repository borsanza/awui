/*
 * samples/awMasterGear/awMasterGear.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "form1.h"

#include <awui/Windows/Forms/Application.h>

#include <awui/Console.h>
#include <awui/IO/Directory.h>

using namespace awui::IO;
using namespace awui::Windows::Forms;

int main(int argc, char ** argv) {
	Form1 *form = new Form1();
	Console::WriteLine(Directory::GetCurrentDirectory());
	form->LoadRom(argv[1]);
//	form->LoadRom("./tests/64 Color Palette Test Program by Charles MacDonald V1.00 (PD).sms");

	Application::Run(form);

	return 0;
}