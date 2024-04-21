#pragma once

#include <awui/GOB/Engine/Math/Quaternion.h>

namespace awui::GOB::Engine {
	struct Euler {
	  private:
		union {
			struct {
				float m_x, m_y, m_z;
			};
			struct {
				float x, y, z;
			};
			float data[3];
		};

		Quaternion m_quaternion;

	  public:
		static const int X = 0;
		static const int Y = 1;
		static const int Z = 2;

		Euler(float x = 0, float y = 0, float z = 0);

		void Set(float x, float y, float z);
		inline float GetX() const { return m_x; }
		inline float GetY() const { return m_y; }
		inline float GetZ() const { return m_z; }

		Quaternion GetQuaternion() const;
	};
} // namespace awui::GOB::Engine
