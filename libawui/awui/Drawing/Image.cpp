/**
 * awui/Drawing/Image.cpp
 *
 * Copyright (C) 2011 Borja Sánchez Zamorano
 */

#include "Image.h"

#include <SDL_opengl.h>
#include <awui/Console.h>
#include <awui/String.h>
#include <cairo.h>
#include <stdlib.h>
#include <algorithm>

#define BTPP 4

using namespace awui::Drawing;

std::vector<Image *> &Image::List() {
	static std::vector<Image *> *list = new std::vector<Image *>();
	return *list;
}

Image::Image(int width, int height) {
	Create(width, height);
}

// El PNG se pinta en el buffer propio: así se comporta como cualquier otra imagen (antes la textura se subía desde
// un buffer nulo y quedaba vacía) y un PNG sin transparencia, que cairo carga como RGB24 con el cuarto byte sin
// definir, queda opaco. Si no se puede leer, queda una imagen transparente de 1x1
Image::Image(String filename) {
	cairo_surface_t *png = cairo_image_surface_create_from_png(filename.ToCharArray());
	bool ok = cairo_surface_status(png) == CAIRO_STATUS_SUCCESS;
	if (!ok) {
		Console::Error->WriteLine("No se puede cargar la imagen: %s (%s)", filename.ToCharArray(), cairo_status_to_string(cairo_surface_status(png)));
	}

	Create(ok ? cairo_image_surface_get_width(png) : 1, ok ? cairo_image_surface_get_height(png) : 1);

	if (ok) {
		cairo_set_source_surface(m_cr, png, 0, 0);
		cairo_set_operator(m_cr, CAIRO_OPERATOR_SOURCE);
		cairo_paint(m_cr);
		cairo_set_operator(m_cr, CAIRO_OPERATOR_OVER);
	}

	cairo_surface_destroy(png);
}

Image::~Image() {
	List().erase(std::remove(List().begin(), List().end(), this), List().end());

	// Primero cairo (el contexto y la superficie usan el buffer) y después el buffer
	if (m_cr != NULL)
		cairo_destroy(m_cr);

	if (m_cairo_surface != NULL)
		cairo_surface_destroy(m_cairo_surface);

	if (m_image != NULL)
		free(m_image);

	Unload();
}

// Toda imagen tiene su propio buffer ARGB32 (el que se sube a la textura y el que tocan SetPixel y Graphics)
void Image::Create(int width, int height) {
	m_texture = 0;
	m_width = width;
	m_height = height;
	m_image = (unsigned char *) calloc(BTPP, m_width * m_height);
	m_cairo_surface = cairo_image_surface_create_for_data(m_image, CAIRO_FORMAT_ARGB32, m_width, m_height, BTPP * m_width);
	m_cr = cairo_create(m_cairo_surface);
	m_loaded = false;

	List().push_back(this);
}

int Image::GetWidth() const {
	return m_width;
}

int Image::GetHeight() const {
	return m_height;
}

void Image::Load() {
	if (m_loaded)
		return;

	SyncWithCairo();

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
	for (Image *image : List()) {
		image->Unload();
	}
}

void Image::Update() {
	if (m_loaded) {
		SyncWithCairo();
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GetWidth(), GetHeight(), GL_BGRA, GL_UNSIGNED_BYTE, m_image);
	}
}

// El buffer lo comparten cairo (Graphics) y SetPixel/Clear. Antes de leerlo para subirlo a la textura hay que
// pedir a cairo que termine lo pendiente (flush) y avisarle de que el buffer puede haber cambiado por fuera
// (mark_dirty), para que no use datos cacheados si vuelve a dibujar en él
void Image::SyncWithCairo() {
	if (m_cairo_surface == NULL)
		return;

	cairo_surface_flush(m_cairo_surface);
	cairo_surface_mark_dirty(m_cairo_surface);
}

GLuint Image::GetTexture() const {
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
