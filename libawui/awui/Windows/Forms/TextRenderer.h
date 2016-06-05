#pragma once

#include <awui/String.h>

namespace awui {
	namespace Drawing {
		class Font;
		class Graphics;
		class Image;
		class GlyphMetrics;
	}

	namespace Windows {
		namespace Forms {
			class TextRenderer {
				private:
					static awui::Drawing::Graphics * graphics;
					static awui::Drawing::Image * image;

				public:
					static awui::Drawing::GlyphMetrics GetMeasureText(const String text, awui::Drawing::Font *font);
			};
		}
	}
}
