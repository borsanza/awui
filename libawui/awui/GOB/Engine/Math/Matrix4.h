#pragma once

namespace awui::GOB::Engine {
	struct Quaternion;
	class Vector3;

	class Matrix4 {
		friend class Vector3;
		friend class Vertex;

	  private:
		float m[16];

	  public:
		Matrix4();

		bool IsIdentity() const;
		static Matrix4 Identity();

		Matrix4 operator*(const Matrix4 &other) const;

		float *operator[](int index);
		const float *data() const;

		void Set(float n11, float n12, float n13, float n14, float n21, float n22, float n23, float n24, float n31, float n32, float n33, float n34, float n41, float n42, float n43, float n44);

		void Compose(const Vector3 &position, const Quaternion &rotation, const Vector3 &scale);
	};
} // namespace awui::GOB::Engine
