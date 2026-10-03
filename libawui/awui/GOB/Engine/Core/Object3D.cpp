#include "Object3D.h"

#include <algorithm>
#include <awui/GOB/Engine/Math/Matrix4.h>
#include <awui/GOB/Engine/Math/Quaternion.h>

using namespace awui::GOB::Engine;

unsigned int Object3D::s_changes = 0;

unsigned int Object3D::GetChanges() {
	return s_changes;
}

Object3D::Object3D() {
	m_scale.Set(1, 1, 1);
	m_visible = true;
	m_dynamic = false;
}

Object3D::~Object3D() {
	for (auto *child : m_children) {
		delete child;
	}

	m_children.clear();
}

void Object3D::Changed() {
	if (!m_dynamic)
		s_changes++;
}

void Object3D::Add(Object3D *object) {
	if (object) {
		m_children.push_back(object);
		s_changes++;
	}
}

// Eliminar un objeto hijo del contenedor
void Object3D::Remove(Object3D *object) {
	if (object) {
		m_children.erase(std::remove(m_children.begin(), m_children.end(), object), m_children.end());
		s_changes++;
	}
}

void Object3D::SetPosition(const Vector3 &newPosition) {
	m_position = newPosition;
	Changed();
}

void Object3D::SetPosition(float x, float y, float z) {
	m_position.Set(x, y, z);
	Changed();
}

void Object3D::SetScale(float x, float y, float z) {
	m_scale.Set(x, y, z);
	Changed();
}

void Object3D::SetRotation(float x, float y, float z) {
	m_rotation.Set(x, y, z);
	Changed();
}

Vector3 Object3D::GetPosition() const {
	return m_position;
}

void Object3D::SetVisible(bool visible) {
	if (m_visible != visible) {
		m_visible = visible;
		Changed();
	}
}

void Object3D::SetDynamic(bool dynamic) {
	if (m_dynamic != dynamic) {
		m_dynamic = dynamic;
		s_changes++; // Pasa de una capa a otra
	}
}

void Object3D::Collect(const Matrix4 &parentMatrix, RenderList &list, bool dynamic, bool parentDynamic) {
	if (!m_visible)
		return;

	bool isDynamic = parentDynamic || m_dynamic;

	Matrix4 matrix;
	matrix.Compose(m_position, m_rotation.GetQuaternion(), m_scale);
	matrix = matrix * parentMatrix;

	if (isDynamic == dynamic)
		Render(matrix, list);

	for (auto *child : m_children) {
		child->Collect(matrix, list, dynamic, isDynamic);
	}
}

void Object3D::Render(const Matrix4 &transform, RenderList &list) {
}
