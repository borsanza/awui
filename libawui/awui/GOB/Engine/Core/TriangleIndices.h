#pragma once

#include <cstdint>

namespace awui::GOB::Engine {
	struct TriangleIndices {
		union {
			struct {
				uint32_t v1, v2, v3;
			};
			uint32_t v[3];
		};

		TriangleIndices(int v1, int v2, int v3) : v1(v1), v2(v2), v3(v3) {}

		uint32_t Get(int index) const { return v[index]; }
	};
} // namespace awui::GOB::Engine
