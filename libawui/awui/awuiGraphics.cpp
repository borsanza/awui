// (c) Copyright 2011 Borja S�nchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiGraphics.h"

#include "awuiImage.h"
#include "awuiColor.h"
#include "awuiPen.h"
#include <cairo.h>
#include <stdlib.h>

awuiGraphics::awuiGraphics(void)
{
}

awuiGraphics::~awuiGraphics(void)
{
	if (this->cr != NULL)
		cairo_destroy(this->cr);

	if (this->cairo_surface != NULL)
		cairo_surface_destroy(this->cairo_surface);
}

awuiGraphics * awuiGraphics::FromImage(awuiImage *image) {
	awuiGraphics * graphics = new awuiGraphics();
	graphics->cairo_surface = cairo_image_surface_create_for_data(image->image, CAIRO_FORMAT_ARGB32, image->GetWidth(), image->GetHeight(), image->btpp * image->width);
	graphics->cr = cairo_create(graphics->cairo_surface);

	return graphics;
}

void awuiGraphics::DrawRectangle(awuiPen * pen, float x, float y, float width, float height) {
	this->SetPen(pen);
	cairo_rectangle(this->cr, x, y, width, height);
	cairo_stroke(this->cr);
}

void awuiGraphics::FillRectangle(awuiColor * color, float x, float y, float width, float height) {
	cairo_set_source_rgba(this->cr, color->GetR() / 255.0f, color->GetG() / 255.0f, color->GetB() / 255.0f, color->GetA() / 255.0f);
	cairo_rectangle(this->cr, x, y, width, height);
	cairo_fill(this->cr);
}

void awuiGraphics::DrawImage(awuiImage * image, float x, float y) {
	this->DrawImage(image, x, y, (float)image->GetWidth(), (float)image->GetHeight());
}

void awuiGraphics::DrawImage(awuiImage * image, float x, float y, float width, float height) {
	cairo_surface_t *surfaceAux = cairo_image_surface_create_for_data(image->image, CAIRO_FORMAT_ARGB32, image->GetWidth(), image->GetHeight(), image->btpp * image->width);

	cairo_translate(this->cr, x, y);
	cairo_scale(this->cr, image->GetWidth() / width, image->GetHeight() / height);

	cairo_set_source_surface(this->cr, surfaceAux, 0, 0);
	cairo_paint(this->cr);

	cairo_surface_destroy(surfaceAux);
}

void awuiGraphics::SetPen(awuiPen * pen) {
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
}

void awuiGraphics::DrawLine(awuiPen * pen, float x1, float y1, float x2, float y2) {
	this->SetPen(pen);
	cairo_move_to(this->cr, x1, y1);
	cairo_line_to(this->cr, x2, y2);
	cairo_stroke(this->cr);
}