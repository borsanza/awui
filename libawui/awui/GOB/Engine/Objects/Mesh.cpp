#include "Mesh.h"

#include <SDL_opengl.h>
#include <awui/Console.h>
#include <awui/GOB/Engine/Core/TriangleIndices.h>
#include <awui/GOB/Engine/Math/Matrix4.h>
#include <awui/GOB/Engine/Math/Quaternion.h>
#include <awui/GOB/Engine/Math/Vector3.h>
#include <vector>

using namespace awui::GOB::Engine;

Mesh::Mesh(BufferGeometry *geometry) {
	m_geometry = geometry;
}

void Mesh::Render() {
	Vector3 vector1, vector2, vector3;
	std::vector<Vector3> *vertices = &(m_geometry->m_vertices);

	// Console::WriteLine("%d\n", m_geometry->m_vertices.size());

	Quaternion quaternion;
	quaternion.SetFromEuler(rotation);

	Matrix4 matrix;
	matrix.Compose(position, quaternion, scale);

	// glBegin(GL_LINES);
	glBegin(GL_TRIANGLES);
	glColor3ub(255, 0, 0);
	for (auto triangle : m_geometry->m_indices) {
		vector1 = vertices->at(triangle.v1) * matrix;
		glVertex3f(vector1.x, vector1.y, vector1.z);

		vector2 = vertices->at(triangle.v2) * matrix;
		glVertex3f(vector2.x, vector2.y, vector2.z);

		vector3 = vertices->at(triangle.v3) * matrix;
		glVertex3f(vector3.x, vector3.y, vector3.z);
	}
	glEnd();
}
