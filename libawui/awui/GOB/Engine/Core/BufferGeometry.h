
#pragma once

#include <awui/GOB/Engine/Core/TriangleVertices.h>
#include <awui/GOB/Engine/Math/Vector2.h>
#include <awui/GOB/Engine/Math/Vector3.h>
#include <vector>

namespace awui::GOB::Engine {
	class BufferGeometry {
	  public:
		std::vector<Vector3> m_vertices;
		std::vector<TriangleVertices> m_indices;
		std::vector<Vector3> m_uvs;
	};
} // namespace awui::GOB::Engine