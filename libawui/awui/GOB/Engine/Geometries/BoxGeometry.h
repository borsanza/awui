#pragma once

#include <awui/GOB/Engine/Core/BufferGeometry.h>

namespace awui::GOB::Engine {
	class BoxGeometry : public BufferGeometry {
	  private:
		float m_width;
		float m_height;
		float m_depth;
		int m_widthSegments;
		int m_heightSegments;
		int m_depthSegments;

		void BuildPlane(int u, int v, int w, int udir, int vdir, float width, float height, float depth, int widthSegments, int heightSegments, int materialIndex);

	  public:
		BoxGeometry(float width = 1.0f, float height = 1.0f, float depth = 1.0f, int widthSegments = 1, int heightSegments = 1, int depthSegments = 1);
	};
} // namespace awui::GOB::Engine
