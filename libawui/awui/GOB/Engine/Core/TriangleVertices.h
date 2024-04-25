#pragma once

#include <cstdint>

namespace awui::GOB::Engine {
	class Vector3;

	struct TriangleVertices {
		union {
			struct {
				Vector3 *v1, *v2, *v3;
			};
			Vector3 *v[3];
		};

		TriangleVertices(Vector3 *v1, Vector3 *v2, Vector3 *v3) : v1(v1), v2(v2), v3(v3) {}

		Vector3 *Get(int index) const { return v[index]; }
	};
} // namespace awui::GOB::Engine
