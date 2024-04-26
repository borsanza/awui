#pragma once

#include <cstdint>

namespace awui::GOB::Engine {
	class Vertex;
	class Material;

	struct Triangle {
		union {
			struct {
				Vertex *v1, *v2, *v3;
				int m_material_index;
			};
			struct {
				Vertex *v[3];
				int m_materials_index;
			};
		};

		Triangle(Vertex *v1, Vertex *v2, Vertex *v3, int material_index) : v1(v1), v2(v2), v3(v3), m_material_index(material_index) {}

		Vertex *Get(int index) const { return v[index]; }
	};
} // namespace awui::GOB::Engine
