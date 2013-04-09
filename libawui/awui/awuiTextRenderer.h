// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <string>

namespace awui {
	namespace Drawing {
		class Font;
		class Graphics;
		class Image;
		class Size;
	}
}

class awuiTextRenderer {
private:
	static awui::Drawing::Graphics * graphics;
	static awui::Drawing::Image * image;

public:
	static awui::Drawing::Size GetMeasureText(const std::string& text, awui::Drawing::Font *font);
};
