// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Console.h"

#include <awui/String.h>

#include <iostream>

using namespace awui;

void awui::Console::outClass::WriteLine() {
	std::cout << std::endl;
}

void awui::Console::outClass::WriteLine(String value) {
	std::cout << std::endl;
}

IO::TextWriter * awui::Console::GetOut() {
	static IO::TextWriter *out = new outClass();

	return out;
}
