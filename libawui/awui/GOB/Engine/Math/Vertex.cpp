#include "Vertex.h"

#include <awui/GOB/Engine/Math/Matrix4.h>

using namespace awui::GOB::Engine;

void Vertex::Set(float x, float y, float z, float u, float v) {
	m_x = x;
	m_y = y;
	m_z = z;
	m_u = u;
	m_v = v;
}

Vertex Vertex::operator*(const Matrix4 &other) const {
	float px = m_x * other.m[0] + m_y * other.m[4] + m_z * other.m[8] + other.m[12];
	float py = m_x * other.m[1] + m_y * other.m[5] + m_z * other.m[9] + other.m[13];
	float pz = m_x * other.m[2] + m_y * other.m[6] + m_z * other.m[10] + other.m[14];
	float pw = m_x * other.m[3] + m_y * other.m[7] + m_z * other.m[11] + other.m[15];

	if (pw != 1.0f) {
		return Vertex(px / pw, py / pw, pz / pw, m_u, m_v);
	}

	return Vertex(px, py, pz, m_u, m_v);
}