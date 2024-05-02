#pragma once

#include "stdint.h"
#include <awui/Core/Color.h>
#include <awui/GOB/Engine/Materials/Material.h>

namespace awui::GOB::Engine {
	class Texture;

	class MeshBasicMaterial : public Material {
	  private:
		Color m_color;
		Texture *m_texture;
		bool m_wireframe;

	  public:
		MeshBasicMaterial(uint32_t color, bool wireframe = false);
		MeshBasicMaterial(Texture *texture, bool wireframe = false);

		void ApplyMaterial() override;
		void UnApplyMaterial() override;

		Texture *GetTexture() { return m_texture; }

		void ApplyUVs(const Vector3 *uv) override;
	};
} // namespace awui::GOB::Engine
