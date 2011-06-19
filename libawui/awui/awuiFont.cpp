// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiFont.h"

awuiFont::awuiFont(const std::string& font, float size) {
	this->font.assign(font);
	this->size = size;
	this->bold = false;
}

bool awuiFont::GetBold() {
	return this->bold;
}

float awuiFont::GetSize() {
	return this->size;
}

const std::string& awuiFont::GetFont() {
	return this->font;
}
