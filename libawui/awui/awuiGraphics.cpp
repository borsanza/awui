// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiGraphics.h"

#include "awuiImage.h"
#include <cairo.h>

awuiGraphics::awuiGraphics(void)
{
}

awuiGraphics::~awuiGraphics(void)
{
	if (this->cr != 0) {
		cairo_destroy(this->cr);
		this->cr = 0;
	}

	if (this->cairo_surface != 0) {
		cairo_surface_destroy(this->cairo_surface);
		this->cairo_surface = 0;
	}
}

awuiGraphics * awuiGraphics::FromImage(awuiImage *image) {
	awuiGraphics * graphics = new awuiGraphics();
	graphics->cairo_surface = cairo_image_surface_create_for_data (image->image, CAIRO_FORMAT_ARGB32, image->GetWidth(), image->GetHeight(), image->btpp * image->width);
	graphics->cr = cairo_create(graphics->cairo_surface);

	return graphics;
}

void awuiGraphics::FillRectangle(int r, int g, int b, int a, int x, int y, int width, int height) {
	cairo_set_source_rgba(this->cr, r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	cairo_rectangle(this->cr, x, y, width, height);
	cairo_fill(this->cr);
}

void awuiGraphics::DrawLine(int r, int g, int b, int a, int x1, int y1, int x2, int y2) {
	cairo_set_source_rgba(this->cr, r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	cairo_set_line_width(this->cr, 1.0);
	cairo_move_to(this->cr, x1, y1);
	cairo_line_to(this->cr, x2, y2);
	cairo_stroke (this->cr);
}