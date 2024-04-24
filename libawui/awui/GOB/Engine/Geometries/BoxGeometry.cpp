#include "BoxGeometry.h"

using namespace awui::GOB::Engine;

BoxGeometry::BoxGeometry(float width, float height, float depth, int widthSegments, int heightSegments, int depthSegments) {
	m_width = width;
	m_height = height;
	m_depth = depth;
	m_widthSegments = widthSegments;
	m_heightSegments = heightSegments;
	m_depthSegments = depthSegments;

	BuildPlane(Vector3::Z, Vector3::Y, Vector3::X, -1, -1, depth, height, width, m_depthSegments, m_heightSegments, 0);	 // X+
	BuildPlane(Vector3::Z, Vector3::Y, Vector3::X, 1, -1, depth, height, -width, m_depthSegments, m_heightSegments, 1);	 // X-
	BuildPlane(Vector3::X, Vector3::Z, Vector3::Y, 1, 1, width, depth, height, m_widthSegments, m_depthSegments, 2);	 // Y+
	BuildPlane(Vector3::X, Vector3::Z, Vector3::Y, 1, -1, width, depth, -height, m_widthSegments, m_depthSegments, 3);	 // Y-
	BuildPlane(Vector3::X, Vector3::Y, Vector3::Z, 1, -1, width, height, depth, m_widthSegments, m_heightSegments, 4);	 // Z+
	BuildPlane(Vector3::X, Vector3::Y, Vector3::Z, -1, -1, width, height, -depth, m_widthSegments, m_heightSegments, 5); // Z-
}

void BoxGeometry::BuildPlane(int u, int v, int w, int udir, int vdir, float width, float height, float depth, int widthSegments, int heightSegments, int materialIndex) {
	float faceFixed = depth / 2.0f;
	float faceWidthHalf = width / 2.0f;
	float faceHeightHalf = height / 2.0f;
	float faceSegmentWidth = width / (float) widthSegments;
	float faceSegmentHeight = height / (float) heightSegments;

	int numVertices = m_vertices.size();

	for (int iy = 0; iy <= heightSegments; iy++) {
		for (int ix = 0; ix <= widthSegments; ix++) {
			float x = ix * faceSegmentWidth - faceWidthHalf;
			float y = iy * faceSegmentHeight - faceHeightHalf;

			Vector3 vec;
			vec.data[u] = x * udir;
			vec.data[v] = y * vdir;
			vec.data[w] = faceFixed;
			m_vertices.push_back(vec);

			m_uvs.push_back(Vector3((float) ix / (float) widthSegments, ((float) iy / (float) heightSegments), materialIndex));
		}
	}

	for (int iy = 0; iy < heightSegments; iy++) {
		for (int ix = 0; ix < widthSegments; ix++) {
			float a = numVertices + ix + (widthSegments + 1.0f) * iy;
			float b = numVertices + ix + (widthSegments + 1.0f) * (iy + 1.0f);
			float c = numVertices + (ix + 1.0f) + (widthSegments + 1.0f) * (iy + 1.0f);
			float d = numVertices + (ix + 1.0f) + (widthSegments + 1.0f) * iy;

			m_indices.push_back(TriangleIndices(a, b, d));
			m_indices.push_back(TriangleIndices(b, c, d));
		}
	}
}
