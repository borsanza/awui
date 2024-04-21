#pragma once

#include <awui/GOB/Engine/Core/Object3D.h>
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

		virtual void Render(const Matrix4 &transform);
	};
} // namespace awui::GOB::Engine