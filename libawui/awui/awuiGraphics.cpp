// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiGraphics.h"

#include "awuiImage.h"
#include "awuiColor.h"
#include "awuiPen.h"
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

void awuiGraphics::FillRectangle(awuiColor * color, int x, int y, int width, int height) {
	cairo_set_source_rgba(this->cr, color->GetR() / 255.0f, color->GetG() / 255.0f, color->GetB() / 255.0f, color->GetA() / 255.0f);
	cairo_rectangle(this->cr, x, y, width, height);
	cairo_fill(this->cr);
}

void awuiGraphics::DrawLine(awuiPen * pen, float x1, float y1, float x2, float y2) {
	awuiColor * color = pen->GetColor();
	cairo_set_source_rgba(this->cr, color->GetR() / 255.0f, color->GetG() / 255.0f, color->GetB() / 255.0f, color->GetA() / 255.0f);
	delete color;

	cairo_set_line_width(this->cr, pen->GetWidth());

	switch (pen->GetCap()) {
		case awuiCap::Butt:
			cairo_set_line_cap(this->cr, CAIRO_LINE_CAP_BUTT);
			break;
		case awuiCap::Round:
			cairo_set_line_cap(this->cr, CAIRO_LINE_CAP_ROUND);
			break;
		case awuiCap::Square:
			cairo_set_line_cap(this->cr, CAIRO_LINE_CAP_SQUARE);
			break;
	}

	cairo_move_to(this->cr, x1, y1);
	cairo_line_to(this->cr, x2, y2);
	cairo_stroke(this->cr);
}