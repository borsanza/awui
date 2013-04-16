// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Font.h"

using namespace awui::Drawing;

Font::Font(const String font, float size) {
	this->font = font;
	this->size = size;
	this->bold = false;
}

bool Font::GetBold() {
	return this->bold;
}

float Font::GetSize() {
	return this->size;
}

const awui::String Font::GetFont() const {
	return this->font;
}
