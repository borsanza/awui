// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIRECTANGLE_H__
#define __AWUIRECTANGLE_H__

#include <awui/Drawing/Point.h>
#include <awui/Drawing/Size.h>

namespace awui {
	namespace Drawing {
		class Rectangle {
		private:
			Point location;
			Size size;

		public:
			Rectangle();
			Rectangle(const Point location, const Size size);
			Rectangle(int x, int y, int width, int height);

			int GetWidth() const;
			void SetWidth(int width);

			int GetHeight() const;
			void SetHeight(int height);

			int GetX() const;
			void SetX(int x);

			int GetY() const;
			void SetY(int y);

			int GetBottom() const;
			int GetLeft() const;
			int GetRight() const;
			int GetTop() const;
			Point GetLocation() const;
			Size GetSize() const;
			void SetLocation(const Point location);
			void SetSize(const Size size);

			void Inflate(const Size size);
			void Inflate(int width, int height);

			void Offset(const Point pos);
			void Offset(int x, int y);

			static Rectangle FromLTRB(int left, int top, int right, int bottom);
			static Rectangle Intersect(const Rectangle rectangle1, const Rectangle rectangle2);
			void Intersect(const Rectangle rectangle);

			Rectangle & operator=(const Rectangle & other);
		};
	}
}

#endif
