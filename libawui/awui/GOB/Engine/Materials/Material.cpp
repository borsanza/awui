#include "Material.h"

using namespace awui::GOB::Engine;

awui::Drawing::Color Material::GetColor() const {
	return Drawing::Color::FromArgb(255, 255, 255, 255);
}

Texture *Material::GetTexture() const {
	return nullptr;
}
