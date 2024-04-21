#include "Matrix4.h"

#include <awui/GOB/Engine/Math/Quaternion.h>
#include <awui/GOB/Engine/Math/Vector3.h>

using namespace awui::GOB::Engine;

Matrix4::Matrix4() {
	for (int i = 0; i < 16; i++) {
		m[i] = 0;
	}

	m[0] = m[5] = m[10] = m[15] = 1;
}

void Matrix4::Set(float n11, float n12, float n13, float n14, float n21, float n22, float n23, float n24, float n31, float n32, float n33, float n34, float n41, float n42, float n43, float n44) {
	m[0] = n11;
	m[4] = n12;
	m[8] = n13;
	m[12] = n14;
	m[1] = n21;
	m[5] = n22;
	m[9] = n23;
	m[13] = n24;
	m[2] = n31;
	m[6] = n32;
	m[10] = n33;
	m[14] = n34;
	m[3] = n41;
	m[7] = n42;
	m[11] = n43;
	m[15] = n44;
}

float *Matrix4::operator[](int index) {
	return &m[index * 4];
}

const float *Matrix4::data() const {
	return m;
}

void Matrix4::Compose(const Vector3 &position, const Quaternion &rotation, const Vector3 &scale) {
	float x = rotation.x, y = rotation.y, z = rotation.z, w = rotation.w;
	float x2 = x + x, y2 = y + y, z2 = z + z;
	float xx = x * x2, xy = x * y2, xz = x * z2;
	float yy = y * y2, yz = y * z2, zz = z * z2;
	float wx = w * x2, wy = w * y2, wz = w * z2;

	float sx = scale.x, sy = scale.y, sz = scale.z;

	m[0] = (1 - (yy + zz)) * sx;
	m[1] = (xy - wz) * sx;
	m[2] = (xz + wy) * sx;

	m[4] = (xy + wz) * sy;
	m[5] = (1 - (xx + zz)) * sy;
	m[6] = (yz - wx) * sy;

	m[8] = (xz - wy) * sz;
	m[9] = (yz + wx) * sz;
	m[10] = (1 - (xx + yy)) * sz;

	m[12] = position.x;
	m[13] = position.y;
	m[14] = position.z;

	m[3] = m[7] = m[11] = m[15] = 0;
	m[15] = 1;
}