// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Color.h"

#include <awui/Math.h>
#include <stdlib.h>

using namespace awui::Drawing;

Color::Color() {
	this->a = 0;
	this->r = 0;
	this->g = 0;
	this->b = 0;
}

int Color::IsClass(Classes objectClass) const {
	if (objectClass == awui::Color)
		return 1;

	return Object::IsClass(objectClass);
}

unsigned char Color::GetA() const {
	return this->a;
}

unsigned char Color::GetR() const {
	return this->r;
}

unsigned char Color::GetG() const {
	return this->g;
}

unsigned char Color::GetB() const {
	return this->b;
}

int Color::ToArgb() const {
	return (((((this->a << 8) + this->r) << 8) + this->g) << 8) + this->b;
}

float Color::GetBrightness() const {
	int M = Math::Max(Math::Max(this->r, this->g), this->b);
	int m = Math::Min(Math::Min(this->r, this->g), this->b);

	return ((M + m) / 2.0f) / 255.0f;
}

float Color::GetHue() const {
	int M = Math::Max(Math::Max(this->r, this->g), this->b);
	int m = Math::Min(Math::Min(this->r, this->g), this->b);

	double C = M - m;

	double H;
	if (C == 0)
		H = 0.0;
	else if (M == this->r)
		H = Math::FMod((this->g - this->b) / C, 6);
	else if (M == this->g)
		H = ((this->b - this->r) / C) + 2.0;
	else
		H = ((this->r - this->g) / C) + 4.0;

	H = (60.0 * H);

	while (H < 0.0)
		H += 360.0;

	return (float)H;
}

float Color::GetSaturation() const {
	int M = Math::Max(Math::Max(this->r, this->g), this->b);
	int m = Math::Min(Math::Min(this->r, this->g), this->b);

	double C = M - m;

	double L = ((M + m) / 2.0) / 255.0;

	double value = C / (1 - abs((2 * L) - 1));

	value /= 255.0;

	return (float)value;
}

Color Color::FromArgb(int argb) {
	unsigned char a;
	unsigned char r;
	unsigned char g;
	unsigned char b;

	b = argb % 256;
	argb = argb >> 8;
	g = argb % 256;
	argb = argb >> 8;
	r = argb % 256;
	argb = argb >> 8;
	a = argb % 256;

	return Color::FromArgb(a, r, g, b);
}

Color Color::FromArgb(int alpha, const Color baseColor) {
	return Color::FromArgb(alpha, baseColor.r, baseColor.g, baseColor.b);
}

Color Color::FromArgb(int red, int green, int blue) {
	return Color::FromArgb(255, red, green, blue);
}

Color Color::FromArgb(int alpha, int red, int green, int blue) {
	Color color;

	color.a = (alpha > 255)? 255 : (alpha < 0) ? 0 : alpha;
	color.r = (red > 255)? 255 : (red < 0) ? 0 : red;
	color.g = (green > 255)? 255 : (green < 0) ? 0 : green;
	color.b = (blue > 255)? 255 : (blue < 0) ? 0 : blue;

	return color;
}

Color & Color::operator=(const Color & other) {
	this->r = other.r;
	this->g = other.g;
	this->b = other.b;
	this->a = other.a;

	return *this;
}
