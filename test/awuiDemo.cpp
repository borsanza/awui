// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"

#include <awui/Windows/Forms/Application.h>

using namespace awui::Windows::Forms;

//#include <awui/Console.h>
//#include <awui/Diagnostics/Process.h>

int main(int argc, char ** argv) {
/*
	awui::Diagnostics::Process proceso;
	proceso.Start("mame", "-listfull");
	while (!proceso.GetHasExited()) {
		if (proceso.GetHasString())
			Console::GetOut()->WriteLine(proceso.GetLine());
	}
*/
	Form1 *form = new Form1();

	Application::Run(form);

	return 0;
}
