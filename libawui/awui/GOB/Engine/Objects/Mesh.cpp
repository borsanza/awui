#include "Mesh.h"

#include <SDL_opengl.h>
#include <awui/GOB/Engine/Core/BufferGeometry.h>
#include <awui/GOB/Engine/Core/Triangle.h>
#include <awui/GOB/Engine/Materials/Material.h>
#include <awui/GOB/Engine/Math/Matrix4.h>
#include <awui/GOB/Engine/Math/Quaternion.h>
#include <awui/GOB/Engine/Math/Vector3.h>
#include <awui/GOB/Engine/Math/Vertex.h>

#include <vector>

using namespace awui::GOB::Engine;

Mesh::Mesh(BufferGeometry *geometry, const std::vector<Material *> &materials) : m_geometry(geometry), m_materials(materials) {
}

void Mesh::Render(const Matrix4 &transform) {
	const std::vector<Triangle *> &triangles = m_geometry->m_triangles;
	Vertex vertex;
	Material *material, *lastMaterial = nullptr;

	// Recorro todos los triangulos
	for (const Triangle *triangle : triangles) {
		// Recorro los tres vertices de un triangulo
		for (const Vertex *iTriangleVertex : triangle->v) {

			material = m_materials[triangle->m_material_index];
			if (material != lastMaterial) {
				material->ApplyMaterial();
				lastMaterial = material;
			}
			// material->ApplyUVs(uv);

			vertex = *iTriangleVertex * transform;

			glVertex3f(vertex.x, vertex.y, vertex.z);
		}
	}
}
