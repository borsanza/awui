#include "MeshBasicMaterial.h"

using namespace awui::GOB::Engine;

MeshBasicMaterial::MeshBasicMaterial(uint32_t color, bool wireframe) : m_color(color), m_wireframe(wireframe) {
	m_texture = NULL;
}

MeshBasicMaterial::MeshBasicMaterial(Texture *texture, bool wireframe) : m_texture(texture), m_wireframe(wireframe) {
	m_color = 0xffffffff;
}

awui::Drawing::Color MeshBasicMaterial::GetColor() const {
	// Opaco: el alfa del color no se usa (como con el glColor3ub de antes)
	return Drawing::Color::FromArgb(255, m_color.GetR(), m_color.GetG(), m_color.GetB());
}

Texture *MeshBasicMaterial::GetTexture() const {
	return m_texture;
}
