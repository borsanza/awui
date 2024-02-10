// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Console.h"

#include <awui/Environment.h>

#include <iostream>

using namespace awui;

IO::TextWriter* awui::Console::Error = new awui::Console::ErrorClass();
IO::TextWriter* awui::Console::Out = new awui::Console::OutClass();

void awui::Console::OutClass::Flush() {
	std::cout << std::flush;
}

String awui::Console::OutClass::GetNewLine() {
	return Environment::GetNewLine();
}

void awui::Console::OutClass::Write(String value) {
	std::cout << value.ToCharArray();
}

void awui::Console::OutClass::Write(char value) {
	std::cout << value;
}

void awui::Console::ErrorClass::Flush() {
	std::cerr << std::flush;
}

String awui::Console::ErrorClass::GetNewLine() {
	return Environment::GetNewLine();
}

void awui::Console::ErrorClass::Write(String value) {
	std::cerr << value.ToCharArray();
}

void awui::Console::ErrorClass::Write(char value) {
	std::cerr << value;
}

void awui::Console::Write(String value) {
	awui::Console::Out->Write(value);
}

void awui::Console::WriteLine(String value) {
	awui::Console::Out->WriteLine(value);
}

void awui::Console::WriteLine(Object * value) {
	awui::Console::Out->WriteLine(value);
}
