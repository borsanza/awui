// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Graphics.h"

#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Drawing/GlyphMetrics.h>
#include <awui/Drawing/Image.h>
#include <awui/Drawing/Pen.h>
#include <awui/Drawing/Size.h>
#include <awui/Math.h>
#include <awui/String.h>

#include <cairo.h>
#include <pango/pangocairo.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <string>

using namespace awui::Drawing;

Graphics::Graphics() {
	m_cairo_surface = NULL;
	m_cr = NULL;
}

Graphics *Graphics::FromImage(Drawing::Image *image) {
	Graphics *graphics = new Graphics();
	graphics->m_cairo_surface = image->m_cairo_surface;
	graphics->m_cr = image->m_cr;

	return graphics;
}

void Graphics::DrawRectangle(Drawing::Pen *pen, float x, float y, float width, float height) {
	SetPen(pen);
	cairo_rectangle(m_cr, x, y, width, height);
	cairo_stroke(m_cr);
}

void Graphics::Clear(const Color color) {
	cairo_set_source_rgba(m_cr, color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);
	cairo_paint(m_cr);
}

void Graphics::FillRectangle(const Color color, float x, float y, float width, float height) {
	cairo_set_source_rgba(m_cr, color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);
	cairo_rectangle(m_cr, x, y, width, height);
	cairo_fill(m_cr);
}

// Contorno de un rectángulo con las esquinas redondeadas (sin pintar)
static void RoundedRectanglePath(cairo_t *cr, float x, float y, float width, float height, float radius) {
	float r = std::min(radius, std::min(width, height) / 2.0f);
	cairo_new_sub_path(cr);
	cairo_arc(cr, x + width - r, y + r, r, -awui::Math::PI / 2.0, 0.0);
	cairo_arc(cr, x + width - r, y + height - r, r, 0.0, awui::Math::PI / 2.0);
	cairo_arc(cr, x + r, y + height - r, r, awui::Math::PI / 2.0, awui::Math::PI);
	cairo_arc(cr, x + r, y + r, r, awui::Math::PI, 3.0 * awui::Math::PI / 2.0);
	cairo_close_path(cr);
}

void Graphics::DrawRoundedRectangle(Drawing::Pen *pen, float x, float y, float width, float height, float radius) {
	SetPen(pen);
	RoundedRectanglePath(m_cr, x, y, width, height, radius);
	cairo_stroke(m_cr);
}

void Graphics::FillRoundedRectangle(const Color color, float x, float y, float width, float height, float radius) {
	cairo_set_source_rgba(m_cr, color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);
	RoundedRectanglePath(m_cr, x, y, width, height, radius);
	cairo_fill(m_cr);
}

void Graphics::DrawImage(Drawing::Image *image, float x, float y) {
	DrawImage(image, x, y, (float) image->GetWidth(), (float) image->GetHeight());
}

void Graphics::DrawImage(Drawing::Image *image, float x, float y, float width, float height) {
	cairo_surface_t *surfaceAux = image->m_cairo_surface;

	cairo_save(m_cr);
	cairo_translate(m_cr, x, y);
	cairo_scale(m_cr, image->GetWidth() / width, image->GetHeight() / height);

	cairo_set_source_surface(m_cr, surfaceAux, 0, 0);
	cairo_paint(m_cr);
	cairo_restore(m_cr);
}

void Graphics::SetPen(Drawing::Pen *pen) {
	Color color = pen->GetColor();
	cairo_set_source_rgba(m_cr, color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);

	cairo_set_line_width(m_cr, pen->GetWidth());

	switch (pen->GetLineCap()) {
		case LineCap::Butt:
			cairo_set_line_cap(m_cr, CAIRO_LINE_CAP_BUTT);
			break;
		case LineCap::Round:
			cairo_set_line_cap(m_cr, CAIRO_LINE_CAP_ROUND);
			break;
		case LineCap::Square:
			cairo_set_line_cap(m_cr, CAIRO_LINE_CAP_SQUARE);
			break;
	}

	// Unión entre segmentos de un mismo trazo (esquinas de DrawRectangle, por ejemplo)
	switch (pen->GetLineJoin()) {
		case LineJoin::Miter:
			cairo_set_line_join(m_cr, CAIRO_LINE_JOIN_MITER);
			break;
		case LineJoin::Round:
			cairo_set_line_join(m_cr, CAIRO_LINE_JOIN_ROUND);
			break;
		case LineJoin::Bevel:
			cairo_set_line_join(m_cr, CAIRO_LINE_JOIN_BEVEL);
			break;
	}
}

