// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiColor.h"

#include "awuiMath.h"
#include <math.h>

awuiColor::awuiColor() {
	this->a = 0;
	this->r = 0;
	this->g = 0;
	this->b = 0;
}

unsigned char awuiColor::GetA() {
	return this->a;
}

unsigned char awuiColor::GetR() {
	return this->r;
}

unsigned char awuiColor::GetG() {
	return this->g;
}

unsigned char awuiColor::GetB() {
	return this->b;
}

int awuiColor::ToArgb() {
	return (((((a << 8) + r) << 8) + g) << 8) + b;
}

float awuiColor::GetBrightness() {
	int M = awuiMath::Max(awuiMath::Max(this->r, this->g), this->b);
	int m = awuiMath::Min(awuiMath::Min(this->r, this->g), this->b);
	return ((M + m) / 2.0) / 255.0;
}

float awuiColor::GetHue() {
	int M = awuiMath::Max(awuiMath::Max(this->r, this->g), this->b);
	int m = awuiMath::Min(awuiMath::Min(this->r, this->g), this->b);

	double C = M - m;

	double H;
	if (C == 0)
		H = 0.0;
	else if (M == this->r)
		H = fmod((this->g - this->b) / C, 6);
	else if (M == this->g)
		H = ((this->b - this->r) / C) + 2.0;
	else
		H = ((this->r - this->g) / C) + 4.0;

	H = (60.0 * H);

	while (H < 0.0)
		H += 360.0;

	return H;
}

float awuiColor::GetSaturation() {
	int M = awuiMath::Max(awuiMath::Max(this->r, this->g), this->b);
	int m = awuiMath::Min(awuiMath::Min(this->r, this->g), this->b);

	double C = M - m;

	double L = ((M + m) / 2.0) / 255.0;

	double value = C / (1 - abs((2 * L) - 1));

	value /= 255.0;

	return value;
}

awuiColor * awuiColor::FromArgb(int argb) {
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

	return awuiColor::FromArgb(a, r, g, b);
}

awuiColor * awuiColor::FromArgb(int alpha, awuiColor * baseColor) {
	return awuiColor::FromArgb(alpha, baseColor->r, baseColor->g, baseColor->b);
}

awuiColor * awuiColor::FromArgb(int red, int green, int blue) {
	return awuiColor::FromArgb(255, red, green, blue);
}

awuiColor * awuiColor::FromArgb(int alpha, int red, int green, int blue) {
	awuiColor * color = new awuiColor();

	color->a = (alpha > 255)? 255 : (alpha < 0) ? 0 : alpha;
	color->r = (red > 255)? 255 : (red < 0) ? 0 : red;
	color->g = (green > 255)? 255 : (green < 0) ? 0 : green;
	color->b = (blue > 255)? 255 : (blue < 0) ? 0 : blue;

	return color;
}