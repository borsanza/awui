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
	float px = m_x * other.m_elements[0] + m_y * other.m_elements[4] + m_z * other.m_elements[8] + other.m_elements[12];
	float py = m_x * other.m_elements[1] + m_y * other.m_elements[5] + m_z * other.m_elements[9] + other.m_elements[13];
	float pz = m_x * other.m_elements[2] + m_y * other.m_elements[6] + m_z * other.m_elements[10] + other.m_elements[14];
	float pw = m_x * other.m_elements[3] + m_y * other.m_elements[7] + m_z * other.m_elements[11] + other.m_elements[15];

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
	t_x = m_x * transform.m_elements[0] + m_y * transform.m_elements[4] + m_z * transform.m_elements[8] + transform.m_elements[12];
	t_y = m_x * transform.m_elements[1] + m_y * transform.m_elements[5] + m_z * transform.m_elements[9] + transform.m_elements[13];
	t_z = m_x * transform.m_elements[2] + m_y * transform.m_elements[6] + m_z * transform.m_elements[10] + transform.m_elements[14];
	float pw = m_x * transform.m_elements[3] + m_y * transform.m_elements[7] + m_z * transform.m_elements[11] + transform.m_elements[15];

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
