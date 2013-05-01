// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Console.h"

#include <awui/String.h>
#include <awui/Environment.h>

#include <iostream>

using namespace awui;

void awui::Console::outClass::Flush() {
	std::cout << std::flush;
}

String awui::Console::outClass::GetNewLine() {
	return Environment::GetNewLine();
}

void awui::Console::outClass::Write(String value) {
	std::cout << value.ToCharArray();
}

void awui::Console::outClass::Write(char value) {
	std::cout << value;
}

IO::TextWriter * awui::Console::GetOut() {
	static IO::TextWriter *out = new outClass();

	return out;
}

void awui::Console::WriteLine(String value) {
	awui::Console::GetOut()->WriteLine(value);
}
