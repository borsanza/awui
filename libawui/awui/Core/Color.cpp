/**
 * awui/Core/Color.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "Color.h"

#include <awui/Math.h>
#include <awui/String.h>

using namespace awui;

Color::Color() : m_a(0), m_r(0), m_g(0), m_b(0) {
	m_class = Classes::Color;
}

Color::Color(uint32_t color) {
	m_r = (color >> 24) & 0xFF;
	m_g = (color >> 16) & 0xFF;
	m_b = (color >> 8) & 0xFF;
	m_a = (color >> 0) & 0xFF;
}

Color::Color(float r, float g, float b, float a) {
	m_r = static_cast<uint8_t>(Math::Min(Math::Max(r, 0.0f), 1.0f) * 255.0f);
	m_g = static_cast<uint8_t>(Math::Min(Math::Max(g, 0.0f), 1.0f) * 255.0f);
	m_b = static_cast<uint8_t>(Math::Min(Math::Max(b, 0.0f), 1.0f) * 255.0f);
	m_a = static_cast<uint8_t>(Math::Min(Math::Max(a, 0.0f), 1.0f) * 255.0f);
}

bool Color::IsClass(Classes objectClass) const {
	return (objectClass == Classes::Color) || Object::IsClass(objectClass);
}

awui::String Color::ToString() const {
	return String("Color [A=%d, R=%d, G=%d, B=%d]", m_a, m_r, m_g, m_b);
}

uint8_t Color::GetA() const {
	return m_a;
}

uint8_t Color::GetR() const {
	return m_r;
}

uint8_t Color::GetG() const {
	return m_g;
}

uint8_t Color::GetB() const {
	return m_b;
}

uint32_t Color::ToArgb() const {
	return (m_a << 24) | (m_r << 16) | (m_g << 8) | m_b;
}

float Color::GetBrightness() const {
	int M = Math::Max(Math::Max(m_r, m_g), m_b);
	int m = Math::Min(Math::Min(m_r, m_g), m_b);

	return ((M + m) / 2.0f) / 255.0f;
}

float Color::GetHue() const {
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

	return (float) H;
}

float Color::GetSaturation() const {
	int M = Math::Max(Math::Max(m_r, m_g), m_b);
	int m = Math::Min(Math::Min(m_r, m_g), m_b);

	double C = M - m;

	double L = ((M + m) / 2.0) / 255.0;

	double value = C / (1.0 - Math::Abs((2.0 * L) - 1.0));

	value /= 255.0;

	return (float) value;
}

Color Color::FromArgb(uint32_t argb) {
	auto a = static_cast<std::uint8_t>((argb >> 24) & 0xFF);
	auto r = static_cast<std::uint8_t>((argb >> 16) & 0xFF);
	auto g = static_cast<std::uint8_t>((argb >> 8) & 0xFF);
	auto b = static_cast<std::uint8_t>(argb & 0xFF);

	return Color::FromArgb(a, r, g, b);
}

Color Color::FromArgb(uint8_t alpha, const Color baseColor) {
	return Color::FromArgb(alpha, baseColor.m_r, baseColor.m_g, baseColor.m_b);
}

Color Color::FromArgb(uint8_t red, uint8_t green, uint8_t blue) {
	return Color::FromArgb(255, red, green, blue);
}

Color Color::FromArgb(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue) {
	Color color;

	color.m_a = Math::Clamp(alpha, 0, 255);
	color.m_r = Math::Clamp(red, 0, 255);
	color.m_g = Math::Clamp(green, 0, 255);
	color.m_b = Math::Clamp(blue, 0, 255);

	return color;
}

Color &Color::operator=(const Color &other) {
	if (this != &other) {
		m_r = other.m_r;
		m_g = other.m_g;
		m_b = other.m_b;
		m_a = other.m_a;
	}

	return *this;
}

bool Color::operator!=(const Color &b) const {
	return ((m_r != b.m_r) || (m_g != b.m_g) || (m_b != b.m_b) || (m_a != b.m_a));
}
