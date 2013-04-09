// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUISIZE_H__
#define __AWUISIZE_H__

namespace awui {
	namespace Drawing {
		class Point;

		class Size {
		private:
			int width;
			int height;

		public:
			Size();
			Size(const Point pt);
			Size(int width, int height);

			int GetWidth() const;
			void SetWidth(int width);

			int GetHeight() const;
			void SetHeight(int height);

			Size & operator= (const Size & other);
		};
	}
}

#endif
