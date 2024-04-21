#pragma once

#include <cstdint>

namespace awui::GOB::Engine {
	struct TriangleIndices {
		uint32_t v1, v2, v3;

		TriangleIndices(int v1, int v2, int v3) : v1(v1), v2(v2), v3(v3) {}
	};
} // namespace awui::GOB::Engine
