// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Font.h"

#include <awui/Convert.h>

using namespace awui::Drawing;

Font::Font(const String font, float size) {
	m_class = Classes::Font;
	m_font = font;
	m_size = size;
	m_style = FontStyle::Regular;
}

Font::Font(const String font, float size, int style) {
	m_class = Classes::Font;
	m_font = font;
	m_size = size;
	m_style = style;
}

bool Font::IsClass(Classes objectClass) const {
    return (objectClass == Classes::Font) || Object::IsClass(objectClass);
}

bool Font::GetBold() {
	return m_style & FontStyle::Bold;
}

bool Font::GetItalic() {
	return m_style & FontStyle::Italic;
}

bool Font::GetStrikeout() {
	return m_style & FontStyle::Strikeout;
}

bool Font::GetUnderline() {
	return m_style & FontStyle::Underline;
}

float Font::GetSize() {
	return m_size;
}

const awui::String Font::GetFont() const {
	return m_font;
}

awui::String Font::ToString() const {
	String value;
	value = String("[Font: Name=") + m_font + ", Size=" + Convert::ToString(m_size) + "]";
	return value;
}

Font & Font::operator= (const Font & other) {
	m_font = other.m_font;
	m_size = other.m_size;
	m_style = other.m_style;

	return *this;
}
