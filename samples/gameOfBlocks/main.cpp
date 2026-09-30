/**
 * samples/stationTV/stationTV.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formGOB.h"

#include <awui/UI/Application.h>

using namespace awui;

int main(int argc, char **argv) {
	FormGOB *form = new FormGOB();

	Application::Run(form);

	return 0;
}
