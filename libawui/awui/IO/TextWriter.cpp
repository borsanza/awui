// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "TextWriter.h"

#include <awui/Object.h>
#include <awui/String.h>

using namespace awui::IO;

void TextWriter::Write(Object *value) {
	this->Write(value->ToString());
}

void TextWriter::WriteLine() {
	this->Write(this->GetNewLine());
	this->Flush();
}

void TextWriter::WriteLine(char value) {
	this->Write(value);
	this->WriteLine();
}

void TextWriter::WriteLine(Object *value) {
	this->Write(value);
	this->WriteLine();
}

void TextWriter::WriteLine(String value) {
	this->Write(value);
	this->WriteLine();
}
