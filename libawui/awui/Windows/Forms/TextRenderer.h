// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/String.h>

namespace awui {
	namespace Drawing {
		class Font;
		class Graphics;
		class Image;
		class Size;
	}
	namespace Windows {
		namespace Forms {
			class TextRenderer {
			private:
				static awui::Drawing::Graphics * graphics;
				static awui::Drawing::Image * image;

			public:
				static awui::Drawing::Size GetMeasureText(const String text, awui::Drawing::Font *font);
			};
		}
	}
}
