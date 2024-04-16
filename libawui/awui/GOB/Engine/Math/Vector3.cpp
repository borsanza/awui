#include "Vector3.h"

#include <awui/Math.h>

using namespace awui::GOB::Engine;

Vector3 Vector3::operator-(const Vector3 &other) const {
	return Vector3(x - other.x, y - other.y, z - other.z);
}

Vector3 Vector3::Cross(const Vector3 &a, const Vector3 &b) {
	return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

float Vector3::Dot(const Vector3 &a, const Vector3 &b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

void Vector3::Normalize() {
	float length = Math::Sqrt(x * x + y * y + z * z);
	x /= length;
	y /= length;
	z /= length;
}

Vector3 Vector3::Normalize(const Vector3 &a) {
	float length = Math::Sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	return Vector3(a.x / length, a.y / length, a.z / length);
}
