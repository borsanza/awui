#pragma once

namespace awui {
	class String;

	namespace Drawing {
		class Size;

		class Point {
			private:
				int _x;
				int _y;

				static int Sign(Point * p1, Point * p2, Point * p3);

			public:
				Point();
				Point(const Size sz);
				Point(int x, int y);

				inline int GetX() const { return this->_x; }
				void SetX(int x);

				inline int GetY() const { return this->_y; }
				void SetY(int y);

				Point & operator= (const Point & other);

				virtual String ToString();

				bool InTriangle(Point * v1, Point * v2, Point * v3);
		};
	}
}
