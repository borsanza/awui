#pragma once

namespace awui::GOB::Engine {
	class Matrix4;

	class Vertex {
	  public:
		static const int X = 0;
		static const int Y = 1;
		static const int Z = 2;
		static const int U = 3;
		static const int V = 4;

		union {
			struct {
				float m_x, m_y, m_z, m_u, m_v;
			};
			struct {
				float x, y, z, u, v;
			};
			float data[5];
		};

		Vertex(float x = 0, float y = 0, float z = 0, float u = 0, float v = 0) : m_x(x), m_y(y), m_z(z), m_u(u), m_v(v) {}

		void Set(float x, float y, float z, float u, float v);
		Vertex operator*(const Matrix4 &other) const;
	};
} // namespace awui::GOB::Engine
