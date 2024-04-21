#pragma once

namespace awui::GOB::Engine {
	class Material {
	  public:
		virtual ~Material() = default;

		virtual void ApplyMaterial();
		virtual void ApplyUVs(float u, float v);
	};
} // namespace awui::GOB::Engine