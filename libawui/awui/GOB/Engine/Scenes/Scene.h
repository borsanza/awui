#pragma once

#include <awui/GOB/Engine/Core/Object3D.h>
#include <unordered_set>

namespace awui::GOB::Engine {
	class Scene : public Object3D {
	  private:
		std::unordered_set<Material *> m_materials;

	  public:
		virtual void Add(Object3D *object);

		std::unordered_set<Material *> &GetMaterials() { return m_materials; }
	};
} // namespace awui::GOB::Engine
