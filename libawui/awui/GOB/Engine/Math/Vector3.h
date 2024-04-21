#pragma once

namespace awui::GOB::Engine {
	class Matrix4;

	struct Vector3 {

	  public:
		static const int X = 0;
		static const int Y = 1;
		static const int Z = 2;

		union {
			struct {
				float m_x, m_y, m_z;
			};
			struct {
				float x, y, z;
			};
			struct {
				float u, v, w;
			};
			float data[3];
		};

		Vector3(float x = 0, float y = 0, float z = 0) : m_x(x), m_y(y), m_z(z) {}

		Vector3 operator+(const Vector3 &other) const;
		Vector3 operator-(const Vector3 &other) const;
		Vector3 operator*(const Matrix4 &other) const;

		void Set(float x, float y, float z);

		static Vector3 Cross(const Vector3 &a, const Vector3 &b);
		static float Dot(const Vector3 &a, const Vector3 &b);
		static Vector3 Normalize(const Vector3 &a);
		void Normalize();
	};
} // namespace awui::GOB::Engine
