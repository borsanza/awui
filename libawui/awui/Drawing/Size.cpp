// (c) Copyright 2011 Borja Sanchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Size.h"

#include <awui/Convert.h>
#include <awui/Drawing/Point.h>
#include <awui/String.h>

using namespace awui::Drawing;

Size::Size() {
	this->width = 0;
	this->height = 0;
	this->SetWidth(0);
	this->SetHeight(0);
}

Size::Size(const Point pt) {
	this->width = 0;
	this->height = 0;
	this->SetWidth(pt.GetX());
	this->SetHeight(pt.GetY());
}

Size::Size(int width, int height) {
	this->width = 0;
	this->height = 0;
	this->SetWidth(width);
	this->SetHeight(height);
}

Size::~Size() {
}

int Size::GetWidth() const {
	return this->width;
}

void Size::SetWidth(int width) {
	if (width < 0)
		width = 0;

	this->width = width;
}

int Size::GetHeight() const {
	return this->height;
}

void Size::SetHeight(int height) {
	if (height < 0)
		height = 0;

	this->height = height;
}

Size & Size::operator= (const Size & other) {
	this->width = other.width;
	this->height = other.height;

	return *this;
}

awui::String Size::ToString() {
	String value;
	value = String("{Width=") + Convert::ToString(this->width) + ", Height=" + Convert::ToString(this->height) + "}";
	return value;
}
