#pragma once

#include <awui/Object.h>

namespace awui {
	namespace Drawing {
		class ColorF : public Object {
			private:
				float a, r, g, b;

			public:
				ColorF();

				virtual int IsClass(Classes::Enum objectClass) const;
				virtual String ToString();

				float GetA() const;
				float GetR() const;
				float GetG() const;
				float GetB() const;
				int ToArgb() const;
				float GetBrightness() const;
				float GetHue() const;
				float GetSaturation() const;

				static ColorF FromArgb(int argb);
				static ColorF FromArgb(float alpha, ColorF baseColor);
				static ColorF FromArgb(float red, float green, float blue);
				static ColorF FromArgb(float alpha, float red, float green, float blue);

				ColorF & operator= (const ColorF & other);

				bool operator !=(const ColorF &b) const;
		};
	}
}
