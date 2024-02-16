// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Console.h"

#include <awui/Environment.h>

#include <cstdarg>
#include <iostream>
#include <cstdio>

using namespace awui;

IO::TextWriter* awui::Console::Error = new awui::Console::ErrorClass();
IO::TextWriter* awui::Console::Out = new awui::Console::OutClass();

void awui::Console::OutClass::Flush() {
	fflush(stdout);
}

String awui::Console::OutClass::GetNewLine() {
	return Environment::GetNewLine();
}

void awui::Console::OutClass::Write(const char * str, va_list args) {
	vfprintf(stdout, str, args);
}

void awui::Console::ErrorClass::Flush() {
	fflush(stderr);
}

String awui::Console::ErrorClass::GetNewLine() {
	return Environment::GetNewLine();
}

void awui::Console::ErrorClass::Write(const char * str, va_list args) {
	vfprintf(stderr, str, args);
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

void awui::Console::Write(const char *str, ...) {
	va_list args;
	va_start(args, str);
	awui::Console::Out->Write(str, args);
	va_end(args);
}

void awui::Console::WriteLine(const char *str, ...) {
	va_list args;
	va_start(args, str);
	awui::Console::Out->WriteLine(str, args);
	va_end(args);
}
