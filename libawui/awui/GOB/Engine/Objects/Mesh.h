#pragma once

#include <awui/GOB/Engine/Core/BufferGeometry.h>
#include <awui/GOB/Engine/Core/Object3D.h>

namespace awui::GOB::Engine {
	class Mesh : public Object3D {
	  private:
		BufferGeometry *m_geometry;

	  public:
		Mesh(BufferGeometry *geometry);

		virtual void Render();
	};
} // namespace awui::GOB::Engine