#pragma once

namespace awui::GOB::Engine {
	class Matrix4;

	class Vector3 {
	  public:
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

		struct {
			float t_x, t_y, t_z;
		};

		bool m_dirtyTransform;

		static const int X = 0;
		static const int Y = 1;
		static const int Z = 2;

		Vector3(float x = 0, float y = 0, float z = 0) : m_x(x), m_y(y), m_z(z), m_dirtyTransform(true) {}

		Vector3 operator+(const Vector3 &other) const;
		Vector3 operator-(const Vector3 &other) const;
		Vector3 operator*(const Matrix4 &other) const;

		void Set(float x, float y, float z);

		static Vector3 Cross(const Vector3 &a, const Vector3 &b);
		static float Dot(const Vector3 &a, const Vector3 &b);
		static Vector3 Normalize(const Vector3 &a);
		void Normalize();

		bool IsDirtyTransform() const { return m_dirtyTransform; }
		void ApplyTransform(const Matrix4 &transform);
	};
} // namespace awui::GOB::Engine
