#include "Matrix4.h"

#include <awui/GOB/Engine/Math/Quaternion.h>
#include <awui/GOB/Engine/Math/Vector3.h>

using namespace awui::GOB::Engine;

Matrix4::Matrix4() {
	for (int i = 0; i < 16; i++) {
		m_elements[i] = 0;
	}

	m_elements[0] = m_elements[5] = m_elements[10] = m_elements[15] = 1;
}

void Matrix4::Set(float n11, float n12, float n13, float n14, float n21, float n22, float n23, float n24, float n31, float n32, float n33, float n34, float n41, float n42, float n43, float n44) {
	m_elements[0] = n11;
	m_elements[4] = n12;
	m_elements[8] = n13;
	m_elements[12] = n14;
	m_elements[1] = n21;
	m_elements[5] = n22;
	m_elements[9] = n23;
	m_elements[13] = n24;
	m_elements[2] = n31;
	m_elements[6] = n32;
	m_elements[10] = n33;
	m_elements[14] = n34;
	m_elements[3] = n41;
	m_elements[7] = n42;
	m_elements[11] = n43;
	m_elements[15] = n44;
}

bool Matrix4::IsIdentity() const {
	return m_elements[0] == 1.0f && m_elements[1] == 0.0f && m_elements[2] == 0.0f && m_elements[3] == 0.0f && m_elements[4] == 0.0f && m_elements[5] == 1.0f && m_elements[6] == 0.0f && m_elements[7] == 0.0f && m_elements[8] == 0.0f && m_elements[9] == 0.0f && m_elements[10] == 1.0f && m_elements[11] == 0.0f && m_elements[12] == 0.0f && m_elements[13] == 0.0f && m_elements[14] == 0.0f && m_elements[15] == 1.0f;
}

float *Matrix4::operator[](int index) {
	return &m_elements[index * 4];
}

const float *Matrix4::data() const {
	return m_elements;
}

void Matrix4::Compose(const Vector3 &position, const Quaternion &rotation, const Vector3 &scale) {
	float x = rotation.x, y = rotation.y, z = rotation.z, w = rotation.w;
	float x2 = x + x, y2 = y + y, z2 = z + z;
	float xx = x * x2, xy = x * y2, xz = x * z2;
	float yy = y * y2, yz = y * z2, zz = z * z2;
	float wx = w * x2, wy = w * y2, wz = w * z2;

	float sx = scale.x, sy = scale.y, sz = scale.z;

	m_elements[0] = (1 - (yy + zz)) * sx;
	m_elements[1] = (xy + wz) * sx;
	m_elements[2] = (xz - wy) * sx;

	m_elements[4] = (xy - wz) * sy;
	m_elements[5] = (1 - (xx + zz)) * sy;
	m_elements[6] = (yz + wx) * sy;

	m_elements[8] = (xz + wy) * sz;
	m_elements[9] = (yz - wx) * sz;
	m_elements[10] = (1 - (xx + yy)) * sz;

	m_elements[12] = position.x;
	m_elements[13] = position.y;
	m_elements[14] = position.z;

	m_elements[3] = m_elements[7] = m_elements[11] = m_elements[15] = 0;
	m_elements[15] = 1;
}

Matrix4 Matrix4::Identity() {
	return Matrix4();
}

Matrix4 Matrix4::operator*(const Matrix4 &other) const {
	Matrix4 result;

	for (int row = 0; row < 16; row += 4) {
		for (int col = 0; col < 4; col++) {
			result.m_elements[row + col] = 0;
			for (int k = 0; k < 4; k++) {
				result.m_elements[row + col] += m_elements[row + k] * other.m_elements[(k << 2) + col];
			}
		}
	}

	return result;
}
