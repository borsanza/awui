#pragma once

#include <awui/Drawing/Point.h>
#include <awui/Drawing/Size.h>

namespace awui {
	class String;

	namespace Drawing {
		class Rectangle {
			private:
				Point location;
				Size size;

			public:
				Rectangle();
				Rectangle(const Point location, const Size size);
				Rectangle(int x, int y, int width, int height);
				virtual ~Rectangle();

				inline int GetWidth() const { return this->size.GetWidth(); }
				void SetWidth(int width);

				inline int GetHeight() const { return this->size.GetHeight(); }
				void SetHeight(int height);

				inline int GetX() const { return this->location.GetX(); }
				void SetX(int x);

				inline int GetY() const { return this->location.GetY(); }
				void SetY(int y);

				int GetBottom() const;
				inline int GetLeft() const { return this->location.GetX(); }
				int GetRight() const;
				inline int GetTop() const { return this->location.GetY(); }
				Point GetLocation() const;

				Size GetSize() const;
				void SetLocation(const Point location);
				void SetLocation(int x, int y) { this->SetLocation(Point(x, y)); }
				void SetSize(const Size size);
				void SetSize(int width, int height);

				void Inflate(const Size size);
				void Inflate(int width, int height);

				void Offset(const Point pos);
				void Offset(int x, int y);

				static Rectangle FromLTRB(int left, int top, int right, int bottom);
				static Rectangle Intersect(const Rectangle rectangle1, const Rectangle rectangle2);
				void Intersect(const Rectangle rectangle);

				Rectangle & operator=(const Rectangle & other);

				virtual String ToString();
		};
	}
}
