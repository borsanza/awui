#include "Quaternion.h"

#include <awui/GOB/Engine/Math/Vector3.h>
#include <awui/Math.h>

using namespace awui::GOB::Engine;

void Quaternion::Set(float x, float y, float z, float w) {
	m_x = x;
	m_y = y;
	m_z = z;
	m_w = w;
}

void Quaternion::SetFromEuler(const Vector3 &euler) {
	float c1 = Math::Cos(euler.m_x / 2.0f);
	float c2 = Math::Cos(euler.m_y / 2.0f);
	float c3 = Math::Cos(euler.m_z / 2.0f);

	float s1 = Math::Sin(euler.m_x / 2.0f);
	float s2 = Math::Sin(euler.m_y / 2.0f);
	float s3 = Math::Sin(euler.m_z / 2.0f);

	m_x = s1 * c2 * c3 + c1 * s2 * s3;
	m_y = c1 * s2 * c3 - s1 * c2 * s3;
	m_z = c1 * c2 * s3 + s1 * s2 * c3;
	m_w = c1 * c2 * c3 - s1 * s2 * s3;
}
