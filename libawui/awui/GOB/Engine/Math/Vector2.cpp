#include "Vector2.h"

using namespace awui::GOB::Engine;

Vector2 Vector2::operator-(const Vector2 &other) const {
	return Vector2(m_x - other.m_x, m_y - other.m_y);
}

void Vector2::Set(float x, float y) {
	m_x = x;
	m_y = y;
}
