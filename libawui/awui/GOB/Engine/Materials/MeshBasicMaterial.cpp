#include "MeshBasicMaterial.h"

#include <GL/glu.h>

using namespace awui::GOB::Engine;

MeshBasicMaterial::MeshBasicMaterial(uint32_t color, bool wireframe) : m_color(color), m_wireframe(wireframe) {
}

void MeshBasicMaterial::ApplyMaterial() {
	glColor3ub(m_color.GetR(), m_color.GetG(), m_color.GetB());
}
