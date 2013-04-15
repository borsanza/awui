// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"

#include <awui/Windows/Forms/Application.h>

#include <awui/Console.h>
#include <awui/String.h>

using namespace awui::Windows::Forms;

int main(int argc, char ** argv) {
	String a;
	Console::GetOut()->WriteLine(a);

	Form1 *form = new Form1();

	Application::Run(form);

	return 0;
}
