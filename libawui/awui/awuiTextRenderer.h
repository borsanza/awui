// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <string>

class awuiSize;
class awuiFont;
class awuiGraphics;
class awuiImage;

class awuiTextRenderer {
private:
	static awuiGraphics * graphics;
	static awuiImage * image;

public:
	static awuiSize GetMeasureText(const std::string& text, awuiFont *font);
};