// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <string>

class awuiFont {
private:
	std::string font;
	bool bold;
	float size;

public:
	awuiFont(const std::string& font, float size);

	const std::string& GetFont();
	bool GetBold();
	float GetSize();
};