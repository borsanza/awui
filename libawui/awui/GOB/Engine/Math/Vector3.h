#pragma once

namespace awui::GOB::Engine {
	class Vector3 {
	  public:
		float x, y, z;

		Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

		Vector3 operator-(const Vector3 &other) const;

		static Vector3 Cross(const Vector3 &a, const Vector3 &b);
		static float Dot(const Vector3 &a, const Vector3 &b);
		static Vector3 Normalize(const Vector3 &a);
		void Normalize();
	};
} // namespace awui::GOB::Engine
