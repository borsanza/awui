#pragma once

#include <awui/Object.h>

namespace awui {
	namespace Drawing {
		class Color : public Object {
			private:
				unsigned char a, r, g, b;

			public:
				Color();

				virtual bool IsClass(Classes objectClass) const override;
				virtual String ToString() const override;

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

				bool operator !=(const Color &b) const;
		};
	}
}
