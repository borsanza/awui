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
	const std::vector<Vector3> &vertices = m_geometry->m_vertices;
	const std::vector<TriangleIndices> &indices = m_geometry->m_indices;
	const std::vector<Vector3> &uvs = m_geometry->m_uvs;
	Material *material, *lastMaterial = nullptr;
	Vector3 vector;

	glBegin(GL_TRIANGLES);

	// Recorro todos los triangulos
	for (const TriangleIndices &triangle : indices) {

		material = m_materials[(&uvs[0])->data[2]];
		if (material != lastMaterial) {
			glEnd();
			material->ApplyMaterial();
			glBegin(GL_TRIANGLES);
			lastMaterial = material;
		}

		// Recorro los tres vertices de un triangulo
		for (int iTriangleVertex = 0; iTriangleVertex < 3; iTriangleVertex++) {
			int triangleIndex = triangle.v[iTriangleVertex];

			material->ApplyUVs(&uvs[triangleIndex]);

			vector = vertices[triangleIndex] * transform;
			glVertex3f(vector.x, vector.y, vector.z);
		}
	}

	glEnd();
}
