// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "formTest.h"

#include <awui/Windows/Forms/Application.h>

using namespace awui::Windows::Forms;

int main(int argc, char **argv) {
	FormTest *form = new FormTest();

	Application::Run(form);

	delete form;

	return 0;
}