void Graphics::DrawLine(Drawing::Pen *pen, float x1, float y1, float x2, float y2) {
	SetPen(pen);
	cairo_save(m_cr);
	cairo_move_to(m_cr, x1, y1);
	cairo_line_to(m_cr, x2, y2);
	cairo_stroke(m_cr);
	cairo_restore(m_cr);
}

#define BORDER 2

// Texto con Pango (sobre cairo): si a la fuente le falta un carácter (japonés, chino...) busca otra que lo tenga, y
// compone bien las escrituras complejas y las de derecha a izquierda
static std::string s_textLanguage;

// En Windows, pango usa DirectWrite, y cairo no sabe dibujar las fuentes que se le añaden (AddFontsFromDirectory) en
// una imagen: se usa fontconfig y FreeType, como en Linux. La configuración de fontconfig va junto al ejecutable
// (etc/fonts, la copia la compilación). Hay que elegirlo antes de que pango cree nada
static void InitFontBackend() {
#ifdef _WIN32
	static bool done = false;
	if (done)
		return;
	done = true;

	if (!getenv("PANGOCAIRO_BACKEND"))
		_putenv("PANGOCAIRO_BACKEND=fc");
#endif
}

void Graphics::SetTextLanguage(const String &code) {
	s_textLanguage = code.ToStdString();
}

int Graphics::AddFontsFromDirectory(const String &directory) {
	InitFontBackend();

	int count = 0;
#if PANGO_VERSION_CHECK(1, 56, 0)
	std::error_code error;
	PangoFontMap *fontMap = pango_cairo_font_map_get_default();
	for (const auto &entry : std::filesystem::directory_iterator(directory.ToCharArray(), error)) {
		std::string extension = entry.path().extension().string();
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
		if ((extension != ".ttf") && (extension != ".otf"))
			continue;

		if (pango_font_map_add_font_file(fontMap, entry.path().string().c_str(), nullptr))
			count++;
	}
#else
	(void) directory; // pango_font_map_add_font_file llegó en pango 1.56: se usan solo las fuentes del sistema
#endif
	return count;
}

static PangoLayout *CreateLayout(cairo_t *cr, const awui::String &text, Font *font) {
	InitFontBackend();
	PangoLayout *layout = pango_cairo_create_layout(cr);

	if (!s_textLanguage.empty()) {
		PangoAttrList *attributes = pango_attr_list_new();
		pango_attr_list_insert(attributes, pango_attr_language_new(pango_language_from_string(s_textLanguage.c_str())));
		pango_layout_set_attributes(layout, attributes);
		pango_attr_list_unref(attributes);
	}

	PangoFontDescription *description = pango_font_description_new();
	pango_font_description_set_family(description, font->GetFont().ToCharArray());
	pango_font_description_set_weight(description, font->GetBold() ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL);
	pango_font_description_set_style(description, font->GetItalic() ? PANGO_STYLE_ITALIC : PANGO_STYLE_NORMAL);
	// En píxeles, como cairo_set_font_size (el tamaño normal de Pango va en puntos)
	pango_font_description_set_absolute_size(description, font->GetSize() * PANGO_SCALE);
	pango_layout_set_font_description(layout, description);
	pango_font_description_free(description);

	pango_layout_set_text(layout, text.ToCharArray(), -1);
	return layout;
}

// Medidas del texto como las daba cairo: la tinta (ink) respecto al punto de partida en la línea base
struct TextExtents {
	int bearingX; // Desde el punto de partida hasta el primer píxel de tinta
	int bearingY; // Desde la línea base hasta el píxel de tinta más alto (negativo: por encima)
	int width;	  // Tinta
	int height;
	int advance; // Hasta donde empezaría el texto siguiente
	int ascent;	 // De la línea (no de la tinta): por encima y por debajo de la línea base
	int descent;
	int inkX; // Tinta respecto a la esquina superior izquierda del layout (para colocarlo al pintar)
	int inkY;
};

