/**
 * awui/IO/TextWriter.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "TextWriter.h"

#include <awui/Object.h>
#include <awui/String.h>

using namespace awui::IO;

TextWriter::~TextWriter() {
}

void TextWriter::Write(Object *value) {
	const char * str = value->ToString().ToCharArray();
	Write(str);
}

void awui::IO::TextWriter::Write(String value) {
	const char * str = value.ToCharArray();
	Write(str);
}

void TextWriter::WriteLine() {
	Write(GetNewLine());
	Flush();
}

void TextWriter::WriteLine(Object *value) {
	const char * str = value->ToString().ToCharArray();
	Write(str);
	WriteLine();
}

void TextWriter::WriteLine(String value) {
	const char * str = value.ToCharArray();
	Write(str);
	WriteLine();
}

void awui::IO::TextWriter::WriteLine(const char *str, ...) {
	va_list args;
	va_start(args, str);
	Write(str, args);
	va_end(args);
	WriteLine();
}

void awui::IO::TextWriter::Write(const char *str, ...) {
	va_list args;
	va_start(args, str);
	Write(str, args);
	va_end(args);
}

void awui::IO::TextWriter::WriteLine(const char *value, va_list args) {
	Write(value, args);
	WriteLine();
}
