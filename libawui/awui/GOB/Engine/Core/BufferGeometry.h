
#pragma once

#include <vector>

namespace awui::GOB::Engine {
	class Vertex;
	class Triangle;

	class BufferGeometry {
	  public:
		std::vector<Vertex *> m_vertices;
		std::vector<Triangle *> m_triangles;
	};
} // namespace awui::GOB::Engine