/**
 * awui/Drawing/ColorF.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "ColorF.h"

#include <awui/Convert.h>
#include <awui/Math.h>
#include <awui/String.h>
#include <stdlib.h>

using namespace awui::Drawing;

ColorF::ColorF() {
	m_class = Classes::ColorF;
	m_a = 0.0f;
	m_r = 0.0f;
	m_g = 0.0f;
	m_b = 0.0f;
}

bool ColorF::IsClass(Classes objectClass) const {
	return (objectClass == Classes::ColorF) || Object::IsClass(objectClass);
}

awui::String ColorF::ToString() const {
	String value;
	value = String("ColorF [A=") + Convert::ToString(m_a) +
				", R=" + Convert::ToString(m_r) +
				", G=" + Convert::ToString(m_g) +
				", B=" + Convert::ToString(m_b) + "]";
	return value;
}

float ColorF::GetA() const {
	return m_a;
}

float ColorF::GetR() const {
	return m_r;
}

float ColorF::GetG() const {
	return m_g;
}

float ColorF::GetB() const {
	return m_b;
}

int ColorF::ToArgb() const {
	return (((((m_a * 256) + m_r) * 256) + m_g) * 256) + m_b;
}

float ColorF::GetBrightness() const {
	int M = Math::Max(Math::Max(m_r, m_g), m_b);
	int m = Math::Min(Math::Min(m_r, m_g), m_b);

	return ((M + m) / 2.0f) / 255.0f;
}

float ColorF::GetHue() const {
	int M = Math::Max(Math::Max(m_r, m_g), m_b);
	int m = Math::Min(Math::Min(m_r, m_g), m_b);

	double C = M - m;

	double H;
	if (C == 0)
		H = 0.0;
	else if (M == m_r)
		H = Math::FMod((m_g - m_b) / C, 6);
	else if (M == m_g)
		H = ((m_b - m_r) / C) + 2.0;
	else
		H = ((m_r - m_g) / C) + 4.0;

	H = (60.0 * H);

	while (H < 0.0)
		H += 360.0;

	return (float)H;
}

float ColorF::GetSaturation() const {
	int M = Math::Max(Math::Max(m_r, m_g), m_b);
	int m = Math::Min(Math::Min(m_r, m_g), m_b);

	double C = M - m;

	double L = ((M + m) / 2.0) / 255.0;

	double value = C / (1 - Math::Abs((2.0 * L) - 1.0));

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
	return ColorF::FromArgb(alpha, baseColor.m_r, baseColor.m_g, baseColor.m_b);
}

ColorF ColorF::FromArgb(float red, float green, float blue) {
	return ColorF::FromArgb(255, red, green, blue);
}

ColorF ColorF::FromArgb(float alpha, float red, float green, float blue) {
	ColorF color;

	color.m_a = Math::Clamp(alpha, 0.0f, 255.0f);
	color.m_r = Math::Clamp(red, 0.0f, 255.0f);
	color.m_g = Math::Clamp(green, 0.0f, 255.0f);
	color.m_b = Math::Clamp(blue, 0.0f, 255.0f);

	return color;
}

ColorF & ColorF::operator=(const ColorF & other) {
	m_r = other.m_r;
	m_g = other.m_g;
	m_b = other.m_b;
	m_a = other.m_a;

	return *this;
}

bool ColorF::operator !=(const ColorF &b) const {
	return ((m_r != b.m_r) || (m_g != b.m_g) || (m_b != b.m_b) || (m_a != b.m_a));
}
