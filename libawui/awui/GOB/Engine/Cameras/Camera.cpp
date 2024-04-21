#include "Camera.h"

using namespace awui::GOB::Engine;
using namespace awui::GOB::Engine::Cameras;

Camera::Camera() {
	m_upVector = Vector3(0, 1, 0);
	m_target = Vector3(0, 0, 0);
}

void Camera::LookAt(float x, float y, float z) {
	m_target.x = x;
	m_target.y = y;
	m_target.z = z;
}

Vector3 Camera::GetTarget() const {
	return m_target;
}

void Camera::SetUpVector(const Vector3 &newUpVector) {
	m_upVector = newUpVector;
}

Vector3 Camera::GetUpVector() const {
	return m_upVector;
}
