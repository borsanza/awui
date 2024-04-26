#include "Vector3.h"

#include <awui/GOB/Engine/Math/Matrix4.h>
#include <awui/Math.h>

using namespace awui::GOB::Engine;

Vector3 Vector3::operator+(const Vector3 &other) const {
	return Vector3(m_x + other.x, m_y + other.y, m_z + other.z);
}

Vector3 Vector3::operator-(const Vector3 &other) const {
	return Vector3(m_x - other.x, m_y - other.y, m_z - other.z);
}

/*
Vector3 Vector3::operator*(const Matrix4 &other) const {
	float px = m_x * other.m[0] + m_y * other.m[4] + m_z * other.m[8] + other.m[12];
	float py = m_x * other.m[1] + m_y * other.m[5] + m_z * other.m[9] + other.m[13];
	float pz = m_x * other.m[2] + m_y * other.m[6] + m_z * other.m[10] + other.m[14];
	float pw = m_x * other.m[3] + m_y * other.m[7] + m_z * other.m[11] + other.m[15];

	if (w != 1.0f) {
		return Vector3(px / pw, py / pw, pz / pw);
	}

	return Vector3(px, py, pz);
}
*/
Vector3 Vector3::Cross(const Vector3 &a, const Vector3 &b) {
	return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

float Vector3::Dot(const Vector3 &a, const Vector3 &b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

void Vector3::Normalize() {
	float length = Math::Sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
	m_x /= length;
	m_y /= length;
	m_z /= length;
}

void Vector3::ApplyTransform(const Matrix4 &transform) {
	t_x = m_x * transform.m[0] + m_y * transform.m[4] + m_z * transform.m[8] + transform.m[12];
	t_y = m_x * transform.m[1] + m_y * transform.m[5] + m_z * transform.m[9] + transform.m[13];
	t_z = m_x * transform.m[2] + m_y * transform.m[6] + m_z * transform.m[10] + transform.m[14];
	float pw = m_x * transform.m[3] + m_y * transform.m[7] + m_z * transform.m[11] + transform.m[15];

	if (pw != 1.0f) {
		t_x /= pw;
		t_y /= pw;
		t_z /= pw;
	}

	m_dirtyTransform = false;
}

void Vector3::Set(float x, float y, float z) {
	m_x = x;
	m_y = y;
	m_z = z;
}

Vector3 Vector3::Normalize(const Vector3 &a) {
	float length = Math::Sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	return Vector3(a.x / length, a.y / length, a.z / length);
}
