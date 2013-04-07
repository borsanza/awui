// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiGraphics.h"

#include "awuiColor.h"
#include "awuiFont.h"
#include "awuiImage.h"
#include "awuiPen.h"
#include "awuiSize.h"

#include <cairo.h>
#include <stdlib.h>
#include <iostream>

awuiGraphics::awuiGraphics() {
}

awuiGraphics::~awuiGraphics() {
}

int awuiGraphics::IsClass(awuiObject::awuiClasses objectClass) const {
	if (objectClass == awuiObject::Graphics)
		return 1;

	return awuiObject::IsClass(objectClass);
}

awuiGraphics * awuiGraphics::FromImage(awuiImage *image) {
	awuiGraphics * graphics = new awuiGraphics();
	graphics->cairo_surface = image->cairo_surface;
	graphics->cr = image->cr;

	return graphics;
}

void awuiGraphics::DrawRectangle(awuiPen * pen, float x, float y, float width, float height) {
	this->SetPen(pen);
	cairo_rectangle(this->cr, x, y, width, height);
	cairo_stroke(this->cr);
}

void awuiGraphics::Clear(const awuiColor color) {
	cairo_set_source_rgba(this->cr, color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);
	cairo_paint(this->cr);
}

void awuiGraphics::FillRectangle(const awuiColor color, float x, float y, float width, float height) {
	cairo_set_source_rgba(this->cr, color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);
	cairo_rectangle(this->cr, x, y, width, height);
	cairo_fill(this->cr);
}

void awuiGraphics::DrawImage(awuiImage * image, float x, float y) {
	this->DrawImage(image, x, y, (float)image->GetWidth(), (float)image->GetHeight());
}

void awuiGraphics::DrawImage(awuiImage * image, float x, float y, float width, float height) {
	cairo_surface_t *surfaceAux = image->cairo_surface;

	cairo_save(this->cr);
	cairo_translate(this->cr, x, y);
	cairo_scale(this->cr, image->GetWidth() / width, image->GetHeight() / height);

	cairo_set_source_surface(this->cr, surfaceAux, 0, 0);
	cairo_paint(this->cr);
	cairo_restore(this->cr);
}

void awuiGraphics::SetPen(awuiPen * pen) {
	awuiColor color = pen->GetColor();
	cairo_set_source_rgba(this->cr, color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);

	cairo_set_line_width(this->cr, pen->GetWidth());

	switch (pen->GetLineCap()) {
		case awuiLineCap::Butt:
			cairo_set_line_cap(this->cr, CAIRO_LINE_CAP_BUTT);
			break;
		case awuiLineCap::Round:
			cairo_set_line_cap(this->cr, CAIRO_LINE_CAP_ROUND);
			break;
		case awuiLineCap::Square:
			cairo_set_line_cap(this->cr, CAIRO_LINE_CAP_SQUARE);
			break;
	}
}

void awuiGraphics::DrawLine(awuiPen * pen, float x1, float y1, float x2, float y2) {
	this->SetPen(pen);
	cairo_save(this->cr);
	cairo_move_to(this->cr, x1, y1);
	cairo_line_to(this->cr, x2, y2);
	cairo_stroke(this->cr);
	cairo_restore(this->cr);
}

awuiSize awuiGraphics::GetMeasureText(const std::string& text, awuiFont *font) const {
	cairo_text_extents_t extents;

	cairo_save(this->cr);
	cairo_select_font_face(this->cr, font->GetFont().c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(this->cr, font->GetSize());
	cairo_text_extents(this->cr, text.c_str(), &extents);
	cairo_restore(this->cr);

	return awuiSize(extents.width + 4, extents.height + 4);
}

void awuiGraphics::DrawString(const std::string& text, awuiFont * font, float x, float y) {
	cairo_text_extents_t extents;
	cairo_save(this->cr);
	cairo_select_font_face(this->cr, font->GetFont().c_str(), CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(this->cr, font->GetSize());
	cairo_text_extents(this->cr, text.c_str(), &extents);
/*
	std::cout << "Bearing: " << extents.x_bearing << "x" << extents.y_bearing << std::endl;
	std::cout << "Size: " << extents.width << "x" << extents.height << std::endl;
	std::cout << "Advance: " << extents.x_advance << "x" << extents.y_advance << std::endl;
*/
	cairo_move_to(this->cr, x + 2 - extents.x_bearing, y - 2 - (extents.height + extents.y_bearing));
	cairo_show_text(this->cr, text.c_str());
	cairo_restore(this->cr);
}
