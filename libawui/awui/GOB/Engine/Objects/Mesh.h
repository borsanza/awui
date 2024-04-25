#pragma once

#include <awui/GOB/Engine/Core/Object3D.h>
#include <unordered_set>
#include <vector>

namespace awui::GOB::Engine {
	class BufferGeometry;
	class Material;

	class Mesh : public Object3D {
	  private:
		BufferGeometry *m_geometry;
		std::vector<Material *> m_materials;

	  public:
		Mesh(BufferGeometry *geometry, const std::vector<Material *> &materials);

		virtual void FillMaterialList(std::unordered_set<Material *> &materials) override;
		virtual void Render(const Material &material, const Matrix4 &transform) override;
	};
} // namespace awui::GOB::Engine
