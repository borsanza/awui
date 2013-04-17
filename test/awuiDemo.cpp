// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"

#include <awui/Windows/Forms/Application.h>

using namespace awui::Windows::Forms;

#include <awui/Collections/ArrayList.h>
#include <awui/Console.h>

int main(int argc, char ** argv) {
	awui::Drawing::Color a = Drawing::Color::FromArgb(-10,220,330,240);
	Console::GetOut()->WriteLine(&a);
	Console::GetOut()->WriteLine(a.ToString());

	Form1 *form = new Form1();

	Application::Run(form);

	return 0;
}
