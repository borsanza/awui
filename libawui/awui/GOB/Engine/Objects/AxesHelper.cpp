#include "AxesHelper.h"

#include <awui/GOB/Engine/Math/Matrix4.h>
#include <awui/GOB/Engine/Renderers/RenderList.h>

using namespace awui::GOB::Engine;

AxesHelper::AxesHelper(float size) {
	m_size = size;
}

void AxesHelper::Render(const Matrix4 &transform, RenderList &list) {
	const float *m = transform.data();

	// Cada eje va de su color al mismo tirando a naranja, verde claro o celeste (como el AxesHelper de three.js)
	static const uint8_t colors[3][2][3] = {
		{{255, 0, 0}, {255, 153, 0}},
		{{0, 255, 0}, {153, 255, 0}},
		{{0, 0, 255}, {0, 153, 255}},
	};

	RenderList::Vertex origin = {m[12], m[13], m[14], 0.0f, 0.0f, 0, 0, 0, 255};
	for (int axis = 0; axis < 3; axis++) {
		RenderList::Vertex from = origin;
		from.r = colors[axis][0][0];
		from.g = colors[axis][0][1];
		from.b = colors[axis][0][2];

		RenderList::Vertex to = origin;
		to.x += m[axis * 4] * m_size;
		to.y += m[axis * 4 + 1] * m_size;
		to.z += m[axis * 4 + 2] * m_size;
		to.r = colors[axis][1][0];
		to.g = colors[axis][1][1];
		to.b = colors[axis][1][2];

		list.AddLine(from, to);
	}
}
