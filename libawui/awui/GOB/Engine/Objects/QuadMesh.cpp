#include "QuadMesh.h"

#include <awui/GOB/Engine/Math/Matrix4.h>

using namespace awui::GOB::Engine;

void QuadMesh::AddQuad(Texture *texture, const float corners[4][5], const Drawing::Color &color) {
	Group *group = nullptr;
	for (Group &candidate : m_groups) {
		if (candidate.texture == texture) {
			group = &candidate;
			break;
		}
	}

	if (!group) {
		m_groups.push_back({texture, {}});
		group = &m_groups.back();
	}

	// Dos triángulos, en el sentido contrario a las agujas del reloj visto de frente
	static const int order[6] = {0, 2, 1, 2, 3, 1};
	for (int index : order) {
		const float *corner = corners[index];
		group->vertices.push_back({corner[0], corner[1], corner[2], corner[3], corner[4], color.GetR(), color.GetG(), color.GetB(), color.GetA()});
	}
}

int QuadMesh::GetQuadCount() const {
	size_t vertices = 0;
	for (const Group &group : m_groups)
		vertices += group.vertices.size();

	return (int) (vertices / 6);
}

void QuadMesh::Render(const Matrix4 &transform, RenderList &list) {
	const float *m = transform.data();
	bool identity = transform.IsIdentity();

	for (const Group &group : m_groups) {
		for (RenderList::Vertex vertex : group.vertices) {
			if (!identity) {
				float x = vertex.x, y = vertex.y, z = vertex.z;
				vertex.x = x * m[0] + y * m[4] + z * m[8] + m[12];
				vertex.y = x * m[1] + y * m[5] + z * m[9] + m[13];
				vertex.z = x * m[2] + y * m[6] + z * m[10] + m[14];
			}

			list.Add(group.texture, vertex);
		}
	}
}
