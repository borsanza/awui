#pragma once

#include <awui/GOB/Engine/Core/Object3D.h>

namespace awui::GOB::Engine {
	// Los tres ejes como líneas desde el origen del objeto: X en rojo, Y en verde y Z en azul
	class AxesHelper : public Object3D {
	  private:
		float m_size;

	  public:
		AxesHelper(float size = 1.0f);

		virtual void Render(const Matrix4 &transform, RenderList &list) override;
	};
} // namespace awui::GOB::Engine
