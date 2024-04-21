#include "Color.h"

#include <awui/Math.h>

using namespace awui::GOB::Engine;

Color::Color(uint32_t color) {
	m_r = (color >> 24) & 0xFF;
	m_g = (color >> 16) & 0xFF;
	m_b = (color >> 8) & 0xFF;
	m_a = (color >> 0) & 0xFF;
}

Color::Color(float r, float g, float b, float a) {
	m_r = int(Math::Min(Math::Max(r, 0.0f), 1.0f) * 255.0f) << 24;
	m_g = int(Math::Min(Math::Max(g, 0.0f), 1.0f) * 255.0f) << 16;
	m_b = int(Math::Min(Math::Max(b, 0.0f), 1.0f) * 255.0f) << 8;
	m_a = int(Math::Min(Math::Max(a, 0.0f), 1.0f) * 255.0f) << 0;
}
