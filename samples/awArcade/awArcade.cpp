// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"

#include <awui/Windows/Forms/Application.h>

using namespace awui::Windows::Forms;

int main(int argc, char ** argv) {
	Form1 * form = new Form1();

	Application::Run(form);

	delete form;

	return 0;
}
