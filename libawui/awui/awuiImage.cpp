// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiImage.h"
#include <stdlib.h>

awuiImage::awuiImage()
{
	this->image = 0;
	this->width = 0;
	this->height = 0;
	this->btpp = 4;
}

awuiImage::~awuiImage()
{
	if (this->image != 0) {
		free(this->image);
		this->image = 0;
	}
}

int awuiImage::GetWidth() {
	return this->width;
}

int awuiImage::GetHeight() {
	return this->height;
}