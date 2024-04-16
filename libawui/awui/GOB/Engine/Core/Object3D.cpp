#include "Object3D.h"

#include <algorithm>

using namespace awui::GOB::Engine;

Object3D::~Object3D() {
	for (auto *child : _children) {
		delete child;
	}

	_children.clear();
}

void Object3D::Add(Object3D *object) {
	if (object) {
		_children.push_back(object);
	}
}

// Eliminar un objeto hijo del contenedor
void Object3D::Remove(Object3D *object) {
	if (object) {
		_children.erase(std::remove(_children.begin(), _children.end(), object), _children.end());
	}
}

void Object3D::SetPosition(const Vector3 &newPosition) {
	position = newPosition;
}

Vector3 Object3D::GetPosition() const {
	return position;
}
