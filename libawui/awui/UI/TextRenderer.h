#pragma once

#include <awui/String.h>

#include <vector>

namespace awui {
	namespace Drawing {
		class Font;
		class Graphics;
		class Image;
		class GlyphMetrics;
	} // namespace Drawing

	namespace UI {
		class TextRenderer {
		  public:
			static awui::Drawing::GlyphMetrics GetMeasureText(const String text, awui::Drawing::Font *font);
			// Texto partido en líneas de como mucho width píxeles (Graphics::SplitLines)
			static std::vector<String> SplitLines(const String &text, awui::Drawing::Font *font, int width);
		};
	} // namespace UI
} // namespace awui
