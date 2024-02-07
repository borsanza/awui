#pragma once

namespace awui {
	class String;

	namespace Drawing {
		class Size;

		class Point {
			private:
				int _x;
				int _y;

			public:
				Point();
				Point(const Size sz);
				Point(int x, int y);
				virtual ~Point();

				inline int GetX() const { return this->_x; }
				inline int GetLeft() const { return this->_x; }
				inline int GetRight() const { return this->_x; }
				void SetX(int x);

				inline int GetY() const { return this->_y; }
				inline int GetTop() const { return this->_y; }
				inline int GetBottom() const { return this->_y; }
				void SetY(int y);

				Point & operator= (const Point & other);

				virtual String ToString();

				static float Distance(Point * p1, Point * p2);
		};
	}
}
