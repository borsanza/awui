#pragma once

#include <awui/Drawing/Color.h>

namespace awui::GOB::Engine {
	class Texture;

	class Material {
	  public:
		virtual ~Material() = default;

		// Color de los vértices (multiplica a la textura, si la hay)
		virtual Drawing::Color GetColor() const;

		// Textura, o nullptr si solo tiene color
		virtual Texture *GetTexture() const;
	};
} // namespace awui::GOB::Engine
