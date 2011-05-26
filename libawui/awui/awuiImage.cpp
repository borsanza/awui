// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiImage.h"
#include <stdlib.h>
#include <cairo.h>

awuiImage::awuiImage() {
	this->image = NULL;
	this->width = 0;
	this->height = 0;
	this->btpp = 4;
	this->cairo_surface = 0;
	this->cr = 0;
}

awuiImage::~awuiImage() {
	if (this->image != NULL)
		free(this->image);

	if (this->cr != NULL)
		cairo_destroy(this->cr);

	if (this->cairo_surface != NULL)
		cairo_surface_destroy(this->cairo_surface);
}

int awuiImage::IsClass(awuiObject::awuiClasses objectClass) {
	if (objectClass == awuiObject::Image)
		return 1;

	return awuiObject::IsClass(objectClass);
}

int awuiImage::GetWidth() {
	return this->width;
}

int awuiImage::GetHeight() {
	return this->height;
}