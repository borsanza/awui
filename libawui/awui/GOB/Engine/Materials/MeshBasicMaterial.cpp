#include "MeshBasicMaterial.h"

#include <GL/glu.h>
#include <awui/GOB/Engine/Math/Vector3.h>
#include <awui/GOB/Engine/Textures/Texture.h>

using namespace awui::GOB::Engine;

MeshBasicMaterial::MeshBasicMaterial(uint32_t color, bool wireframe) : m_color(color), m_wireframe(wireframe) {
	m_texture = NULL;
}

MeshBasicMaterial::MeshBasicMaterial(Texture *texture, bool wireframe) : m_texture(texture), m_wireframe(wireframe) {
	m_color = 0xffffffff;
}

void MeshBasicMaterial::ApplyMaterial() {
	if (m_texture == NULL) {
		glColor3ub(m_color.GetR(), m_color.GetG(), m_color.GetB());
	} else {
		glColor3f(1.0f, 1.0f, 1.0f);
		m_texture->BindTexture();
	}
}

void MeshBasicMaterial::UnApplyMaterial() {
	if (m_texture == NULL) {
	} else {
		m_texture->UnBindTexture();
	}
}

void MeshBasicMaterial::ApplyUVs(const Vector3 *uv) {
	if (m_texture == NULL) {
		return;
	}

	glTexCoord2f(uv->u, uv->v);
}
