/*
 * samples/awTIS100/awTIS100.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "formTIS100.h"

#include <awui/UI/Application.h>

using namespace awui::UI;

int main(int argc, char **argv) {
	FormTIS100 *form = new FormTIS100();

	Application::Run(form);

	return 0;
}
