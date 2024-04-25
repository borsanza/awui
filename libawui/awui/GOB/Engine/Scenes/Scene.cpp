#include "Scene.h"

using namespace awui::GOB::Engine;

void Scene::Add(Object3D *object) {
	Object3D::Add(object);

	object->PreFillMaterialList(m_materials);
}
