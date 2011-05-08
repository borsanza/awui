// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiImage.h"
#include <stdlib.h>

awuiImage::awuiImage()
{
	this->image = NULL;
	this->width = 0;
	this->height = 0;
	this->btpp = 4;
}

awuiImage::~awuiImage()
{
	if (this->image != NULL)
		free(this->image);
}

int awuiImage::GetWidth() {
	return this->width;
}

int awuiImage::GetHeight() {
	return this->height;
}