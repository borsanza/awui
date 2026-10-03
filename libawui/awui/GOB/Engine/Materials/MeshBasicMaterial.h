#pragma once

#include "stdint.h"
#include <awui/Drawing/Color.h>
#include <awui/GOB/Engine/Materials/Material.h>

namespace awui::GOB::Engine {
	class Texture;

	class MeshBasicMaterial : public Material {
	  private:
		Drawing::Color m_color;
		Texture *m_texture;
		bool m_wireframe;

	  public:
		MeshBasicMaterial(uint32_t color, bool wireframe = false);
		MeshBasicMaterial(Texture *texture, bool wireframe = false);

		Drawing::Color GetColor() const override;
		Texture *GetTexture() const override;
	};
} // namespace awui::GOB::Engine
