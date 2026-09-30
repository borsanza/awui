// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "formSlider.h"

#include <awui/UI/Application.h>

using namespace awui::UI;

int main(int argc, char **argv) {
	FormSlider *form = new FormSlider();

	Application::Run(form);

	delete form;

	return 0;
}