/**
 * samples/stationTV/stationTV.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formGOB.h"

#include <awui/Windows/Forms/Application.h>

using namespace awui;

int main(int argc, char **argv) {
	// Application::Test();

	FormGOB *form = new FormGOB();
	Application::Run(form);

	return 0;
}
