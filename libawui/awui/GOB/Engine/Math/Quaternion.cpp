#include "Quaternion.h"

#include <awui/GOB/Engine/Math/Euler.h>
#include <awui/Math.h>

using namespace awui::GOB::Engine;

void Quaternion::Set(float x, float y, float z, float w) {
	m_x = x;
	m_y = y;
	m_z = z;
	m_w = w;
}

void Quaternion::SetFromEuler(const Euler &euler) {
	float ex = euler.GetX();
	float ey = euler.GetY();
	float ez = euler.GetZ();

	float c1 = Math::Cos(ex / 2.0f);
	float c2 = Math::Cos(ey / 2.0f);
	float c3 = Math::Cos(ez / 2.0f);

	float s1 = Math::Sin(ex / 2.0f);
	float s2 = Math::Sin(ey / 2.0f);
	float s3 = Math::Sin(ez / 2.0f);

	m_x = s1 * c2 * c3 + c1 * s2 * s3;
	m_y = c1 * s2 * c3 - s1 * c2 * s3;
	m_z = c1 * c2 * s3 + s1 * s2 * c3;
	m_w = c1 * c2 * c3 - s1 * s2 * s3;
}
