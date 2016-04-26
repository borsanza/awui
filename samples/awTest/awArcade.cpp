// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "formArcade.h"

#include <awui/Windows/Forms/Application.h>

using namespace awui::Windows::Forms;

int main(int argc, char ** argv) {
	FormArcade * form = new FormArcade();

	Application::Run(form);

	delete form;

	return 0;
}
