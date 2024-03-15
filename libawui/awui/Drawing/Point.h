#pragma once

#include <awui/Object.h>

namespace awui::Drawing {
	class Size;

	class Point : public Object {
	  private:
		float m_x;
		float m_y;

	  public:
		Point();
		Point(const Size sz);
		Point(float x, float y);
		virtual ~Point();

		inline float GetX() const { return m_x; }
		inline float GetLeft() const { return m_x; }
		inline float GetRight() const { return m_x; }
		void SetX(float x);

		inline float GetY() const { return m_y; }
		inline float GetTop() const { return m_y; }
		inline float GetBottom() const { return m_y; }
		void SetY(float y);

		Point &operator=(const Point &other);

		virtual String ToString() const override;

		static float Distance(Point *p1, Point *p2);
	};
} // namespace awui::Drawing
