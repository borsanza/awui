
#pragma once

#include <awui/GOB/Engine/Core/BufferGeometry.h>

namespace awui::GOB::Engine {
	class PlaneGeometry : public BufferGeometry {
	  private:
		float m_width;
		float m_height;
		int m_widthSegments;
		int m_heightSegments;

		void BuildPlane(int u, int v, int w, int udir, int vdir, float width, float height, int widthSegments, int heightSegments);

	  public:
		PlaneGeometry(float width = 1.0f, float height = 1.0f, int widthSegments = 1, int heightSegments = 1);
	};
} // namespace awui::GOB::Engine
