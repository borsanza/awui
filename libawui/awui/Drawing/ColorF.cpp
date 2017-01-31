/**
 * awui/Drawing/ColorF.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "ColorF.h"

#include <awui/Convert.h>
#include <awui/Math.h>
#include <awui/String.h>
#include <cmath>
#include <stdlib.h>

using namespace awui::Drawing;

ColorF::ColorF() {
	this->a = 0.0f;
	this->r = 0.0f;
	this->g = 0.0f;
	this->b = 0.0f;
}

int ColorF::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::ColorF)
		return 1;

	return Object::IsClass(objectClass);
}

awui::String ColorF::ToString() {
	String value;
	value = String("ColorF [A=") + Convert::ToString(this->a) +
				", R=" + Convert::ToString(this->r) +
				", G=" + Convert::ToString(this->g) +
				", B=" + Convert::ToString(this->b) + "]";
	return value;
}

float ColorF::GetA() const {
	return this->a;
}

float ColorF::GetR() const {
	return this->r;
}

float ColorF::GetG() const {
	return this->g;
}

float ColorF::GetB() const {
	return this->b;
}

int ColorF::ToArgb() const {
	return (((((this->a * 256) + this->r) * 256) + this->g) * 256) + this->b;
}

float ColorF::GetBrightness() const {
	int M = Math::Max(Math::Max(this->r, this->g), this->b);
	int m = Math::Min(Math::Min(this->r, this->g), this->b);

	return ((M + m) / 2.0f) / 255.0f;
}

float ColorF::GetHue() const {
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

float ColorF::GetSaturation() const {
	int M = Math::Max(Math::Max(this->r, this->g), this->b);
	int m = Math::Min(Math::Min(this->r, this->g), this->b);

	double C = M - m;

	double L = ((M + m) / 2.0) / 255.0;

	double value = C / (1 - fabs((2 * L) - 1));

	value /= 255.0;

	return (float)value;
}

ColorF ColorF::FromArgb(int argb) {
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

	return ColorF::FromArgb(a, r, g, b);
}

ColorF ColorF::FromArgb(float alpha, const ColorF baseColor) {
	return ColorF::FromArgb(alpha, baseColor.r, baseColor.g, baseColor.b);
}

ColorF ColorF::FromArgb(float red, float green, float blue) {
	return ColorF::FromArgb(255, red, green, blue);
}

ColorF ColorF::FromArgb(float alpha, float red, float green, float blue) {
	ColorF color;

	color.a = Math::Clamp(alpha, 0.0f, 255.0f);
	color.r = Math::Clamp(red, 0.0f, 255.0f);
	color.g = Math::Clamp(green, 0.0f, 255.0f);
	color.b = Math::Clamp(blue, 0.0f, 255.0f);

	return color;
}

ColorF & ColorF::operator=(const ColorF & other) {
	this->r = other.r;
	this->g = other.g;
	this->b = other.b;
	this->a = other.a;

	return *this;
}

bool ColorF::operator !=(const ColorF &b) const {
	return ((this->r != b.r) || (this->g != b.g) || (this->b != b.b) || (this->a != b.a));
}
