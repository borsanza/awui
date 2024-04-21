#include "Euler.h"

using namespace awui::GOB::Engine;

Euler::Euler(float x, float y, float z) {
	Set(x, y, z);
}

void Euler::Set(float x, float y, float z) {
	m_x = x;
	m_y = y;
	m_z = z;

	m_quaternion.SetFromEuler(*this);
}

Quaternion Euler::GetQuaternion() const {
	return m_quaternion;
}