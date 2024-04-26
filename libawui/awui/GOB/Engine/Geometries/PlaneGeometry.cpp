#include "PlaneGeometry.h"

#include <awui/GOB/Engine/Core/Triangle.h>
#include <awui/GOB/Engine/Math/Vertex.h>

using namespace awui::GOB::Engine;

PlaneGeometry::PlaneGeometry(float width, float height, int widthSegments, int heightSegments) {
	m_width = width;
	m_height = height;
	m_widthSegments = widthSegments;
	m_heightSegments = heightSegments;

	BuildPlane(Vertex::X, Vertex::Y, Vertex::Z, 1, -1, width, height, m_widthSegments, m_heightSegments); // Z+
}

void PlaneGeometry::BuildPlane(int u, int v, int w, int udir, int vdir, float width, float height, int widthSegments, int heightSegments) {
	float faceWidthHalf = width / 2.0f;
	float faceHeightHalf = height / 2.0f;
	float faceSegmentWidth = width / (float) widthSegments;
	float faceSegmentHeight = height / (float) heightSegments;

	int numVertices = m_vertices.size();

	for (int iy = 0; iy <= heightSegments; iy++) {
		for (int ix = 0; ix <= widthSegments; ix++) {
			float x = ix * faceSegmentWidth - faceWidthHalf;
			float y = iy * faceSegmentHeight - faceHeightHalf;

			Vertex vec;
			vec.data[u] = x * udir;
			vec.data[v] = y * vdir;
			vec.data[w] = 0;
			vec.data[Vertex::U] = (float) ix / (float) widthSegments;
			vec.data[Vertex::V] = 1.0f - ((float) iy / (float) heightSegments);
			m_vertices.push_back(&vec);
		}
	}

	for (int iy = 0; iy < heightSegments; iy++) {
		for (int ix = 0; ix < widthSegments; ix++) {
			float a = numVertices + ix + (widthSegments + 1.0f) * iy;
			float b = numVertices + ix + (widthSegments + 1.0f) * (iy + 1.0f);
			float c = numVertices + (ix + 1.0f) + (widthSegments + 1.0f) * (iy + 1.0f);
			float d = numVertices + (ix + 1.0f) + (widthSegments + 1.0f) * iy;

			m_triangles.push_back(new Triangle(m_vertices[a], m_vertices[b], m_vertices[d], 0));
			m_triangles.push_back(new Triangle(m_vertices[b], m_vertices[c], m_vertices[d], 0));
		}
	}
}
