#pragma once

#include "stdint.h"
#include <awui/Core/Color.h>
#include <awui/GOB/Engine/Materials/Material.h>

namespace awui::GOB::Engine {
	class MeshBasicMaterial : public Material {
	  private:
		Color m_color;
		bool m_wireframe;

	  public:
		MeshBasicMaterial(uint32_t color, bool wireframe = false);

		void ApplyMaterial() override;
	};
} // namespace awui::GOB::Engine
