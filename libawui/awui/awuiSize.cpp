// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiSize.h"

int awuiSize::GetWidth() {
	return this->width;
}

void awuiSize::SetWidth(int width) {
	if (width < 0)
		width = 0;

	this->width = width;
}

int awuiSize::GetHeight() {
	return this->height;
}

void awuiSize::SetHeight(int height) {
	if (height < 0)
		height = 0;

	this->height = height;
}

awuiSize & awuiSize::operator= (const awuiSize & other) {
	width = other.width;
	height = other.height;

	return *this;
}