/**
 * awui/Drawing/Image.cpp
 *
 * Copyright (C) 2011 Borja Sánchez Zamorano
 */

#include "Image.h"

#include <awui/String.h>
#include <cairo.h>
#include <stdlib.h>
#include <SDL_opengl.h>

#define BTPP 4

using namespace awui::Drawing;

ArrayList Image::list;

Image::Image(int width, int height) {
	this->texture = 0;
	this->width = width;
	this->height = height;
	this->image = (unsigned char *) calloc(BTPP, this->width * this->height);
	this->cairo_surface = cairo_image_surface_create_for_data(this->image, CAIRO_FORMAT_ARGB32, this->width, this->height, BTPP * this->width);
	this->cr = cairo_create(this->cairo_surface);
	this->loaded = false;

	Image::list.Add(this);
}

Image::Image(String filename) {
	this->image = NULL;
	this->texture = 0;
	this->cairo_surface = cairo_image_surface_create_from_png(filename.ToCharArray());
	this->width = cairo_image_surface_get_width(this->cairo_surface);
	this->height = cairo_image_surface_get_height(this->cairo_surface);
	this->cr = cairo_create(this->cairo_surface);
	this->loaded = false;

	Image::list.Add(this);
}

Image::~Image() {
	Image::list.Remove(this);

	if (this->image != NULL)
		free(this->image);

	if (this->cr != NULL)
		cairo_destroy(this->cr);

	if (this->cairo_surface != NULL)
		cairo_surface_destroy(this->cairo_surface);

	this->Unload();
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

	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->GetWidth(), this->GetHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, this->image);

	this->loaded = true;
}

void Image::Unload() {
	if (!this->loaded)
		return;

	glDeleteTextures(1, &this->texture);
	this->texture = 0;
	this->loaded = false;
}

void Image::UnloadAll() {
	for (int i = 0; i < Image::list.GetCount(); i++) {
		Image * image = (Image *)Image::list.Get(i);
		image->Unload();
	}
}

void Image::Update() {
	if (this->loaded) {
		glBindTexture(GL_TEXTURE_2D, this->texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, this->GetWidth(), this->GetHeight(), GL_BGRA, GL_UNSIGNED_BYTE, this->image);
	}
}

GLuint Image::GetTexture() {
	return this->texture;
}

void Image::SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	int offset = ((y * this->width) + x) * BTPP;
	this->image[offset    ] = b;
	this->image[offset + 1] = g;
	this->image[offset + 2] = r;
	this->image[offset + 3] = a;
}

void Image::Clear() {
	for (int x = 0; x < this->width; x++)
		for (int y = 0; y < this->height; y++)
			this->SetPixel(x, y, 0, 0, 0);
}
