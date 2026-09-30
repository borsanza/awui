#pragma once

#include <awui/String.h>

namespace awui {
	namespace Drawing {
		class Font;
		class Graphics;
		class Image;
		class GlyphMetrics;
	} // namespace Drawing

	namespace Windows::Forms {
		class TextRenderer {
		  private:
			static awui::Drawing::Graphics *s_graphics;
			static awui::Drawing::Image *s_image;

		  public:
			static awui::Drawing::GlyphMetrics GetMeasureText(const String text, awui::Drawing::Font *font);
		};
	} // namespace Windows::Forms
} // namespace awui
