// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIPOINT_H__
#define __AWUIPOINT_H__

namespace awui {
	class String;

	namespace Drawing {
		class Size;

		class Point {
			private:
				int x;
				int y;

				static int Sign(Point * p1, Point * p2, Point * p3);

			public:
				Point();
				Point(const Size sz);
				Point(int x, int y);

				int GetX() const;
				void SetX(int x);

				int GetY() const;
				void SetY(int y);

				Point & operator= (const Point & other);

				virtual String ToString();

				bool InTriangle(Point * v1, Point * v2, Point * v3);
		};
	}
}

#endif
