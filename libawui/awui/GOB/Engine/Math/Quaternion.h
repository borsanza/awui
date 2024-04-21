#pragma once

namespace awui::GOB::Engine {
	class Vector3;

	struct Quaternion {
	  public:
		static const int X = 0;
		static const int Y = 1;
		static const int Z = 2;
		static const int W = 3;

		union {
			struct {
				float m_x, m_y, m_z, m_w;
			};
			struct {
				float x, y, z, w;
			};
			float data[4];
		};

		Quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f) : m_x(x), m_y(y), m_z(z), m_w(w) {}

		void Set(float x, float y, float z, float w);
		void SetFromEuler(const Vector3 &euler);
	};
} // namespace awui::GOB::Engine
