
#include "PerspectiveCamera.h"

#include <GL/gl.h>
#include <awui/Math.h>
#include <cmath>

using namespace awui::GOB::Engine;
using namespace awui::GOB::Engine::Cameras;

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float nearPlane, float farPlane) : Camera() {
	m_fov = fov;
	m_aspect = aspect;
	m_near = nearPlane;
	m_far = farPlane;
}

void PerspectiveCamera::MakePerspectiveMatrix(Matrix4 &matrix, float fovY, float aspect, float nearPlane, float farPlane) {
	const float f = 1.0f / tan((fovY * Math::PI / 180.0f) / 2.0f); // Cotangente de la mitad del ángulo de campo de visión
	matrix[0][0] = f / aspect;
	matrix[1][1] = f;
	matrix[2][2] = (farPlane + nearPlane) / (nearPlane - farPlane);
	matrix[2][3] = -1.0f;
	matrix[3][2] = 2.0f * farPlane * nearPlane / (nearPlane - farPlane);
	matrix[3][3] = 0.0f;
}

void PerspectiveCamera::SetAspectRatio(float aspect) {
	m_aspect = aspect;
}

// Configurar la matriz de proyección
Matrix4 PerspectiveCamera::GetProjectionMatrix() {
	// glMatrixMode(GL_PROJECTION);

	Matrix4 matrix;
	MakePerspectiveMatrix(matrix, m_fov, m_aspect, m_near, m_far);
	return matrix;
	// glLoadMatrixf(matrix.data());
}

void PerspectiveCamera::MakeLookAtMatrix(Matrix4 &matrix, const Vector3 &eye, const Vector3 &target, const Vector3 &up) {
	// El frente es negativo
	Vector3 zaxis = eye - target;
	zaxis.Normalize();

	// El lateral
	Vector3 xaxis = Vector3::Cross(up, zaxis);
	xaxis.Normalize();

	// El verdadero 'up'
	Vector3 yaxis = Vector3::Cross(zaxis, xaxis);

	// Crear una matriz de vista
	matrix[0][0] = xaxis.x;
	matrix[1][0] = xaxis.y;
	matrix[2][0] = xaxis.z;
	matrix[3][0] = -Vector3::Dot(xaxis, eye);

	matrix[0][1] = yaxis.x;
	matrix[1][1] = yaxis.y;
	matrix[2][1] = yaxis.z;
	matrix[3][1] = -Vector3::Dot(yaxis, eye);

	matrix[0][2] = zaxis.x;
	matrix[1][2] = zaxis.y;
	matrix[2][2] = zaxis.z;
	matrix[3][2] = -Vector3::Dot(zaxis, eye);
}

// Configurar la matriz de vista
Matrix4 PerspectiveCamera::GetViewMatrix() {
	Matrix4 viewMatrix;
	MakeLookAtMatrix(viewMatrix, position, m_target, m_upVector);
	return viewMatrix;

	// Establecer el modo de matriz a GL_MODELVIEW para trabajar con la matriz de vista
	// glMatrixMode(GL_MODELVIEW);

	// Cargar la matriz de vista
	// glLoadMatrixf(viewMatrix.data());
}
