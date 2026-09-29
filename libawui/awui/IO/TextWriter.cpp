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

// El texto se escribe tal cual ("%s"): si se pasara como formato, un '%' en él leería argumentos que no existen.
// El String se guarda en una variable para que el puntero de ToCharArray() siga siendo válido al escribir
void TextWriter::Write(Object *value) {
	String text = value->ToString();
	Write("%s", text.ToCharArray());
}

void awui::IO::TextWriter::Write(String value) {
	Write("%s", value.ToCharArray());
}

void TextWriter::WriteLine() {
	Write(GetNewLine());
	Flush();
}

void TextWriter::WriteLine(Object *value) {
	Write(value);
	WriteLine();
}

void TextWriter::WriteLine(String value) {
	Write(value);
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
