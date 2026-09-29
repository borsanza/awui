// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Font.h"

#include <awui/Convert.h>

using namespace awui::Drawing;

Font::Font(const String font, float size) {
	m_font = font;
	m_size = size;
	m_style = FontStyle::Regular;
}

Font::Font(const String font, float size, int style) {
	m_font = font;
	m_size = size;
	m_style = style;
}

bool Font::GetBold() const {
	return m_style & FontStyle::Bold;
}

bool Font::GetItalic() const {
	return m_style & FontStyle::Italic;
}

bool Font::GetStrikeout() const {
	return m_style & FontStyle::Strikeout;
}

bool Font::GetUnderline() const {
	return m_style & FontStyle::Underline;
}

float Font::GetSize() const {
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

Font &Font::operator=(const Font &other) {
	m_font = other.m_font;
	m_size = other.m_size;
	m_style = other.m_style;

	return *this;
}
