#pragma once

#include <awui/Drawing/Point.h>
#include <awui/Drawing/Size.h>
#include <awui/Object.h>

namespace awui::Drawing {
	class Rectangle : public Object {
	  private:
		Point m_location;
		Size m_size;

	  public:
		Rectangle();
		Rectangle(const Point &location, const Size &size);
		Rectangle(float x, float y, float width, float height);
		virtual ~Rectangle();

		inline float GetWidth() const { return m_size.GetWidth(); }
		inline float GetHeight() const { return m_size.GetHeight(); }
		inline float GetX() const { return m_location.GetX(); }
		inline float GetY() const { return m_location.GetY(); }

		void SetWidth(float width);
		void SetHeight(float height);
		void SetX(float x);
		void SetY(float y);
		void SetLocation(const Point &location);
		void SetSize(const Size &size);

		float GetBottom() const;
		inline float GetLeft() const { return m_location.GetX(); }
		float GetRight() const;
		inline float GetTop() const { return m_location.GetY(); }
		Point GetLocation() const;
		Size GetSize() const;
		void SetLocation(float x, float y) { SetLocation(Point(x, y)); }
		void SetSize(float width, float height);
		void Inflate(const Size &size);
		void Inflate(float width, float height);
		void Offset(const Point &pos);
		void Offset(float x, float y);

		static Rectangle FromLTRB(float left, float top, float right, float bottom);
		static Rectangle Intersect(const Rectangle &rectangle1, const Rectangle &rectangle2);
		void Intersect(const Rectangle &rectangle);

		Rectangle &operator=(const Rectangle &other);

		virtual String ToString() const override;
	};
} // namespace awui::Drawing
