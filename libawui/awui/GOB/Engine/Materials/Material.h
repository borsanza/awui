#pragma once

namespace awui::GOB::Engine {
	class Vector3;

	class Material {
	  public:
		virtual ~Material() = default;

		virtual void ApplyMaterial();
		virtual void ApplyUVs(const Vector3 *uv);
	};
} // namespace awui::GOB::Engine