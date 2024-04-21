#include "Mesh.h"

#include <SDL_opengl.h>
#include <awui/GOB/Engine/Core/BufferGeometry.h>
#include <awui/GOB/Engine/Core/TriangleIndices.h>
#include <awui/GOB/Engine/Materials/Material.h>
#include <awui/GOB/Engine/Math/Matrix4.h>
#include <awui/GOB/Engine/Math/Quaternion.h>
#include <awui/GOB/Engine/Math/Vector3.h>

#include <vector>

using namespace awui::GOB::Engine;

Mesh::Mesh(BufferGeometry *geometry, const std::vector<Material *> &materials) : m_geometry(geometry), m_materials(materials) {
}

void Mesh::Render(const Matrix4 &transform) {
	Vector3 vector;
	std::vector<Vector3> *vertices = &(m_geometry->m_vertices);
	TriangleIndices *triangle;
	Material *material, *lastMaterial;
	Vector3 *uvs;

	glBegin(GL_TRIANGLES);
	glColor3ub(255, 0, 0);

	for (int i = 0; i < m_geometry->m_indices.size(); i++) {
		triangle = &m_geometry->m_indices[i];

		for (int j = 0; j < 3; j++) {
			uvs = &m_geometry->m_uvs[triangle->Get(j)];
			material = m_materials[uvs->data[2]];
			if (material != lastMaterial) {
				material->ApplyMaterial();
				lastMaterial = material;
			}
			material->ApplyUVs(uvs->data[0], uvs->data[1]);
			vector = vertices->at(triangle->Get(j)) * transform;
			glVertex3f(vector.x, vector.y, vector.z);
		}
	}
	glEnd();
}
