// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIPOINT_H__
#define __AWUIPOINT_H__

namespace awui {
	namespace Drawing {
		class Size;

		class Point {
		private:
			int x;
			int y;

		public:
			Point();
			Point(const Size sz);
			Point(int x, int y);

			int GetX() const;
			void SetX(int x);

			int GetY() const;
			void SetY(int y);

			Point & operator= (const Point & other);
		};
	}
}

#endif
