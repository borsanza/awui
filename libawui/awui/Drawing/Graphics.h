#pragma once

#include <awui/Object.h>
#include <awui/String.h>

#include <vector>

typedef struct _cairo_surface cairo_surface_t;
typedef struct _cairo cairo_t;

namespace awui {
	class String;

	namespace Drawing {
		class Color;
		class Font;
		class Image;
		class Pen;
		class GlyphMetrics;

		class Graphics : public Object {
		  private:
			cairo_surface_t *m_cairo_surface;
			cairo_t *m_cr;

			void SetPen(Drawing::Pen *pen);
			Graphics();

		  public:
			virtual ~Graphics() = default;

			static Graphics *FromImage(Drawing::Image *image);

			// Idioma de los textos ("ja_JP", "zh_CN"...): con él se elige la fuente de reserva y la forma de los
			// caracteres que comparten el chino y el japonés. Vacío: el del sistema
			static void SetTextLanguage(const String &code);

			void Clear(const Drawing::Color color);
			void DrawRectangle(Drawing::Pen *pen, float x, float y, float width, float height);
			void FillRectangle(const Drawing::Color color, float x, float y, float width, float height);
			// Con las esquinas redondeadas (radius: radio de las esquinas, en píxeles)
			void DrawRoundedRectangle(Drawing::Pen *pen, float x, float y, float width, float height, float radius);
			void FillRoundedRectangle(const Drawing::Color color, float x, float y, float width, float height, float radius);
			void DrawImage(Drawing::Image *image, float x, float y);
			void DrawImage(Drawing::Image *image, float x, float y, float width, float height);
			void DrawLine(Drawing::Pen *pen, float x1, float y1, float x2, float y2);

			Drawing::GlyphMetrics GetMeasureText(const String text, Drawing::Font *font) const;
			// Parte el texto en líneas que quepan en width píxeles (por palabras; en idiomas sin espacios, como el
			// japonés o el chino, por donde permita el idioma)
			std::vector<String> SplitLines(const String &text, Drawing::Font *font, int width) const;
			void DrawString(const String text, Drawing::Font *font, const Drawing::Color color, float x, float y);
		};
	} // namespace Drawing
} // namespace awui
