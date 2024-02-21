// (c) Copyright 2011 Borja Sanchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Size.h"

#include <awui/Convert.h>
#include <awui/Drawing/Point.h>
#include <awui/String.h>

using namespace awui::Drawing;

Size::Size() {
	m_width = 0.0f;
	m_height = 0.0f;
}

Size::Size(const Point pt) {
	m_width = pt.GetX();
	m_height = pt.GetY();
}

Size::Size(float width, float height) {
	m_width = width;
	m_height = height;
}

Size::~Size() {
}

float Size::GetWidth() const {
	return m_width;
}

void Size::SetWidth(float width) {
	m_width = width < 0.0f ? 0.0f : width;
}

float Size::GetHeight() const {
	return m_height;
}

void Size::SetHeight(float height) {
	m_height = (height < 0.0f) ? 0.0f : height;
}

Size & Size::operator= (const Size & other) {
	m_width = other.m_width;
	m_height = other.m_height;

	return *this;
}

awui::String Size::ToString() {
	String value;
	value = String("{Width=") + Convert::ToString(m_width) + ", Height=" + Convert::ToString(m_height) + "}";
	return value;
}
