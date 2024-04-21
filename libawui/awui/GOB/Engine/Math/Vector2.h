#pragma once

namespace awui::GOB::Engine {
	struct Vector2 {
	  public:
		union {
			struct {
				float m_x, m_y;
			};
			struct {
				float x, y;
			};
			struct {
				float u, v;
			};
			float data[2];
		};

		Vector2(float x = 0, float y = 0) : x(x), y(y) {}

		Vector2 operator-(const Vector2 &other) const;

		void Set(float x, float y);
	};
} // namespace awui::GOB::Engine
