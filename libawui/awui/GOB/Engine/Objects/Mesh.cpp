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
	std::vector<Vector3> &vertices = m_geometry->m_vertices;
	const std::vector<TriangleIndices> &indices = m_geometry->m_indices;
	const std::vector<Vector3> &uvs = m_geometry->m_uvs;
	Material *material, *lastMaterial = nullptr;
	const Vector3 *uv;
	Vector3 vector;

	glBegin(GL_TRIANGLES);
	for (const TriangleIndices &triangle : indices) {
		for (int iTriangleVertex = 0; iTriangleVertex < 3; iTriangleVertex++) {
			int triangleIndex = triangle.v[iTriangleVertex];

			uv = &uvs[triangleIndex];

			material = m_materials[uv->data[2]];
			if (material != lastMaterial) {
				material->ApplyMaterial();
				lastMaterial = material;
			}
			material->ApplyUVs(uv);

			Vector3 *vector = &vertices[triangleIndex];
			if (vector->IsDirtyTransform()) {
				vector->ApplyTransform(transform);
			}
			glVertex3f(vector->t_x, vector->t_y, vector->t_z);
		}
	}

	glEnd();
}
