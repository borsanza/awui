// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Font.h"

#include <awui/Convert.h>

using namespace awui::Drawing;

Font::Font(const String font, float size) {
	this->font = font;
	this->size = size;
	this->style = FontStyle::Regular;
}

Font::Font(const String font, float size, int style) {
	this->font = font;
	this->size = size;
	this->style = style;
}

bool Font::GetBold() {
	return this->style & FontStyle::Bold;
}

bool Font::GetItalic() {
	return this->style & FontStyle::Italic;
}

bool Font::GetStrikeout() {
	return this->style & FontStyle::Strikeout;
}

bool Font::GetUnderline() {
	return this->style & FontStyle::Underline;
}

float Font::GetSize() {
	return this->size;
}

const awui::String Font::GetFont() const {
	return this->font;
}

awui::String Font::ToString() {
	String value;
	value = String("[Font: Name=") + this->font + ", Size=" + Convert::ToString(this->size) + "]";
	return value;
}
