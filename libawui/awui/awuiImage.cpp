// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiImage.h"
#include <stdlib.h>
#include <cairo.h>

awuiImage::awuiImage(int width, int height) {
	this->width = width;
	this->height = height;
	this->btpp = 4;
	this->image = (unsigned char *) calloc (this->btpp * this->width * this->height, 1);
	this->cairo_surface = cairo_image_surface_create_for_data(this->image, CAIRO_FORMAT_ARGB32, this->width, this->height, this->btpp * this->width);
	this->cr = cairo_create(this->cairo_surface);
}

awuiImage::awuiImage(const char * filename) {
	this->image = NULL;
	this->btpp = 4;
	this->cairo_surface = cairo_image_surface_create_from_png(filename);
	this->width = cairo_image_surface_get_width(this->cairo_surface);
	this->height = cairo_image_surface_get_height(this->cairo_surface);
	this->cr = cairo_create(this->cairo_surface);
}

awuiImage::~awuiImage() {
	if (this->image != NULL)
		free(this->image);

	if (this->cr != NULL)
		cairo_destroy(this->cr);

	if (this->cairo_surface != NULL)
		cairo_surface_destroy(this->cairo_surface);
}

int awuiImage::IsClass(awuiObject::awuiClasses objectClass) const {
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
