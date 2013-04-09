// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUICOLOR_H__
#define __AWUICOLOR_H__

#include <awui/Object.h>

namespace awui {
	namespace Drawing {
		class Color : public Object {
		private:
			unsigned char a, r, g, b;

		public:
			Color();

			virtual int IsClass(Classes objectClass) const;

			unsigned char GetA() const;
			unsigned char GetR() const;
			unsigned char GetG() const;
			unsigned char GetB() const;
			int ToArgb() const;
			float GetBrightness() const;
			float GetHue() const;
			float GetSaturation() const;

			static Color FromArgb(int argb);
			static Color FromArgb(int alpha, Color baseColor);
			static Color FromArgb(int red, int green, int blue);
			static Color FromArgb(int alpha, int red, int green, int blue);

			Color & operator= (const Color & other);
		};
	}
}

#endif
