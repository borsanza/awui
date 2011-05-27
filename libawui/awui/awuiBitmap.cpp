// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiBitmap.h"
#include <stdlib.h>
#include <cairo.h>

awuiBitmap::awuiBitmap(int width, int height) {
	this->width = width;
	this->height = height;
	this->btpp = 4;
	this->image = (unsigned char *) calloc (this->btpp * this->width * this->height, 1);
	this->cairo_surface = cairo_image_surface_create_for_data(this->image, CAIRO_FORMAT_ARGB32, this->width, this->height, this->btpp * this->width);
	this->cr = cairo_create(this->cairo_surface);
}

awuiBitmap::~awuiBitmap() {
}

int awuiBitmap::IsClass(awuiObject::awuiClasses objectClass) const {
	if (objectClass == awuiObject::Bitmap)
		return 1;

	return awuiImage::IsClass(objectClass);
}