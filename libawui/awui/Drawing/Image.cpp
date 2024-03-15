/**
 * awui/Drawing/Image.cpp
 *
 * Copyright (C) 2011 Borja Sánchez Zamorano
 */

#include "Image.h"

#include <SDL_opengl.h>
#include <awui/String.h>
#include <cairo.h>
#include <stdlib.h>

#define BTPP 4

using namespace awui::Drawing;

ArrayList Image::list;

Image::Image(int width, int height) {
	m_class = Classes::Image;
	m_texture = 0;
	m_width = width;
	m_height = height;
	m_image = (unsigned char *) calloc(BTPP, m_width * m_height);
	m_cairo_surface = cairo_image_surface_create_for_data(m_image, CAIRO_FORMAT_ARGB32, m_width, m_height, BTPP * m_width);
	m_cr = cairo_create(m_cairo_surface);
	m_loaded = false;

	Image::list.Add(this);
}

Image::Image(String filename) {
	m_class = Classes::Image;
	m_image = NULL;
	m_texture = 0;
	m_cairo_surface = cairo_image_surface_create_from_png(filename.ToCharArray());
	m_width = cairo_image_surface_get_width(m_cairo_surface);
	m_height = cairo_image_surface_get_height(m_cairo_surface);
	m_cr = cairo_create(m_cairo_surface);
	m_loaded = false;

	Image::list.Add(this);
}

Image::~Image() {
	Image::list.Remove(this);

	if (m_image != NULL)
		free(m_image);

	if (m_cr != NULL)
		cairo_destroy(m_cr);

	if (m_cairo_surface != NULL)
		cairo_surface_destroy(m_cairo_surface);

	Unload();
}

bool Image::IsClass(Classes objectClass) const {
	return (objectClass == Classes::Image) || Object::IsClass(objectClass);
}

int Image::GetWidth() {
	return m_width;
}

int Image::GetHeight() {
	return m_height;
}

void Image::Load() {
	if (m_loaded)
		return;

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GetWidth(), GetHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, m_image);

	m_loaded = true;
}

void Image::Unload() {
	if (!m_loaded)
		return;

	glDeleteTextures(1, &m_texture);
	m_texture = 0;
	m_loaded = false;
}

void Image::UnloadAll() {
	for (int i = 0; i < Image::list.GetCount(); i++) {
		Image *image = (Image *) Image::list.Get(i);
		image->Unload();
	}
}

void Image::Update() {
	if (m_loaded) {
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GetWidth(), GetHeight(), GL_BGRA, GL_UNSIGNED_BYTE, m_image);
	}
}

GLuint Image::GetTexture() {
	return m_texture;
}

void Image::SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	int offset = ((y * m_width) + x) * BTPP;
	m_image[offset] = b;
	m_image[offset + 1] = g;
	m_image[offset + 2] = r;
	m_image[offset + 3] = a;
}

void Image::Clear() {
	for (int x = 0; x < m_width; x++)
		for (int y = 0; y < m_height; y++)
			SetPixel(x, y, 0, 0, 0);
}
