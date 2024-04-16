#include "Camera.h"

using namespace awui::GOB::Engine;
using namespace awui::GOB::Engine::Cameras;

Camera::Camera() {
	upVector = Vector3(0, 1, 0);
}

void Camera::SetTarget(const Vector3 &newTarget) {
	target = newTarget;
}

Vector3 Camera::GetTarget() const {
	return target;
}

void Camera::SetUpVector(const Vector3 &newUpVector) {
	upVector = newUpVector;
}

Vector3 Camera::GetUpVector() const {
	return upVector;
}
