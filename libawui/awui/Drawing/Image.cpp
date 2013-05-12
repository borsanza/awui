// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Image.h"

#include <awui/String.h>
#include <cairo.h>
#include <SDL_opengl.h>

using namespace awui::Drawing;

Image::Image(int width, int height) {
	this->width = width;
	this->height = height;
	this->btpp = 4;
	this->image = (unsigned char *) calloc (this->btpp * this->width * this->height, 1);
	this->cairo_surface = cairo_image_surface_create_for_data(this->image, CAIRO_FORMAT_ARGB32, this->width, this->height, this->btpp * this->width);
	this->cr = cairo_create(this->cairo_surface);
	this->loaded = false;
}

Image::Image(String filename) {
	this->image = NULL;
	this->btpp = 4;
	this->cairo_surface = cairo_image_surface_create_from_png(filename.ToCharArray());
	this->width = cairo_image_surface_get_width(this->cairo_surface);
	this->height = cairo_image_surface_get_height(this->cairo_surface);
	this->cr = cairo_create(this->cairo_surface);
	this->loaded = false;
}

Image::~Image() {
	if (this->image != NULL)
		free(this->image);

	if (this->cr != NULL)
		cairo_destroy(this->cr);

	if (this->cairo_surface != NULL)
		cairo_surface_destroy(this->cairo_surface);

	if (this->loaded)
		glDeleteTextures(1, &this->texture);
}

int Image::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Image)
		return 1;

	return Object::IsClass(objectClass);
}

int Image::GetWidth() {
	return this->width;
}

int Image::GetHeight() {
	return this->height;
}

void Image::Load() {
	if (this->loaded)
		return;

	this->loaded = true;

	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->GetWidth(), this->GetHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, this->image);
}

GLuint Image::GetTexture() {
	return this->texture;
}