static TextExtents GetExtents(PangoLayout *layout) {
	PangoRectangle ink, logical;
	pango_layout_get_pixel_extents(layout, &ink, &logical);
	int baseline = pango_layout_get_baseline(layout) / PANGO_SCALE;

	TextExtents e;
	e.bearingX = ink.x - logical.x;
	e.bearingY = ink.y - baseline;
	e.width = ink.width;
	e.height = ink.height;
	e.advance = logical.width;
	e.ascent = baseline - logical.y;
	e.descent = logical.y + logical.height - baseline;
	e.inkX = ink.x;
	e.inkY = ink.y;
	return e;
}

GlyphMetrics Graphics::GetMeasureText(const String text, Drawing::Font *font) const {
	PangoLayout *layout = CreateLayout(m_cr, text, font);
	TextExtents extents = GetExtents(layout);
	g_object_unref(layout);

	GlyphMetrics metrics;
	metrics.SetWidth(extents.width + BORDER * 2);
	metrics.SetHeight(extents.height + BORDER * 2);
	metrics.SetAdvanceX(extents.advance);
	metrics.SetAdvanceY(0);
	metrics.SetBearingX(extents.bearingX);
	metrics.SetBearingY(extents.bearingY);
	metrics.SetAscent(extents.ascent);
	metrics.SetDescent(extents.descent);
	return metrics;
}

std::vector<awui::String> Graphics::SplitLines(const String &text, Drawing::Font *font, int width) const {
	std::vector<String> lines;
	if (text.IsEmpty())
		return lines;

	PangoLayout *layout = CreateLayout(m_cr, text, font);
	// El ancho de cada línea al dibujarla lleva un margen (BORDER) a cada lado
	pango_layout_set_width(layout, std::max(1, width - (BORDER * 2)) * PANGO_SCALE);
	pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);

	const std::string &utf8 = text.ToStdString();
	int count = pango_layout_get_line_count(layout);
	for (int i = 0; i < count; i++) {
		PangoLayoutLine *line = pango_layout_get_line_readonly(layout, i);
		String part(utf8.substr(line->start_index, line->length));
		lines.push_back(part.TrimEnd());
	}

	g_object_unref(layout);
	return lines;
}

void Graphics::DrawString(const String text, Drawing::Font *font, const Color color, float x, float y) {
	PangoLayout *layout = CreateLayout(m_cr, text, font);
	TextExtents extents = GetExtents(layout);

	// La tinta empieza en (x + BORDER, y + BORDER), como antes con cairo
	cairo_save(m_cr);
	cairo_set_source_rgba(m_cr, color.GetR() / 255.0f, color.GetG() / 255.0f, color.GetB() / 255.0f, color.GetA() / 255.0f);
	cairo_move_to(m_cr, (int) x - extents.inkX + BORDER, (int) y - extents.inkY + BORDER);
	pango_cairo_show_layout(m_cr, layout);
	cairo_restore(m_cr);
	g_object_unref(layout);

	if (font->GetStrikeout() || font->GetUnderline()) {
		float size = font->GetSize() * 0.07f;
		size = Math::Max(Math::Round(size), 1.0f);
		Pen pen = Pen(color, size);

		cairo_antialias_t old = cairo_get_antialias(m_cr);
		cairo_set_antialias(m_cr, cairo_antialias_t::CAIRO_ANTIALIAS_NONE);

		// Las líneas van donde está la tinta del texto: de x + BORDER a x + BORDER + ancho (como el propio texto)
		if (font->GetStrikeout()) {
			float posy = y - (extents.bearingY / 2.0f) + BORDER;
			posy = Math::Round(posy);
			DrawLine(&pen, x + BORDER, posy, x + extents.width + BORDER, posy);
		}

		if (font->GetUnderline()) {
			float posy = y - extents.bearingY + BORDER + (size * 1.5f);
			posy = Math::Round(posy);
			DrawLine(&pen, x + BORDER, posy, x + extents.width + BORDER, posy);
		}

		cairo_set_antialias(m_cr, old);
	}
}
